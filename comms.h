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
 */
class Comms : public QObject
{
     Q_OBJECT

signals:
        void lineAvailable(void);      //!< Emitted when a complete line arrives in the ring buffer.
    void DataReady(void);           //!< Emitted whenever new bytes are placed into the ring buffer.
    void ADCrecordingDone(void);    //!< Emitted when all ADC vectors have been received.
    void ADCvectorReady(void);      //!< Emitted when a single ADC vector is ready to consume.

public:
    explicit Comms(SettingsDialog *settings, QString Host, QStatusBar *statusbar);
    bool ConnectToMIPS();
    void DisconnectFromMIPS();
    bool SendCommand(QString name, QString message);
    bool SendCommand(QString message);
    QString SendMessage(QString name, QString message);
    QString SendMessage(QString message);
    QString SendMess(QString name, QString message);
    QString SendMess(QString message);
    bool    SendString(QString name, QString message);
    bool    SendString(QString message);
    void    writeData(const QByteArray &data);
    bool    openSerialPort();
    void    reopenSerialPort(void);
    void    reopenPort(void);
    void    closeSerialPort();
    void    waitforline(int timeout);
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
    bool isMIPS(QString port);
    bool isAMPS(QString port, QString baud);
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
    RingBuffer *mrb = nullptr;      //!< Optional secondary ring buffer for async (unsolicited) messages.
    QTimer pollTimer;

    // ADC buffer processing
    ADCreadStates ADCstate;          //!< Current state of the ADC binary data reception state machine.
    quint16 *ADCbuf;                //!< Caller-supplied buffer for incoming ADC sample data.
    int ADClen;                     //!< Number of ADC samples expected in the current transfer.
    QTimer *keepAliveTimer;

private:
    void    msDelay(int ms);
    bool    serialBusy = false;
    QSerialPort             *serial;
    SettingsDialog::Settings p;
    Properties              *properties;
    QString                  host;
    int                      major, minor;  //!< MIPS firmware version major.minor

public slots:
    void readData2RingBuffer(void);

private slots:
    void handleError(QSerialPort::SerialPortError error);
    void readData2ADCBuffer(void);
    void connected(void);
    void disconnected(void);
    void slotAboutToClose(void);
    void slotKeepAlive(void);
    void slotReconnect(void);
    void pollLoop(void);
};

#endif // COMMS

