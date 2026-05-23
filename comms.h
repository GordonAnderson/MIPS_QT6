// =============================================================================
// comms.h
//
// Comms — serial-port and TCP-socket communication layer for MIPS/AMPS devices.
// Handles connection management, command send/receive, ADC streaming, and
// binary file transfer (EEPROM, FLASH, SD-card files).
//
// Depends on:  settingsdialog.h, ringbuffer.h, properties.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 refactoring
//
// Refactored:  May 2026
//   - Comms object is designed to be moved to a dedicated QThread via
//     moveToThread(). All serial I/O occurs on that thread using the
//     blocking waitForReadyRead / waitForBytesWritten Qt calls.
//   - Public send methods (SendMessage, SendCommand, SendString) may be
//     called from any thread. They post work to the Comms thread via
//     QMetaObject::invokeMethod and block the caller via QWaitCondition
//     until the transaction completes. Caller API is unchanged.
//   - serialBusy bool replaced by QMutex + QWaitCondition.
//   - keepAliveTimer and reconnectTimer now parented to this object.
//   - errorOccurred connected with Qt::UniqueConnection to prevent
//     duplicate handler registration on reconnect.
//   - QThread::sleep() removed from handleError.
//   - ADC state machine local statics promoted to instance members.
//   - waitforline() no longer calls QApplication::processEvents().
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef COMMS_H
#define COMMS_H

#include <QtCore/QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QStatusBar>
#include <QMessageBox>
#include <QObject>
#include <QTimer>
#include <QApplication>
#include <QtNetwork/QTcpSocket>
#include <QFileInfo>
#include <QFileDialog>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "settingsdialog.h"
#include "ringbuffer.h"
#include "properties.h"


/*! \enum ADCreadStates
 * States for the ADC binary data reception state machine.
 */
enum ADCreadStates
{
    ADCdone,
    WaitingForHeader,
    ReadingHeader,
    ReadingData,
    ReadingTrailer
};

/*! \class Comms
 * \brief Serial-port and TCP-socket communication layer for MIPS/AMPS devices.
 *
 * Manages connection, command send/receive (with ACK/NAK and timeout retry),
 * ADC vector streaming, async message buffering, and binary file transfer.
 *
 * Move this object to a dedicated QThread before calling ConnectToMIPS():
 * \code
 *   QThread *commsThread = new QThread;
 *   comms->moveToThread(commsThread);
 *   commsThread->start();
 * \endcode
 *
 * The public Send* methods are thread-safe and may be called from any thread.
 * They block the caller until the transaction completes, but do not pump the
 * Qt event loop, so no re-entrant signal delivery can occur.
 */
class Comms : public QObject
{
    Q_OBJECT

signals:
    void lineAvailable(void);       //!< Emitted when a complete line arrives in the ring buffer.
    void DataReady(void);           //!< Emitted whenever new bytes are placed into the ring buffer.
    void ADCrecordingDone(void);    //!< Emitted when all ADC vectors have been received.
    void ADCvectorReady(void);      //!< Emitted when a single ADC vector is ready to consume.
    void statusMessage(QString message, int timeout = 0);
    void errorMessage(QString title, QString message);
    void infoMessage(QString title, QString message);


public:
    explicit Comms(SettingsDialog *settings, QString Host, QStatusBar *statusbar);
    bool    ConnectToMIPS();
    void    DisconnectFromMIPS();
    bool    SendCommand(QString name, QString message);
    bool    SendCommand(QString message);
    QString SendMessage(QString name, QString message);
    QString SendMessage(QString message);
    QString SendMess(QString name, QString message);
    QString SendMess(QString message);
    bool    SendString(QString name, QString message);
    bool    SendString(QString message);
    void    writeData(const QByteArray &data);
    bool    openSerialPort();
    void    reopenSerialPort(void);
    void    closeSerialPort();
    void    waitforline(int timeout);
    void    clearReceiveBuffer() {
        QMetaObject::invokeMethod(this, [this]() {
            QMutexLocker lock(&sendMutex);
            rb.clear();
        }, Qt::QueuedConnection);
    }
    char    getchar(void);
    void    GetADCbuffer(quint16 *ADCbuffer, int NumSamples);
    void    ADCrelease(void);
    void    GetMIPSfile(QString MIPSfile, QString LocalFile);
    void    PutMIPSfile(QString MIPSfile, QString LocalFile);
    void    GetEEPROM(QString FileName, QString Board, int Addr);
    void    PutEEPROM(QString FileName, QString Board, int Addr);
    void    GetARBFLASH(QString FileName);
    void    PutARBFLASH(QString FileName);
    void    enableAsyncMessages(bool enable);
    void    processAsyncMessages(void);
    QString getAsyncMessage(void);
    void    ARBupload(QString Faddress, QString FileName);
    bool    isConnected(void);
    void    readAvailableData2RingBuffer(void);
    QString getline(void);
    int     CalculateCRC(QByteArray fdata);
    void    GetMIPSnameAndVersion(void);
    QString MIPSname;
    QByteArray readall(void);
    bool    isMIPS(QString port);
    bool    isAMPS(QString port, QString baud);
    QTimer *reconnectTimer;

    // Accessors for members used by program.cpp, faims.cpp, and mips.cpp
    QSerialPort*             serialPort() const;
    void                     version(int &maj, int &min) const;
    void                     setProperties(Properties *prop);
    void                     setHost(const QString &h);
    void                     setSettings(const SettingsDialog::Settings &s);

    QStatusBar *sb;
    QTcpSocket client;
    bool client_connected;
    RingBuffer rb;
    RingBuffer *mrb = nullptr;  //!< Optional secondary ring buffer for async messages.
    QTimer pollTimer;

    // ADC buffer processing
    ADCreadStates ADCstate;     //!< Current state of the ADC binary data reception state machine.
    quint16 *ADCbuf;            //!< Caller-supplied buffer for incoming ADC sample data.
    int ADClen;                 //!< Number of ADC samples expected in the current transfer.
    QTimer *keepAliveTimer;

private:
    void    msDelay(int ms);

    QSerialPort             *serial;
    SettingsDialog::Settings p;
    Properties              *properties;
    QString                  host;
    int                      major, minor;  //!< MIPS firmware version major.minor
    QAtomicInt portAlive;                   // 1 = connected, 0 = dead

    // -------------------------------------------------------------------------
    // Thread-safety primitives replacing the serialBusy bool.
    // sendMutex is held for the duration of each send transaction.
    // sendWait is used to block the calling thread until the worker slot
    // posts the result back into pendingResponse / pendingBool.
    // -------------------------------------------------------------------------
    QMutex        sendMutex;
    QWaitCondition sendWait;
    QString        pendingResponse;   //!< Result written by do* slots, read by Send* callers.
    bool           pendingBool;       //!< Result written by doSendCommand slot.

    // -------------------------------------------------------------------------
    // ADC state machine instance members (replaced static locals in
    // readData2ADCBuffer to avoid stale state across successive ADC sessions).
    // -------------------------------------------------------------------------
    quint8  adcLast    = 0;
    quint8 *adcB       = nullptr;
    int     adcDataPtr = 0;
    int     adcVlength = 0;
    int     adcVnum    = 0;
    bool    adcVlast   = false;

public slots:
    void readData2RingBuffer(void);
    void reopenPort(void);

private slots:
    // -------------------------------------------------------------------------
    // Worker slots — always execute on the Comms thread.
    // Called via QMetaObject::invokeMethod(Qt::QueuedConnection) from the
    // public Send* methods so that all serial I/O is confined to one thread.
    // -------------------------------------------------------------------------
    void doSendCommand(QString message);
    void doSendMessage(QString message);
    void doSendString(QString message);

    void handleError(QSerialPort::SerialPortError error);
    void readData2ADCBuffer(void);
    void connected(void);
    void disconnected(void);
    void slotAboutToClose(void);
    void slotKeepAlive(void);
    void slotReconnect(void);
    void pollLoop(void);
};

#endif // COMMS_H
