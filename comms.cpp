// =============================================================================
// comms.cpp
//
// Comms — serial-port and TCP-socket communication layer for MIPS/AMPS devices.
// Handles connection, command send/receive (ACK/NAK + timeout retry), ADC
// streaming, async message buffering, and binary file transfer.
//
// Depends on:  comms.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 refactoring
//
// Refactored:  May 2026
//   See comms.h for a full description of the threading model and the
//   list of bugs fixed in this revision.
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include <QElapsedTimer>
#include <QtSerialPort/QtSerialPort>
#include <QStringView>
#include <qeventloop.h>
#include "comms.h"

// =============================================================================
// Construction
// =============================================================================

/*! \brief Comms::Comms
 * Initialises the serial port or TCP socket for communication with MIPS.
 * Connects all signals required for incoming data and connection-state events.
 *
 * After construction, move this object to a dedicated thread before calling
 * ConnectToMIPS():
 * \code
 *   QThread *t = new QThread;
 *   comms->moveToThread(t);
 *   t->start();
 * \endcode
 */
Comms::Comms(SettingsDialog *settings, QString Host, QStatusBar *statusbar)
    : QObject(nullptr)
{
    p = settings->settings();
    sb = statusbar;
    client_connected = false;
    host = Host;
    properties = nullptr;

    serial = new QSerialPort(this);

    // FIX: timers now parented to this object so they are destroyed with it
    // and their thread affinity follows moveToThread() correctly.
    keepAliveTimer = new QTimer(this);
    reconnectTimer = new QTimer(this);

    connect(&client,  &QTcpSocket::readyRead,    this, &Comms::readData2RingBuffer);
    connect(serial,   &QSerialPort::readyRead,   this, &Comms::readData2RingBuffer);
    connect(&client,  &QTcpSocket::connected,    this, &Comms::connected);
    connect(&client,  &QTcpSocket::disconnected, this, &Comms::disconnected);
    connect(&client,  &QIODevice::aboutToClose,  this, &Comms::slotAboutToClose);
    connect(keepAliveTimer, &QTimer::timeout,    this, &Comms::slotKeepAlive);
    connect(reconnectTimer, &QTimer::timeout,    this, &Comms::slotReconnect);
    connect(&pollTimer,     &QTimer::timeout,    this, &Comms::pollLoop);

    // (Qt::QueuedConnection ensures it crosses to the UI thread)
    connect(this, &Comms::statusMessage,sb, &QStatusBar::showMessage,Qt::QueuedConnection);

    connect(this, &Comms::errorMessage, this, [](QString t, QString m){
        QMessageBox::critical(nullptr, t, m);
    }, Qt::QueuedConnection);

    connect(this, &Comms::infoMessage, this, [](QString t, QString m){
        QMessageBox::information(nullptr, t, m);
    }, Qt::QueuedConnection);

    portAlive.storeRelaxed(1);
}

// =============================================================================
// Accessors
// =============================================================================

/*! \brief Comms::serialPort
 * Returns the underlying QSerialPort pointer.
 */
QSerialPort* Comms::serialPort() const { return serial; }

/*! \brief Comms::version
 * Returns the connected MIPS firmware version as major/minor integers.
 */
void Comms::version(int &maj, int &min) const { maj = major; min = minor; }

/*! \brief Comms::setProperties */
void Comms::setProperties(Properties *prop) { properties = prop; }

/*! \brief Comms::setHost */
void Comms::setHost(const QString &h) { host = h; }

/*! \brief Comms::setSettings */
void Comms::setSettings(const SettingsDialog::Settings &s) { p = s; }

// =============================================================================
// Poll loop
// =============================================================================

/*! \brief Comms::pollLoop
 * Timer-driven poll: manually triggers readyRead on the serial port if bytes
 * are waiting. Runs on the Comms thread.
 */
void Comms::pollLoop(void)
{
    if(serial->isOpen() && serial->bytesAvailable() > 0)
        emit serial->readyRead();
}

// =============================================================================
// Utility
// =============================================================================

/*! \brief Comms::getchar
 * Returns the next byte from the ring buffer, or 0 if empty.
 */
char Comms::getchar(void)
{
    return rb.getch();
}

/*! \brief Comms::CalculateCRC
 * Computes an 8-bit CRC of the given byte array using the 0x1D generator
 * polynomial.
 */
int Comms::CalculateCRC(QByteArray fdata)
{
    unsigned char generator = 0x1D;
    unsigned char crc = 0;

    for(int i = 0; i < fdata.length(); i++)
    {
        crc ^= (unsigned char)fdata[i];
        for(int j = 0; j < 8; j++)
        {
            if((crc & 0x80) != 0) crc = ((crc << 1) ^ generator);
            else                  crc <<= 1;
        }
    }
    return crc;
}

/*! \brief Comms::msDelay
 * Blocking delay — safe to call from the Comms worker thread.
 */
void Comms::msDelay(int ms)
{
    QThread::msleep(ms);
}

// =============================================================================
// ADC streaming
// =============================================================================

/*! \brief Comms::GetADCbuffer
 * Arms ADC streaming mode. Redirects incoming data to the ADC state machine
 * until all vectors have been received.
 */
void Comms::GetADCbuffer(quint16 *ADCbuffer, int NumSamples)
{
    if(ADCbuffer == nullptr) return;
    ADCbuf     = ADCbuffer;
    ADClen     = NumSamples;
    ADCstate   = WaitingForHeader;

    // Reset ADC instance state so a fresh session starts cleanly.
    adcLast    = 0;
    adcB       = nullptr;
    adcDataPtr = 0;
    adcVlength = 0;
    adcVnum    = 0;
    adcVlast   = false;

    disconnect(&client, &QTcpSocket::readyRead,  nullptr, nullptr);
    disconnect(serial,  &QSerialPort::readyRead, nullptr, nullptr);
    connect(&client, &QTcpSocket::readyRead,  this, &Comms::readData2ADCBuffer);
    connect(serial,  &QSerialPort::readyRead, this, &Comms::readData2ADCBuffer);
}

/*! \brief Comms::ADCrelease
 * Releases ADC streaming mode and restores normal ring-buffer data processing.
 */
void Comms::ADCrelease(void)
{
    disconnect(&client, &QTcpSocket::readyRead,  nullptr, nullptr);
    disconnect(serial,  &QSerialPort::readyRead, nullptr, nullptr);
    connect(&client, &QTcpSocket::readyRead,  this, &Comms::readData2RingBuffer);
    connect(serial,  &QSerialPort::readyRead, this, &Comms::readData2RingBuffer);
    ADCbuf = nullptr;
    adcB   = nullptr;
}

/*! \brief Comms::readData2ADCBuffer
 * Slot: reads incoming bytes and feeds them through the ADC binary framing
 * state machine. Emits ADCvectorReady per vector and ADCrecordingDone when
 * finished.
 *
 * FIX: all state variables are now instance members instead of static locals
 * so that successive ADC sessions start with a clean state.
 *
 * FIX: the implicit fall-through between ReadingTrailer and ADCdone has been
 * replaced with an explicit goto to make the intent clear and prevent
 * accidental double-emit if the switch is ever edited.
 */
void Comms::readData2ADCBuffer(void)
{
    QByteArray data;

    if(ADCbuf == nullptr) return;
    if(client.isOpen()) data = client.readAll();
    if(serial->isOpen()) data = serial->readAll();

    for(int i = 0; i < data.size(); i++)
    {
        quint8 byte = (quint8)data[i];

        switch(ADCstate)
        {
        case WaitingForHeader:
            if((adcLast == 0x55) && (byte == 0xAA))
            {
                ADCstate   = ReadingHeader;
                adcDataPtr = 0;
                adcB       = (quint8 *)&adcVlength;
            }
            break;

        case ReadingHeader:
            if(adcB == nullptr) break;
            if(adcDataPtr == 0) adcB[0] = byte;
            if(adcDataPtr == 1) adcB[1] = byte;
            if(adcDataPtr == 2) { adcB[2] = byte; adcB = (quint8 *)&adcVnum; }
            if(adcDataPtr == 3) adcB[0] = byte;
            if(adcDataPtr == 4) adcB[1] = byte;
            if(adcDataPtr == 5) adcVlast = (byte == 0xFF);
            adcDataPtr++;
            if(adcDataPtr > 5)
            {
                adcDataPtr = 0;
                ADCstate   = ReadingData;
                adcB       = (quint8 *)ADCbuf;
            }
            break;

        case ReadingData:
            if(adcB == nullptr) break;
            adcB[adcDataPtr++] = byte;
            if(adcDataPtr >= (adcVlength * 2))
                ADCstate = ReadingTrailer;
            break;

        case ReadingTrailer:
            if((adcLast == 0xAE) && (byte == 0xEA))
            {
                emit ADCvectorReady();
                if(adcVlast)
                    goto adc_done;          // explicit, no fall-through ambiguity
                ADCstate   = WaitingForHeader;
                adcDataPtr = 0;
            }
            break;

        adc_done:
        case ADCdone:
            emit ADCrecordingDone();
            return;

        default:
            break;
        }

        adcLast = byte;
    }
}

// =============================================================================
// File transfer — SD card
// =============================================================================

/*! \brief Comms::GetMIPSfile
 * Downloads a file from the MIPS SD card and saves it locally.
 * Verifies the transfer with a CRC check.
 */
void Comms::GetMIPSfile(QString MIPSfile, QString LocalFile)
{
    bool ok = false;
    QString FileData;
    char c;
    int len;

    if(SendCommand("GET," + MIPSfile + "\n"))
    {
        waitforline(500);
        QString FileSize = getline();
        for(int i = 0; i < FileSize.toInt(); i += 1024)
        {
            len = 1024;
            if((FileSize.toInt() - i) < 1024) len = FileSize.toInt() - i;
            for(int j = 0; j < len * 2; j++)
            {
                // FIX: replaced processEvents() spin with waitForReadyRead
                while((c = getchar()) == 0)
                {
                    if(serial->isOpen()) serial->waitForReadyRead(100);
                    if(client.isOpen()) client.waitForReadyRead(100);
                    readData2RingBuffer();
                    if(!serial->isOpen() && !client.isOpen()) return;
                }
                if(c == '\n') break;
                FileData += QChar(c);
            }
            if((FileSize.toInt() - i) > 1024) SendString("Next\n");
        }
        // Drain the trailing byte
        {
            int guard = 0;
            while(getchar() == 0)
            {
                if(serial->isOpen()) serial->waitForReadyRead(100);
                if(client.isOpen()) client.waitForReadyRead(100);
                readData2RingBuffer();
                if(!serial->isOpen() && !client.isOpen()) return;
                if(++guard > 50) return;   // give up after 5 s
            }
        }
        if(FileData.length() != 2 * FileSize.toInt())
        {
            emit errorMessage(tr("File Error"), "Data block size not correct!");
            return;
        }
        waitforline(500);
        QString FileCRC = getline();
        QByteArray fdata;
        fdata.resize(FileSize.toInt());
        for(int i = 0; i < FileSize.toInt(); i++)
            fdata[i] = QStringView{FileData}.mid(i * 2, 2).toUInt(&ok, 16);

        if(CalculateCRC(fdata) == FileCRC.toInt())
        {
            QFile file(LocalFile);
            file.open(QIODevice::WriteOnly);
            file.write(fdata);
            file.close();
            emit infoMessage(tr("File saved"),
                                     "File from MIPS read and saved successfully!");
        }
        else
        {
            emit errorMessage(tr("File Error"), "CRC error!");
        }
    }
}

/*! \brief Comms::PutMIPSfile
 * Uploads a local file to the MIPS SD card in 1 KB chunks.
 * Appends a CRC for integrity verification.
 */
void Comms::PutMIPSfile(QString MIPSfile, QString LocalFile)
{
    QByteArray fdata;
    int fsize, len;
    QString dblock, res;

    QFile file(LocalFile);
    file.open(QIODevice::ReadOnly);
    fdata  = file.readAll();
    fsize  = (int)file.size();
    for(int i = 0; i < fsize; i++)
        dblock += QString().asprintf("%02x", (unsigned char)fdata[i]);
    file.close();

    if(SendCommand("PUT," + MIPSfile + "," + QString::number(fsize) + "\n"))
    {
        for(int i = 0; i < dblock.length(); i += 1024)
        {
            len = qMin(1024, dblock.length() - i);
            for(int k = 0; k < len; k += 128)
            {
                int chunk = qMin(128, len - k);
                SendString(dblock.mid(i + k, chunk));
                if(serial->isOpen()) serial->waitForBytesWritten();
                if(client.isOpen()) client.waitForBytesWritten();
                msDelay(5);
            }
            if((len == 1024) && ((dblock.length() - i) != 1024))
            {
                waitforline(1000);
                if((res = getline()) == "")
                {
                    emit errorMessage(tr("Data read error"),
                                         "Timedout waiting for data from MIPS!");
                    return;
                }
            }
        }
        SendString("\n");
        SendString(QString::number(CalculateCRC(fdata)) + "\n");
        emit infoMessage(tr("File saved"), "File sent to MIPS!");
    }
}

// =============================================================================
// File transfer — EEPROM
// =============================================================================

/*! \brief Comms::GetEEPROM
 * Reads the EEPROM of the named board module and saves the data locally.
 */
void Comms::GetEEPROM(QString FileName, QString Board, int Addr)
{
    bool ok = false;
    QString FileData;

    if(SendCommand("GETEEPROM," + Board + "," + QString().asprintf(",%x\n", Addr)))
    {
        waitforline(500);
        QString FileSize = getline();
        waitforline(1000);
        FileData = getline();
        if(FileData.length() != 2 * FileSize.toInt())
        {
            emit errorMessage(tr("File Error"), "Data block size not correct!");
            return;
        }
        waitforline(500);
        QString FileCRC = getline();
        QByteArray fdata;
        fdata.resize(FileSize.toInt());
        for(int i = 0; i < FileSize.toInt(); i++)
            fdata[i] = QStringView{FileData}.mid(i * 2, 2).toUInt(&ok, 16);

        if(CalculateCRC(fdata) == FileCRC.toInt())
        {
            QFile file(FileName);
            file.open(QIODevice::WriteOnly);
            file.write(fdata);
            file.close();
            emit infoMessage(tr("File saved"),
                                     "EEPROM from MIPS read and saved successfully!");
        }
        else
        {
            emit errorMessage(tr("File Error"), "CRC error!");
        }
    }
}

/*! \brief Comms::PutEEPROM
 * Writes a local file to the EEPROM of the named board module.
 */
void Comms::PutEEPROM(QString FileName, QString Board, int Addr)
{
    QString dblock;
    QByteArray fdata;

    if(SendCommand("PUTEEPROM," + Board + QString().asprintf(",%x\n", Addr)))
    {
        QFile file(FileName);
        file.open(QIODevice::ReadOnly);
        fdata = file.readAll();
        for(int i = 0; i < (int)file.size(); i++)
            dblock += QString().asprintf("%02x", (unsigned char)fdata[i]);
        SendString(QString::number(file.size()) + "\n");
        file.close();
        SendString(dblock + "\n");
        SendString(QString::number(CalculateCRC(fdata)) + "\n");
        emit infoMessage(tr("EEPROM write"), "MIPS module's EEPROM Written!");
    }
}

// =============================================================================
// File transfer — ARB FLASH
// =============================================================================

/*! \brief Comms::GetARBFLASH
 * Reads the ARB module FLASH memory and saves it to a local file.
 */
void Comms::GetARBFLASH(QString FileName)
{
    bool ok = false;
    QString FileData;

    if(SendCommand("GETFLASH\n"))
    {
        waitforline(500);
        QString FileSize = getline();
        waitforline(1000);
        FileData = getline();
        if(FileData.length() != 2 * FileSize.toInt())
        {
            emit errorMessage(tr("File Error"), "Data block size not correct!");
            return;
        }
        waitforline(500);
        QString FileCRC = getline();
        QByteArray fdata;
        fdata.resize(FileSize.toInt());
        for(int i = 0; i < FileSize.toInt(); i++)
            fdata[i] = QStringView{FileData}.mid(i * 2, 2).toUInt(&ok, 16);

        if(CalculateCRC(fdata) == FileCRC.toInt())
        {
            QFile file(FileName);
            file.open(QIODevice::WriteOnly);
            file.write(fdata);
            file.close();
            emit infoMessage(tr("File saved"),
                                     "FLASH from ARB read and saved successfully!");
        }
        else
        {
            emit errorMessage(tr("File Error"), "CRC error!");
        }
    }
}

/*! \brief Comms::PutARBFLASH
 * Writes a local file to the ARB module FLASH memory.
 */
void Comms::PutARBFLASH(QString FileName)
{
    QString dblock;
    QByteArray fdata;

    if(SendCommand("PUTFLASH\n"))
    {
        QFile file(FileName);
        file.open(QIODevice::ReadOnly);
        fdata = file.readAll();
        for(int i = 0; i < (int)file.size(); i++)
            dblock += QString().asprintf("%02x", (unsigned char)fdata[i]);
        SendString(QString::number(file.size()) + "\n");
        file.close();
        SendString(dblock + "\n");
        SendString(QString::number(CalculateCRC(fdata)) + "\n");
        emit infoMessage(tr("FLASH write"), "ARB module FLASH Written!");
    }
}

/*! \brief Comms::ARBupload
 * Uploads a local file to the ARB module at the given flash address in
 * 512-byte chunks.
 */
void Comms::ARBupload(QString Faddress, QString FileName)
{
    QByteArray fdata;
    int fsize, len;
    QString dblock, res;

    QFile file(FileName);
    file.open(QIODevice::ReadOnly);
    fdata = file.readAll();
    fsize = (int)file.size();
    for(int i = 0; i < fsize; i++)
        dblock += QString().asprintf("%02x", (unsigned char)fdata[i]);
    file.close();

    if(SendCommand("ARBPGM," + Faddress + "," + QString::number(fsize) + "\n"))
    {
        for(int i = 0; i < dblock.length(); i += 512)
        {
            len = qMin(512, dblock.length() - i);
            for(int k = 0; k < len; k += 128)
            {
                int chunk = qMin(128, len - k);
                SendString(dblock.mid(i + k, chunk));
                if(serial->isOpen()) serial->waitForBytesWritten();
                if(client.isOpen()) client.waitForBytesWritten();
                msDelay(10);
            }
            if(len == 512)
            {
                waitforline(2000);
                if((res = getline()) == "")
                {
                    emit errorMessage(tr("Data read error"),
                                         "Timedout waiting for data from ARB!");
                    return;
                }
            }
        }
        SendString("\n");
        SendString(QString::number(CalculateCRC(fdata)) + "\n");
        emit infoMessage(tr("File saved"), "File uploaded to ARB FLASH!");
    }
}

// =============================================================================
// Async message buffer
// =============================================================================

/*! \brief Comms::enableAsyncMessages
 * Enables or disables asynchronous (unsolicited) message buffering.
 */
void Comms::enableAsyncMessages(bool enable)
{
    if(enable)
    {
        if(mrb == nullptr) mrb = new RingBuffer;
    }
    else
    {
        delete mrb;
        mrb = nullptr;
    }
}

/*! \brief Comms::processAsyncMessages
 * Drains non-ACK/NAK lines from the main ring buffer into the async ring
 * buffer. Called at the start of each send transaction to flush unsolicited
 * messages before sending a new command.
 */
void Comms::processAsyncMessages(void)
{
    bool acknak = false;

    if(mrb == nullptr) return;
    while(true)
    {
        waitforline(-1);
        if(rb.numLines() > 0)
        {
            QString res = rb.getline(&acknak);
            if(acknak == false) mrb->putString(res + "\n");
        }
        else break;
    }
}

/*! \brief Comms::getAsyncMessage
 * Returns the next unsolicited message from the async ring buffer, or "".
 */
QString Comms::getAsyncMessage(void)
{
    if(mrb == nullptr) return "";
    return mrb->getline();
}

// =============================================================================
// Public send methods — thread-safe entry points
//
// Each public Send* method may be called from any thread (including the UI
// thread). It posts the real work to the Comms thread via invokeMethod
// (QueuedConnection) and then blocks on sendWait until the worker slot
// signals completion. The mutex ensures only one transaction runs at a time.
//
// Because we block on QWaitCondition::wait() rather than calling
// QApplication::processEvents(), the Qt event loop is NOT re-entered and no
// re-entrant signal can fire during the wait.
// =============================================================================

// -----------------------------------------------------------------------------
// SendString
// -----------------------------------------------------------------------------

/*! \brief Comms::SendString (named overload) */
bool Comms::SendString(QString name, QString message)
{
    if((name == MIPSname) || (name == "")) return SendString(message);
    return false;
}

/*! \brief Comms::SendString
 * Writes a raw string to the open serial port or TCP socket without waiting
 * for a response. Thread-safe: posts to the Comms thread and blocks until done.
 */
bool Comms::SendString(QString message)
{
    if(QThread::currentThread() == qApp->thread())
    {
        if(!portAlive.loadRelaxed()) return false;
        qWarning() << "WARNING: SendString called from UI thread with live port:"
                   << message << "- this may block the UI thread";
    }
    QMutexLocker lock(&sendMutex);

    if(!serial->isOpen() && !client.isOpen())
    {
        if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + " SS Disconnected!", 2000);
        else                    sb->showMessage("SS Disconnected!", 2000);
        return true;
    }

    pendingBool = false;
    QMetaObject::invokeMethod(this, "doSendString",
                              Qt::QueuedConnection,
                              Q_ARG(QString, message));
    sendWait.wait(&sendMutex);
    return pendingBool;
}

/*! \brief Comms::doSendString
 * Worker slot — always runs on the Comms thread.
 * Performs the actual serial/TCP write, then wakes the calling thread.
 */
void Comms::doSendString(QString message)
{
    if(!portAlive.loadRelaxed())
    {
        QMutexLocker lock(&sendMutex);
        pendingBool = false;
        sendWait.wakeAll();
        return;
    }
    QMutexLocker lock(&sendMutex);

    if(client.isOpen()) keepAliveTimer->setInterval(600000);

    if(serial->isOpen())
    {
        if(message.length() > 100)
        {
            for(int i = 0; i < message.length(); i++)
            {
                QString m = message.at(i);
                serial->write(m.toLocal8Bit());
                if(((i + 1) % 100) == 0)
                {
                    serial->waitForBytesWritten();
                    msDelay(10);
                }
            }
        }
        else
        {
            serial->write(message.toLocal8Bit());
        }
    }

    if(client.isOpen()) client.write(message.toLocal8Bit());

    pendingBool = true;
    sendWait.wakeAll();
}

// -----------------------------------------------------------------------------
// SendCommand
// -----------------------------------------------------------------------------

/*! \brief Comms::SendCommand (named overload) */
bool Comms::SendCommand(QString name, QString message)
{
    if((name == MIPSname) || (name == "")) return SendCommand(message);
    return false;
}

/*! \brief Comms::SendCommand
 * Sends a command to MIPS and waits for an ACK. Thread-safe.
 * Returns true on ACK or timeout, false on NAK.
 */
bool Comms::SendCommand(QString message)
{
    if(QThread::currentThread() == qApp->thread())
    {
        if(!portAlive.loadRelaxed()) return true;
        qWarning() << "WARNING: SendCommand called from UI thread with live port:"
                   << message << "- this may block the UI thread";
    }
    QMutexLocker lock(&sendMutex);

    if(!serial->isOpen() && !client.isOpen())
    {
        if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + " SC Disconnected!", 2000);
        else                    sb->showMessage("SC Disconnected!", 2000);
        return true;
    }

    pendingBool = false;
    QMetaObject::invokeMethod(this, "doSendCommand",
                              Qt::QueuedConnection,
                              Q_ARG(QString, message));
    sendWait.wait(&sendMutex);
    return pendingBool;
}

/*! \brief Comms::doSendCommand
 * Worker slot — always runs on the Comms thread.
 * Implements the send / ACK-wait / retry loop, then wakes the calling thread.
 */
void Comms::doSendCommand(QString message)
{
    QString res;
    bool hasacknak = false;

    if(!portAlive.loadRelaxed())
    {
        QMutexLocker lock(&sendMutex);
        pendingBool = true;    // return safe default
        sendWait.wakeAll();
        return;
    }

    QMutexLocker lock(&sendMutex);
    if(client.isOpen()) keepAliveTimer->setInterval(600000);

    for(int i = 0; i < 2; i++)
    {
        processAsyncMessages();
        rb.clear();

        if(serial->isOpen())
        {
            if(message.length() > 100)
            {
                for(int j = 0; j < message.length(); j++)
                {
                    QString m = message.at(j);
                    serial->write(m.toLocal8Bit());
                    if(((j + 1) % 100) == 0)
                    {
                        serial->waitForBytesWritten();
                        msDelay(10);
                    }
                }
            }
            else
            {
                serial->write(message.toLocal8Bit());
            }
        }
        if(client.isOpen()) client.write(message.toLocal8Bit());

        waitforline(message.length() > 100 ? 3000 : 1000);

        while(rb.numLines() >= 1)
        {
            res = rb.getline(&hasacknak);
            if((mrb != nullptr) && (hasacknak == false))
            {
                mrb->putString(res + "\n");
                waitforline(100);
                continue;
            }
            if(res == "")
            {
                pendingBool = true;
                sendWait.wakeAll();
                return;
            }
            if(res == "?")
            {
                res = message + " :NAK";
                if(!MIPSname.isEmpty()) emit statusMessage(MIPSname + ", " + res, 2000);
                else                    emit statusMessage(res, 2000);
                pendingBool = false;
                sendWait.wakeAll();
                return;
            }
            break;
        }
    }

    // Timeout
    res = message + " :Timeout";
    if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + ", " + res, 2000);
    else                    sb->showMessage(res, 2000);
    pendingBool = true;   // original returned true on timeout
    sendWait.wakeAll();
}

// -----------------------------------------------------------------------------
// SendMessage / SendMess
// -----------------------------------------------------------------------------

/*! \brief Comms::SendMessage (named overload) */
QString Comms::SendMessage(QString name, QString message)
{
    if((name == MIPSname) || (name == "")) return SendMessage(message);
    return "";
}

/*! \brief Comms::SendMess (named overload) */
QString Comms::SendMess(QString name, QString message)
{
    if((name == MIPSname) || (name == "")) return SendMessage(message);
    return "";
}

/*! \brief Comms::SendMess */
QString Comms::SendMess(QString message)
{
    return SendMessage(message);
}

/*! \brief Comms::SendMessage
 * Sends a message to MIPS and returns the response string. Thread-safe.
 * Returns "" on timeout or disconnection.
 */
QString Comms::SendMessage(QString message)
{
    if(QThread::currentThread() == qApp->thread())
    {
        if(!portAlive.loadRelaxed()) return "";
        qWarning() << "WARNING: SendMessage called from UI thread with live port:"
                   << message << "- this may block the UI thread";
    }
    QMutexLocker lock(&sendMutex);

    if(!serial->isOpen() && !client.isOpen())
    {
        if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + " SM Disconnected!", 2000);
        else                    sb->showMessage("SM Disconnected!", 2000);
        return "";
    }

    pendingResponse = "";
    QMetaObject::invokeMethod(this, "doSendMessage",
                              Qt::QueuedConnection,
                              Q_ARG(QString, message));
    sendWait.wait(&sendMutex);
    return pendingResponse;
}

/*! \brief Comms::doSendMessage
 * Worker slot — always runs on the Comms thread.
 * Implements the send / response-wait / retry loop, then wakes the caller.
 */
void Comms::doSendMessage(QString message)
{
    QString res;
    bool hasacknak = false;

    if(!portAlive.loadRelaxed())
    {
        QMutexLocker lock(&sendMutex);
        pendingResponse = "";
        sendWait.wakeAll();
        return;
    }
    QMutexLocker lock(&sendMutex);

    if(client.isOpen()) keepAliveTimer->setInterval(600000);

    for(int i = 0; i < 2; i++)
    {
        processAsyncMessages();
        rb.clear();

        if(serial->isOpen())
        {
            if(message.length() > 100)
            {
                for(int j = 0; j < message.length(); j++)
                {
                    QString m = message.at(j);
                    serial->write(m.toLocal8Bit());
                    if(((j + 1) % 100) == 0)
                    {
                        serial->waitForBytesWritten();
                        msDelay(10);
                    }
                }
            }
            else
            {
                serial->write(message.toLocal8Bit());
            }
        }
        if(client.isOpen()) client.write(message.toLocal8Bit());

        waitforline(message.length() > 100 ? 3000 : 1000);

        while(rb.numLines() >= 1)
        {
            res = rb.getline(&hasacknak);
            if((mrb != nullptr) && (hasacknak == false))
            {
                mrb->putString(res + "\n");
                waitforline(100);
                continue;
            }
            if(res != "")
            {
                pendingResponse = res;
                sendWait.wakeAll();
                return;
            }
        }
    }

    // Timeout
    res = message + " :Timeout";
    if(!MIPSname.isEmpty()) emit statusMessage(MIPSname + ", " + res, 2000);
    else                    emit statusMessage(res, 2000);
    pendingResponse = "";
    sendWait.wakeAll();
}

// =============================================================================
// waitforline
// =============================================================================

/*! \brief Comms::waitforline
 * Waits until at least one complete line is present in the ring buffer, or
 * the timeout expires.
 *
 *  timeout > 0  — wait up to timeout ms, then return regardless.
 *  timeout == 0 — wait indefinitely (returns only when a line arrives or the
 *                 port closes).
 *  timeout == -1 — non-blocking single read attempt.
 *
 * FIX: replaced QApplication::processEvents() spin loops with
 * waitForReadyRead() calls. This blocks only the Comms thread, never pumps
 * the Qt event loop, and therefore cannot cause re-entrant signal delivery.
 */
void Comms::waitforline(int timeout)
{
    if(timeout == -1)
    {
        readData2RingBuffer();
        return;
    }

    if(timeout == 0)
    {
        // FIX: was an unconditional infinite spin with no escape on disconnect.
        // Now exits if the port closes.
        while(true)
        {
            readData2RingBuffer();
            if(rb.numLines() > 0) return;
            if(!serial->isOpen() && !client.isOpen()) return;
            if(serial->isOpen()) serial->waitForReadyRead(100);
            if(client.isOpen()) client.waitForReadyRead(100);
        }
    }

    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() < timeout)
    {
        readData2RingBuffer();
        if(rb.numLines() > 0) return;
        if(!serial->isOpen() && !client.isOpen()) return;
        // Wait in 50 ms slices so we check elapsed time regularly.
        qint64 remaining = timeout - timer.elapsed();
        if(remaining <= 0) return;
        int slice = (int)qMin(remaining, (qint64)50);
        if(serial->isOpen()) serial->waitForReadyRead(slice);
        if(client.isOpen()) client.waitForReadyRead(slice);
    }
}

// =============================================================================
// Ring buffer I/O
// =============================================================================

/*! \brief Comms::readData2RingBuffer
 * Slot: reads all available bytes from the open connection into the ring
 * buffer, emits lineAvailable if a complete line is present, and emits
 * DataReady.
 *
 * FIX: removed waitForReadyRead(0) calls — they are unnecessary because this
 * slot is called either directly from waitforline (which already waited) or
 * from a readyRead signal (which guarantees bytes are available).
 */
void Comms::readData2RingBuffer(void)
{
    if(client.isOpen())
    {
        QByteArray data = client.readAll();
        for(int i = 0; i < data.size(); i++) rb.putch(data[i]);
    }
    if(serial->isOpen())
    {
        QByteArray data = serial->readAll();
        for(int i = 0; i < data.size(); i++) rb.putch(data[i]);
    }
    if(rb.numLines() > 0) emit lineAvailable();
    emit DataReady();
}

/*! \brief Comms::readAvailableData2RingBuffer
 * Reads all currently available bytes from the open connection into the ring
 * buffer and emits DataReady. Does not block.
 */
void Comms::readAvailableData2RingBuffer(void)
{
    if(client.isOpen() && client.bytesAvailable() > 0)
    {
        QByteArray data = client.readAll();
        for(int i = 0; i < data.size(); i++) rb.putch(data[i]);
        emit DataReady();
    }
    if(serial->isOpen() && serial->bytesAvailable() > 0)
    {
        QByteArray data = serial->readAll();
        for(int i = 0; i < data.size(); i++) rb.putch(data[i]);
        emit DataReady();
    }
}

/*! \brief Comms::getline */
QString Comms::getline(void) { return rb.getline(); }

/*! \brief Comms::readall
 * Drains and returns all bytes currently in the ring buffer.
 */
QByteArray Comms::readall(void)
{
    QByteArray data;
    char c;
    while(true)
    {
        c = rb.getch();
        if(c == 0) return data;
        data += c;
    }
}

/*! \brief Comms::writeData
 * Writes a byte array to the open TCP socket or serial port in 100-byte chunks.
 */
void Comms::writeData(const QByteArray &data)
{
    if(client.isOpen())
    {
        for(int i = 0; i < data.length(); i++)
        {
            client.putChar(data.at(i));
            if(((i + 1) % 100) == 0)
            {
                client.waitForBytesWritten();
                msDelay(10);
            }
        }
    }
    if(serial->isOpen())
    {
        for(int i = 0; i < data.length(); i++)
        {
            serial->putChar(data.at(i));
            if(((i + 1) % 100) == 0)
            {
                serial->waitForBytesWritten();
                msDelay(10);
            }
        }
    }
}

// =============================================================================
// Connection management
// =============================================================================

/*! \brief Comms::GetMIPSnameAndVersion
 * Queries MIPS for its name and firmware version. Sets time/date if firmware
 * supports it. Sends any commands found in the app-directory .ini file.
 */
void Comms::GetMIPSnameAndVersion(void)
{
    MIPSname = SendMessage("GNAME\n");
    MIPSname = SendMessage("GNAME\n");

    QStringList reslist = SendMessage("GVER\n").split(" ");
    major = minor = 1;
    if(reslist.count() > 2)
    {
        QStringList verlist = reslist[1].split(".");
        if(verlist.count() == 2)
        {
            major = verlist[0].toInt();
            bool ok;
            for(int i = 5; i > 0; i--)
            {
                minor = QStringView{verlist[1]}.left(i).toInt(&ok);
                if(ok) break;
            }
        }
    }
    if((major == 1) && (minor >= 201))
    {
        QString str = QDateTime::currentDateTime().time().toString();
        SendCommand("STIME," + str + "\n");
        str = QDateTime::currentDateTime().date().toString("dd/MM/yyyy");
        SendCommand("SDATE," + str + "\n");
    }

#ifdef Q_OS_MAC
    QString ext = ".app";
#else
    QString ext = ".exe";
#endif
    int i = QApplication::applicationFilePath().indexOf(
                QApplication::applicationName() + ext);
    if(i == -1) return;
    QString FileName = QApplication::applicationFilePath().left(i)
                     + QApplication::applicationName() + ".ini";
    QFile file(FileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            if(line.trimmed().startsWith("#")) continue;
            if(line.trimmed() == "")           continue;
            SendCommand(line + "\n");
        }
        while(!line.isNull());
        file.close();
        rb.clear();
    }
}

/*! \brief Comms::ConnectToMIPS
 * Opens a serial or TCP connection to MIPS and reads the device name and
 * version. Returns true if the connection was established successfully.
 *
 * NOTE: when running on a worker thread, QApplication::processEvents() is
 * used only in the TCP connection wait loop here. This is acceptable because
 * ConnectToMIPS() is called once at startup, before any other transactions
 * are in flight, so re-entrancy into Send* is not possible. If this is ever
 * called from a context where other transactions may be queued, replace the
 * processEvents() loop with a QEventLoop + QTimer::singleShot timeout.
 */
bool Comms::ConnectToMIPS()
{
    QElapsedTimer timer;

    MIPSname = "";
    if(client.isOpen() || serial->isOpen()) return true;

    if(host != "")
    {
        client_connected = false;
        client.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
        client.connectToHost(host, 2015);
        sb->showMessage(tr("Connecting..."));
        timer.start();
        while(timer.elapsed() < 30000)
        {
            QApplication::processEvents();
            if(client_connected)
            {
                keepAliveTimer->start(600000);
                GetMIPSnameAndVersion();
                return true;
            }
        }
        sb->showMessage(tr("MIPS failed to connect!"));
        client.abort();
        client.close();
        return false;
    }
    else
    {
        openSerialPort();
        serial->setDataTerminalReady(true);
        GetMIPSnameAndVersion();
    }
    return true;
}

/*! \brief Comms::DisconnectFromMIPS */
void Comms::DisconnectFromMIPS()
{
    portAlive.storeRelaxed(0);
    if(client.isOpen())
    {
        client.close();
        keepAliveTimer->stop();
        reconnectTimer->stop();
    }
    closeSerialPort();
}

/*! \brief Comms::isConnected */
bool Comms::isConnected(void)
{
    return portAlive.loadRelaxed() == 1;
}

// =============================================================================
// Serial port open / close / error
// =============================================================================

/*! \brief Comms::openSerialPort
 * Configures and opens the serial port using the stored settings.
 * Returns true on success; shows an error dialog on failure.
 *
 * FIX: errorOccurred is connected with Qt::UniqueConnection to prevent
 * duplicate handler registrations when openSerialPort() is called more than
 * once (e.g. after a reconnect).
 */
bool Comms::openSerialPort()
{
    // FIX: Qt::UniqueConnection prevents duplicate signal connections on
    // repeated open calls (e.g. after a reconnect via slotReconnect).
    connect(serial, &QSerialPort::errorOccurred, this, &Comms::handleError,
            Qt::UniqueConnection);

    serial->setPortName(p.name);
#if defined(Q_OS_MAC)
    serial->setPortName("cu." + p.name);
#endif
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);

    if(serial->open(QIODevice::ReadWrite))
    {
        sb->showMessage(
            QString("Connected to %1 : %2, %3, %4, %5, %6")
                .arg(p.name, p.stringBaudRate, p.stringDataBits,
                     p.stringParity, p.stringStopBits, p.stringFlowControl));
        portAlive.storeRelaxed(1);
        return true;
    }

    QMessageBox::critical(nullptr, QString("Error"), serial->errorString());
    sb->showMessage(tr("Open error: ") + serial->errorString());
    return false;
}

/*! \brief Comms::closeSerialPort
 * Closes the serial port and disconnects the error-occurred signal handler.
 */
void Comms::closeSerialPort()
{
    portAlive.storeRelaxed(0);
    if(serial->isOpen()) serial->close();
    if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + " Closed!", 2000);
    else                    sb->showMessage("Closed!", 2000);
    disconnect(serial, &QSerialPort::errorOccurred, nullptr, nullptr);
}

/*! \brief Comms::handleError
 * Slot: handles serial port errors. On ResourceError (USB disconnect), closes
 * the port and starts the reconnect timer if AutoRestore is enabled.
 *
 * FIX: removed QThread::sleep(1) — sleeping inside an error signal handler
 * blocks Qt's serial port teardown and causes a crash on Windows when the
 * USB device is removed. The port is already dead by the time ResourceError
 * fires; no delay is needed before closing it.
 */
void Comms::handleError(QSerialPort::SerialPortError error)
{
    if(error == QSerialPort::ResourceError)
    {
        portAlive.storeRelaxed(0);   // atomic — visible to all threads immediately
        // FIX: no QThread::sleep() here — was causing the USB-disconnect crash.
        closeSerialPort();
        if(!MIPSname.isEmpty())
            emit statusMessage(MIPSname + tr(" Critical Error, port closing: ") + serial->errorString());
        else
            emit statusMessage(tr("Critical Error, port closing: ") + serial->errorString());

        if(properties != nullptr && properties->AutoRestore)
        {
            reconnectTimer->setInterval(2000);
            reconnectTimer->start();
        }
    }
}

/*! \brief Comms::reopenSerialPort */
void Comms::reopenSerialPort(void)
{
    if(!serial->isOpen()) return;
    serial->close();
    msDelay(250);
    serial->open(QIODevice::ReadWrite);
    serial->setDataTerminalReady(true);
}

/*! \brief Comms::reopenPort
 * Attempts to re-establish the connection (TCP or serial) after a drop.
 */
void Comms::reopenPort(void)
{
    QElapsedTimer timer;

    if(host != "")
    {
        client_connected = false;
        client.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
        client.connectToHost(host, 2015);
        sb->showMessage(tr("Connecting..."));
        timer.start();
        while(timer.elapsed() < 5000)
        {
            QThread::msleep(100);
            if(client_connected) return;
        }
        sb->showMessage(tr("MIPS failed to connect!"));
        client.abort();
        client.close();
    }
    else
    {
        serial->close();
        msDelay(250);
        serial->open(QIODevice::ReadWrite);
        serial->setDataTerminalReady(true);
    }
}

// =============================================================================
// Slots
// =============================================================================

/*! \brief Comms::connected — TCP connected */
void Comms::connected(void)
{
    portAlive.storeRelaxed(1);
    if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + tr(" MIPS connected"));
    else                    sb->showMessage(tr("MIPS connected"));
    client_connected = true;
}

/*! \brief Comms::disconnected — TCP disconnected */
void Comms::disconnected(void)
{
    portAlive.storeRelaxed(0);
    if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + " Disconnect signaled!", 2000);
    else                    sb->showMessage("Disconnect signaled!!", 2000);
}

/*! \brief Comms::slotAboutToClose */
void Comms::slotAboutToClose(void) {}

/*! \brief Comms::slotKeepAlive — sends a newline to keep the TCP link alive */
void Comms::slotKeepAlive(void)
{
    SendString("\n");
}

/*! \brief Comms::slotReconnect
 * Slot: attempts to reopen the serial port after a drop, stopping the
 * reconnect timer on success.
 *
 * FIX: connect() uses Qt::UniqueConnection — same guard as openSerialPort().
 */
void Comms::slotReconnect(void)
{
    if(!serial->isOpen())
    {
        serial->open(QIODevice::ReadWrite);
        serial->setDataTerminalReady(true);
        // FIX: Qt::UniqueConnection prevents stacking duplicate error handlers
        connect(serial, &QSerialPort::errorOccurred, this, &Comms::handleError,
                Qt::UniqueConnection);
    }
    if(serial->isOpen())
    {
        portAlive.storeRelaxed(1);
        reconnectTimer->stop();
        if(!MIPSname.isEmpty()) emit statusMessage(MIPSname + tr(" Serial port reconnected!"));
        else                    emit statusMessage(tr("Serial port reconnected!"));
    }
}

// =============================================================================
// Device probing — isMIPS / isAMPS
// =============================================================================

/*! \brief Comms::isMIPS
 * Opens the given serial port at 115200 baud and checks whether the device
 * responds as a MIPS unit. Returns true if so.
 */
bool Comms::isMIPS(QString port)
{
    QString res;

    if(serial->isOpen()) return false;
    disconnect(serial, &QSerialPort::errorOccurred, nullptr, nullptr);

    p.name = port;
    serial->setPortName(p.name);
#if defined(Q_OS_MAC)
    serial->setPortName("cu." + p.name);
#endif
    if(serial->isOpen()) return false;

    p.baudRate   = QSerialPort::Baud115200;
    p.dataBits   = QSerialPort::Data8;
    p.parity     = QSerialPort::NoParity;
    p.stopBits   = QSerialPort::OneStop;
    p.flowControl = QSerialPort::NoFlowControl;
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);

    if(serial->open(QIODevice::ReadWrite))
    {
        serial->setDataTerminalReady(true);
        serial->waitForReadyRead(100);
        res = SendMessage("GVER\n");
        SendString("ECHO,FALSE\n");
        serial->waitForBytesWritten(200);
        serial->close();
        serial->clearError();
        rb.clear();
        if(res.contains("Version") || res.contains("version")) return true;
    }
    serial->clearError();
    rb.clear();
    return false;
}

/*! \brief Comms::isAMPS
 * Opens the given serial port at the specified baud rate and checks whether
 * the connected device responds as an AMPS unit. Returns true if so.
 */
bool Comms::isAMPS(QString port, QString baud)
{
    QString res;

    if(serial->isOpen()) return false;
    disconnect(serial, &QSerialPort::errorOccurred, nullptr, nullptr);

    p.name = port;
    serial->setPortName(p.name);
#if defined(Q_OS_MAC)
    serial->setPortName("cu." + p.name);
#endif
    if(serial->isOpen()) return false;

    p.baudRate    = baud.toInt();
    p.dataBits    = QSerialPort::Data8;
    p.parity      = QSerialPort::EvenParity;
    p.stopBits    = QSerialPort::TwoStop;
    p.flowControl = QSerialPort::SoftwareControl;
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);

    if(serial->open(QIODevice::ReadWrite))
    {
        serial->setDataTerminalReady(true);
        serial->waitForReadyRead(100);
        SendMessage("\n");    // flush
        rb.clear();
        res = SendMessage("GVER\n");
        SendString("RTM,ON\n");
        serial->waitForBytesWritten(200);
        serial->close();
        serial->clearError();
        rb.clear();
        if(res.startsWith("V")) return true;
    }
    serial->clearError();
    rb.clear();
    return false;
}
