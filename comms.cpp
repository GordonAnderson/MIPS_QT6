// =============================================================================
// comms.cpp
//
// Comms — serial-port and TCP-socket communication layer for MIPS/AMPS devices.
// Handles connection, command send/receive (ACK/NAK + timeout retry), ADC
// streaming, async message buffering, and binary file transfer.
//
// Depends on:  comms.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 1+2 refactoring
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include <QElapsedTimer>
#include <QtSerialPort/QtSerialPort>
#include <QStringView>
#include <qeventloop.h>
#include "comms.h"

/*! \brief Comms::Comms
 * Initialises the serial port or TCP socket for communication with MIPS.
 * Connects all signals required for incoming data and connection-state events.
 */
Comms::Comms(SettingsDialog *settings, QString Host, QStatusBar *statusbar)
{
    p = settings->settings();
    sb = statusbar;
    client_connected = false;
    host = Host;
    properties = nullptr;
    serial = new QSerialPort(this);
    keepAliveTimer = new QTimer;
    reconnectTimer = new QTimer;
    connect(&client, &QTcpSocket::readyRead, this, &Comms::readData2RingBuffer);
    connect(serial, &QSerialPort::readyRead, this, &Comms::readData2RingBuffer);
    connect(&client, &QTcpSocket::connected, this, &Comms::connected);
    connect(&client, &QTcpSocket::disconnected, this, &Comms::disconnected);
    connect(&client, &QIODevice::aboutToClose, this, &Comms::slotAboutToClose);
    connect(keepAliveTimer, &QTimer::timeout, this, &Comms::slotKeepAlive);
    connect(reconnectTimer, &QTimer::timeout, this, &Comms::slotReconnect);
    connect(&pollTimer, &QTimer::timeout, this, &Comms::pollLoop);
}

/*! \brief Comms::pollLoop
 * Timer-driven poll: manually triggers readyRead on the serial port if bytes are waiting.
 */
void Comms::pollLoop(void)
{
   if(serial->isOpen())
   {
       if(serial->bytesAvailable() > 0) emit serial->readyRead();
   }
}

/*! \brief Comms::getchar
 * Returns the next byte from the ring buffer, or 0 if empty.
 */
char Comms::getchar(void)
{
    return rb.getch();
}

/*! \brief Comms::CalculateCRC
 * Computes an 8-bit CRC of the given byte array using the 0x1D generator polynomial.
 */
int Comms::CalculateCRC(QByteArray fdata)
{
    unsigned char generator = 0x1D;
    unsigned char crc = 0;

    for(int i=0; i<fdata.length(); i++)
    {
      crc ^= fdata[i];
      for(int j=0; j<8; j++)
      {
        if((crc & 0x80) != 0)
        {
          crc = ((crc << 1) ^ generator);
        }
        else
        {
          crc <<= 1;
        }
      }
    }
    return crc;
}

// Reads an ADC vector from MIPS. This function will setup for data from the
// serial port and then fill the buffer passed by reference if valid. This
// function does not block. After a vector is received a signal is sent and
// then the system is reset for the next vector. After all vectors are read
// the comms are reset to normal command processing
/*! \brief Comms::GetADCbuffer
 * Arms ADC streaming mode. Redirects incoming data to the ADC state machine
 * until all vectors for the given buffer have been received.
 */
void Comms::GetADCbuffer(quint16 *ADCbuffer, int NumSamples)
{
    if(ADCbuffer == nullptr) return;
    // Save the pointer and maximum length parameters
    ADCbuf = ADCbuffer;
    ADClen = NumSamples;
    ADCstate = WaitingForHeader;
    // Redirect the input data processing to ADC buffer processing
    disconnect(&client, &QTcpSocket::readyRead, nullptr, nullptr);
    disconnect(serial, &QSerialPort::readyRead, nullptr, nullptr);
    connect(&client, &QTcpSocket::readyRead, this, &Comms::readData2ADCBuffer);
    connect(serial, &QSerialPort::readyRead, this, &Comms::readData2ADCBuffer);
}

// Release the ADC buffer collection mode and return the processing routine
// for incoming data to the ringbuffers
/*! \brief Comms::ADCrelease
 * Releases ADC streaming mode and restores normal ring-buffer data processing.
 */
void Comms::ADCrelease(void)
{
    // Connect the data read signal back to the ring buffers
    disconnect(&client, &QTcpSocket::readyRead, nullptr, nullptr);
    disconnect(serial, &QSerialPort::readyRead, nullptr, nullptr);
    connect(&client, &QTcpSocket::readyRead, this, &Comms::readData2RingBuffer);
    connect(serial, &QSerialPort::readyRead, this, &Comms::readData2RingBuffer);
    ADCbuf = nullptr;
}

/*! \brief Comms::readData2ADCBuffer
 * Slot: reads incoming bytes and feeds them through the ADC binary framing
 * state machine. Emits ADCvectorReady per vector and ADCrecordingDone when finished.
 */
void Comms::readData2ADCBuffer(void)
{
    int i;
    QByteArray data;
    static quint8 last = 0, *b;
    static int DataPtr;
    static int Vlength=0,Vnum=0;
    static bool Vlast;

    if(ADCbuf == nullptr) return;
    if(client.isOpen()) data = client.readAll();
    if(serial->isOpen()) data = serial->readAll();
    for(i=0;i<data.size();i++)
    {
        switch(ADCstate)
        {
           case WaitingForHeader:
            // look for 0x55, 0xAA. This flags start of data
            if((last == 0x55) && ((quint8)data[i] == 0xAA))
            {
                ADCstate = ReadingHeader;
                DataPtr =0;
                b = (quint8 *)&Vlength;
                continue;
            }
            break;
          case ReadingHeader:
            // Read, 24 bit length, 16 bit vector num, 8 bit last vector flag 0xFF if last
            if(b == nullptr) break;
            if(DataPtr == 0) b[0] = data[i];
            if(DataPtr == 1) b[1] = data[i];
            if(DataPtr == 2) { b[2] = data[i]; b = (quint8 *)&Vnum; }
            if(DataPtr == 3) b[0] = data[i];
            if(DataPtr == 4) b[1] = data[i];
            if(DataPtr == 5)
            {
                if((unsigned char)data[i] == 0xFF) Vlast =  true;
                else Vlast = false;
            }
            DataPtr++;
            if(DataPtr > 5)
            {
                DataPtr = 0;
                ADCstate = ReadingData;
                b = (quint8 *)ADCbuf;
                continue;
            }
            break;
          case ReadingData:
            // Read the data block
            if(b == nullptr) break;
            b[DataPtr++] = data[i];
            if(DataPtr >= (Vlength * 2))
            {
                ADCstate = ReadingTrailer;
            }
            break;
          case ReadingTrailer:
            // look for 0xAE, 0xEA. This flags end of message
            if((last == 0xAE) && ((quint8)data[i] == 0xEA))
            {
                emit ADCvectorReady();
                if(Vlast)
                {
                    ADCstate = ADCdone;
                }
                else ADCstate = WaitingForHeader;
                if(ADCstate == WaitingForHeader) continue;
            }
            if(ADCstate != ADCdone) break;
          case ADCdone:
            // ADC done so send signal
            emit ADCrecordingDone();
            return;
          default:
            last = data[i];
            break;
        }
        last = data[i];
    }
}


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

    // Send the command to MIPS to get the file
    if(SendCommand("GET," + MIPSfile + "\n"))
    {
       // Now process the data in the ring buffer
       // First read the length
        waitforline(500);
        QString FileSize = getline();
        // Read the hex data stream
        for(int i=0; i<FileSize.toInt(); i+=1024)
        {
            len = 1024;
            if((FileSize.toInt() - i) < 1024) len = FileSize.toInt() - i;
            // Read the block of data
            for(int j=0; j<len*2; j++)
            {
              while((c = getchar()) == 0) QApplication::processEvents();
              if(c == '\n') break;
              FileData += QChar(c);
            }
            // Send "Next" block request
            if((FileSize.toInt()-i) > 1024) SendString("Next\n");
        }
        while(getchar() == 0) QApplication::processEvents();
        if(FileData.length() != 2 * FileSize.toInt())
        {
            // Here if file data block is not the proper size
            QMessageBox::critical(this, tr("File Error"), "Data block size not correct!");
            return;
        }
        // Read the CRC
        waitforline(500);
        QString FileCRC = getline();
        // Convert data to byte array and calculate CRC
        QByteArray fdata;
        fdata.resize(FileSize.toInt());
        for(int i=0; i<FileSize.toInt(); i++)
        {
            fdata[i] = QStringView{FileData}.mid(i*2,2).toUInt(&ok,16);
        }
        if(CalculateCRC(fdata) == FileCRC.toInt())
        {
            // Now save the data to the user selected file
            QFile file(LocalFile);
            file.open(QIODevice::WriteOnly);
            file.write(fdata);
            file.close();
            QMessageBox::information(this, tr("File saved"), "File from MIPS read and saved successfully!");
        }
        else
        {
            // Here with file CRC error, raise error message
            QMessageBox::critical(this, tr("File Error"), "CRC error!");
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
    int fsize,len;
    QString dblock,res;

    // open the local file to send and read the data
    QFile file(LocalFile);
    file.open(QIODevice::ReadOnly);
    fdata = file.readAll();
    // Convert the data into a ascii hex string
    for(int i=0; i<file.size(); i++)
    {
         dblock += QString().asprintf("%02x",(unsigned char)fdata[i]);
    }
    fsize = file.size();
    file.close();
    // Send the data to MIPS
    if(SendCommand("PUT," + MIPSfile + "," + QString().number(fsize) + "\n"))
    {
        for(int i=0; i<dblock.length(); i+=1024)
        {
            len = 1024;
            if((dblock.length() - i) < 1024) len = dblock.length() - i;
            // Send this in chunks in case the sender is a lot faster than MIPS
            for(int k=0;k<len;k+=128)
            {
                if(len > (k + 128)) SendString(dblock.mid(i+k,128));
                else SendString(dblock.mid(i+k,len - k));
                if(serial->isOpen()) serial->waitForBytesWritten();
                if(client.isOpen()) client.waitForBytesWritten();
                msDelay(5);
            }
            //SendString(dblock.mid(i,len));
            if((len == 1024) && ((dblock.length() - i) != 1024))
            {
               waitforline(1000);
               if((res = getline()) == "")
                {
                    // Here if we timed out, display error and exit
                    QMessageBox::critical(this, tr("Data read error"), "Timedout waiting for data from MIPS!");
                    return;
                }
            }
            QApplication::processEvents();
        }
        SendString("\n");
        SendString(QString().number(CalculateCRC(fdata)) + "\n");
        QMessageBox::information(this, tr("File saved"), "File sent to MIPS!");
    }
}

/*! \brief Comms::GetEEPROM
 * Reads the EEPROM of the named board module and saves the data locally.
 */
void Comms::GetEEPROM(QString FileName, QString Board, int Addr)
{
    bool ok = false;
    QString FileData;

    // Send the command to MIPS to get the file
    if(SendCommand("GETEEPROM," + Board + "," + QString().asprintf(",%x\n",Addr)))
    {
       // Now process the data in the ring buffer
       // First read the length
        waitforline(500);
        QString FileSize = getline();
        // Read the hex data stream
        waitforline(1000);
        FileData = getline();
        if(FileData.length() != 2 * FileSize.toInt())
        {
            // Here if file data block is not the proper size
            QMessageBox::critical(this, tr("File Error"), "Data block size not correct!");
            return;
        }
        // Read the CRC
        waitforline(500);
        QString FileCRC = getline();
        // Convert data to byte array and calculate CRC
        QByteArray fdata;
        fdata.resize(FileSize.toInt());
        for(int i=0; i<FileSize.toInt(); i++)
        {
            fdata[i] = QStringView{FileData}.mid(i*2,2).toUInt(&ok,16);
        }
        if(CalculateCRC(fdata) == FileCRC.toInt())
        {
            // Now save the data to the user selected file
            QFile file(FileName);
            file.open(QIODevice::WriteOnly);
            file.write(fdata);
            file.close();
            QMessageBox::information(this, tr("File saved"), "EEPROM from MIPS read and saved successfully!");
        }
        else
        {
            // Here with file CRC error, raise error message
            QMessageBox::critical(this, tr("File Error"), "CRC error!");
        }
    }
}

/*! \brief Comms::GetARBFLASH
 * Reads the ARB module FLASH memory and saves it to a local file.
 */
void Comms::GetARBFLASH(QString FileName)
{
    bool ok = false;
    QString FileData;

    if(SendCommand("GETFLASH\n"))
    {
       // Now process the data in the ring buffer
       // First read the length
        waitforline(500);
        QString FileSize = getline();
        // Read the hex data stream
        waitforline(1000);
        FileData = getline();
        if(FileData.length() != 2 * FileSize.toInt())
        {
            // Here if file data block is not the proper size
            QMessageBox::critical(this, tr("File Error"), "Data block size not correct!");
            return;
        }
        // Read the CRC
        waitforline(500);
        QString FileCRC = getline();
        // Convert data to byte array and calculate CRC
        QByteArray fdata;
        fdata.resize(FileSize.toInt());
        for(int i=0; i<FileSize.toInt(); i++)
        {
            fdata[i] = QStringView{FileData}.mid(i*2,2).toUInt(&ok,16);
        }
        if(CalculateCRC(fdata) == FileCRC.toInt())
        {
            // Now save the data to the user selected file
            QFile file(FileName);
            file.open(QIODevice::WriteOnly);
            file.write(fdata);
            file.close();
            QMessageBox::information(this, tr("File saved"), "FLASH from ARB read and saved successfully!");
        }
        else
        {
            // Here with file CRC error, raise error message
            QMessageBox::critical(this, tr("File Error"), "CRC error!");
        }
    }
}

/*! \brief Comms::PutARBFLASH
 * Writes a local file to the ARB module FLASH memory.
 */
void Comms::PutARBFLASH(QString FileName)
{
    //QString FileData;
    QString dblock;
    QByteArray fdata;

    if(SendCommand("PUTFLASH\n"))
    {
        // open the local file to send and read the data
        QFile file(FileName);
        file.open(QIODevice::ReadOnly);
        fdata = file.readAll();
        // Convert the data into a ascii hex string
        for(int i=0; i<file.size(); i++)
        {
             dblock += QString().asprintf("%02x",(unsigned char)fdata[i]);
        }
        SendString(QString().number(file.size()) + "\n");
        file.close();
        // Send the data to ARB
        SendString(dblock + "\n");
        SendString(QString().number(CalculateCRC(fdata)) + "\n");
        QMessageBox::information(this, tr("FLASH write"), "ARB module FLASH Written!");
    }
}

/*! \brief Comms::PutEEPROM
 * Writes a local file to the EEPROM of the named board module.
 */
void Comms::PutEEPROM(QString FileName, QString Board, int Addr)
{
    //QString FileData;
    QString dblock;
    QByteArray fdata;

    // Send the command to MIPS to get the file
    if(SendCommand("PUTEEPROM," + Board + QString().asprintf(",%x\n",Addr)))
    {
        // open the local file to send and read the data
        QFile file(FileName);
        file.open(QIODevice::ReadOnly);
        fdata = file.readAll();
        // Convert the data into a ascii hex string
        for(int i=0; i<file.size(); i++)
        {
             dblock += QString().asprintf("%02x",(unsigned char)fdata[i]);
        }
        SendString(QString().number(file.size()) + "\n");
        file.close();
        // Send the data to MIPS
        SendString(dblock + "\n");
        SendString(QString().number(CalculateCRC(fdata)) + "\n");
        QMessageBox::information(this, tr("EEPROM write"), "MIPS module's EEPROM Written!");
    }
}

/*! \brief Comms::ARBupload
 * Uploads a local file to the ARB module at the given flash address in 512-byte chunks.
 */
void Comms::ARBupload(QString Faddress, QString FileName)
{
    QByteArray fdata;
    int fsize,len;
    QString dblock,res;

    // open the local file to send and read the data
    QFile file(FileName);
    file.open(QIODevice::ReadOnly);
    fdata = file.readAll();
    // Convert the data into a ascii hex string
    for(int i=0; i<file.size(); i++)
    {
         dblock += QString().asprintf("%02x",(unsigned char)fdata[i]);
    }
    fsize = file.size();
    file.close();
    // Send the data to ARB
    if(SendCommand("ARBPGM," + Faddress + "," + QString().number(fsize) + "\n" ))
    {
        for(int i=0; i<dblock.length(); i+=512)
        {
            len = 512;
            if((dblock.length() - i) < 512) len = dblock.length() - i;
            // Send this in chunks in case the sender is a lot faster than MIPS
            for(int k=0;k<len;k+=128)
            {
                if(len > (k + 128)) SendString(dblock.mid(i+k,128));
                else SendString(dblock.mid(i+k,len - k));
                if(serial->isOpen()) serial->waitForBytesWritten();
                if(client.isOpen()) client.waitForBytesWritten();
                msDelay(10);
            }
            if(len == 512)
            {
                waitforline(2000);
                if((res = getline()) == "")
                {
                    // Here if we timed out, display error and exit
                    QMessageBox::critical(this, tr("Data read error"), "Timedout waiting for data from ARB!");
                    return;
                }
            }
            QApplication::processEvents();
        }
        SendString("\n");
        SendString(QString().number(CalculateCRC(fdata)) + "\n");
        QMessageBox::information(this, tr("File saved"), "File uploaded to ARB FLASH!");
    }
}

/*! \brief Comms::SendString (named overload)
 * Sends a raw string to MIPS only if the name matches this instance's MIPSname.
 */
bool Comms::SendString(QString name, QString message)
{
    if((name == MIPSname) || (name == "")) return SendString(message);
    return false;
}

/*! \brief Comms::SendString
 * Writes a raw string to the open serial port or TCP socket without
 * waiting for a response. Long strings (>100 chars) are sent in 100-byte chunks.
 */
bool Comms::SendString(QString message)
{
    if(serialBusy)
    {
        if(properties != nullptr) properties->Log("SendString is busy!");
        return false;
    }
    serialBusy = true;
    if (!serial->isOpen() && !client.isOpen())
    {
        if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + " SS Disconnected!",2000);
        else sb->showMessage("SS Disconnected!",2000);
        serialBusy = false;
        return true;
    }
    if(client.isOpen()) keepAliveTimer->setInterval(600000);
    if (serial->isOpen())
    {
        QString m;
        if(message.length() > 100) for(int i=0;i<message.length();i++)
        {
            m = message.at(i);
            serial->write(m.toLocal8Bit());
            if(((i+1) % 100) == 0)
            {
                if(serial->isOpen()) serial->waitForBytesWritten();
                if(client.isOpen()) client.waitForBytesWritten();
                msDelay(10);
            }
        }
        else serial->write(message.toLocal8Bit());
    }
    if (client.isOpen()) client.write(message.toLocal8Bit());
    serialBusy = false;
    return true;
}

/*! \brief enableAsyncMessages enables or disables asynchronous message processing.
 * This function sets up a ring buffer for asynchronous messages if enabled.
 * If disabled, it deletes the existing ring buffer to free up resources.
 * @param enable If true, enables asynchronous message processing; otherwise, disables it.
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

/*! \brief processAsyncMessages processes incoming messages from MIPS asynchronously.
 * This function reads lines from the ring buffer and sends them to MIPS message ringbuffer
 * if it is set up. It will continue to read lines until there are no more lines available.
 * This is useful for processing messages that are sent by MIPS without blocking the main thread.
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

/*! \brief getAsyncMessage retrieves a message from the MIPS message ringbuffer.
 * This function checks if the MIPS message ringbuffer is set up and then retrieves a line
 * from it. If there are no lines available, it returns an empty string.
 * This is useful for retrieving messages that are sent by MIPS asynchronously.
 */
QString Comms::getAsyncMessage(void)
{
    if(mrb == nullptr) return "";
    QString res = mrb->getline();
    if(res == "") return "";
    return res;
}

/*! \brief Comms::SendCommand (named overload)
 * Sends a command to MIPS only if the name matches this instance's MIPSname.
 */
bool Comms::SendCommand(QString name, QString message)
{
    if((name == MIPSname) || (name == "")) return(SendCommand(message));
    return false;
}

/*! \brief SendCommand sends a command to MIPS and waits for a response.
 * This function is used to send commands to MIPS and receive the response.
 * The function will return true if the command was sent successfully or false
 * if there was an error or timeout.
 */
bool Comms::SendCommand(QString message)
{
    QString res;
    bool    hasacknak=false;

    if(serialBusy)
    {
        if(properties != nullptr) properties->Log("SendCommand is busy!");
        return false;
    }
    serialBusy = true;
    if (!serial->isOpen() && !client.isOpen())
    {
        if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + " SC Disconnected!",2000);
        else sb->showMessage("SC Disconnected!",2000);
        serialBusy = false;
        return true;
    }
    if(client.isOpen()) keepAliveTimer->setInterval(600000);
    for(int i=0;i<2;i++)
    {
        processAsyncMessages();
        rb.clear();
        if (serial->isOpen())
        {
            QString m;
            if(message.length() > 100) for(int j=0;j<message.length();j++)
            {
                m = message.at(j);
                serial->write(m.toLocal8Bit());
                if(((j+1) % 100) == 0)
                {
                    if(serial->isOpen()) serial->waitForBytesWritten();
                    if(client.isOpen()) client.waitForBytesWritten();
                    msDelay(10);
                }
            }
            else serial->write(message.toLocal8Bit());
        }
        if (client.isOpen()) client.write(message.toLocal8Bit());
        if(message.length() > 100) waitforline(3000);
        else waitforline(1000);
        while(rb.numLines() >= 1)
        {
            res = rb.getline(&hasacknak);
            if((mrb != nullptr) && (hasacknak == false))
            {
                // If there is a message ring buffer, put the response into it
                mrb->putString(res + "\n");
                waitforline(100);
                continue; // Continue to read more lines if available
            }
            if(res == "") {serialBusy = false; return true;}
            if(res == "?")
            {
                res = message + " :NAK";
                if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + ", " + res.toLocal8Bit(),2000);
                else sb->showMessage(res.toLocal8Bit(),2000);
                serialBusy = false;
                return false;
            }
            break;
        }
    }
    // Here if the message transaction timed out
    res = message + " :Timeout";
    if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + ", " + res.toLocal8Bit(),2000);
    else sb->showMessage(res.toLocal8Bit(),2000);
    serialBusy = false;
    return true;
}

/*! \brief Comms::SendMessage (named overload)
 * Sends a message to MIPS and returns the response, only if name matches MIPSname.
 */
QString Comms::SendMessage(QString name, QString message)
{
    if((name == MIPSname) || (name == "")) return SendMessage(message);
    return "";
}


/*! \brief Comms::SendMess (named overload)
 * Convenience alias for SendMessage(name, message).
 */
QString Comms::SendMess(QString name, QString message)
{
    if((name == MIPSname) || (name == "")) return SendMessage(message);
    return "";
}

/*! \brief waitforline waits for a line to be received from MIPS.
 * This function will block until a line is received or the timeout expires.
 * If the timeout is 0 then it will wait indefinitely until a line is received.
 * If the timeout is -1 then it will read data from the serial port without waiting.
 */
void Comms::waitforline(int timeout)
{
    QElapsedTimer timer;

    if(timeout == 0)
    {
        while(1)
        {
            readData2RingBuffer();
            if(rb.numLines() > 0) break;
        }
        return;
    }
    if(timeout == -1)
    {
        readData2RingBuffer();
        return;
    }
    timer.start();
    while(timer.elapsed() < timeout)
    {
        readData2RingBuffer();
        if(rb.numLines() > 0) break;
    }
}

/*! \brief SendMessage sends a message to MIPS and waits for a response.
 * This function is used to send commands to MIPS and receive the response.
 * The function will return the response string or an empty string if there
 * was a timeout or error.
 */
QString Comms::SendMessage(QString message)
{
    QString res;
    bool    hasacknak=false;

    if(serialBusy)
    {
        if(properties != nullptr) properties->Log("SendMessage is busy!");
        return "";
    }
    serialBusy = true;
    if (!serial->isOpen() && !client.isOpen())
    {
        if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + " SM Disconnected!",2000);
        else sb->showMessage("SM Disconnected!",2000);
        serialBusy = false;
        return "";
    }
    if(client.isOpen()) keepAliveTimer->setInterval(600000);
    for(int i=0;i<2;i++)
    {
        processAsyncMessages();
        rb.clear();
        if (serial->isOpen())
        {
            QString m;
            if(message.length() > 100) for(int j=0;j<message.length();j++)
            {
                m = message.at(j);
                serial->write(m.toLocal8Bit());
                if(((j+1) % 100) == 0)
                {
                    if(serial->isOpen()) serial->waitForBytesWritten();
                    if(client.isOpen()) client.waitForBytesWritten();
                    msDelay(10);
                }
            }
            else serial->write(message.toLocal8Bit());
        }
        if (client.isOpen()) client.write(message.toLocal8Bit());
        if(message.length() > 100) waitforline(3000);
        else waitforline(1000);
        while(rb.numLines() >= 1)
        {
            res = rb.getline(&hasacknak);
            if((mrb != nullptr) && (hasacknak == false))
            {
                // If there is a message ring buffer, put the response into it
                mrb->putString(res + "\n");
                waitforline(100);
                continue; // Continue to read more lines if available
            }
                serialBusy = false;
            if(res != "") return res;
        }
    }
    // Here if the message transaction timed out
    res = message + " :Timeout";
    if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + ", " + res.toLocal8Bit(),2000);
    else sb->showMessage(res.toLocal8Bit(),2000);
    res = "";
    serialBusy = false;
    return res;
}

/*! \brief Comms::SendMess
 * Convenience alias for SendMessage(message).
 */
QString Comms::SendMess(QString message)
{
    return SendMessage(message);
}

// This function is called after a connection to MIPS is established. The function
// reads the MIPS name, its version and optionally sets the MIPS time and date based
// on version number.
// The function also looks in the app dir for a file that contains initalization
// commands and sends them to the MIPS system. The file name is "MIPS name".ini
/*! \brief Comms::GetMIPSnameAndVersion
 * Queries MIPS for its name and firmware version. Sets time/date if firmware
 * supports it. Sends any commands found in the app-directory .ini file.
 */
void Comms::GetMIPSnameAndVersion(void)
{
    MIPSname = SendMessage("GNAME\n");
    MIPSname = SendMessage("GNAME\n");
    // Get the version string
    QStringList reslist = SendMessage("GVER\n").split(" ");
    major = minor = 1;
    if(reslist.count() > 2)
    {
        QStringList verlist = reslist[1].split(".");
        if(verlist.count() == 2)
        {
            major = verlist[0].toInt();
            bool ok;
            for(int i=5;i>0;i--)
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
    // Open the ini file in the app dir and send data to MIPS.
#ifdef Q_OS_MAC
    QString ext = ".app";
#else
    QString ext = ".exe";
#endif
    int i = QApplication::applicationFilePath().indexOf(QApplication::applicationName() + ext);
    if(i == -1) return;
    QString FileName = QApplication::applicationFilePath().left(i) + QApplication::applicationName() + ".ini";
    QFile file(FileName);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            if(line.trimmed().startsWith("#")) continue;
            if(line.trimmed() == "") continue;
            // If here set the line to MIPS
            SendCommand(line + "\n");
        }while(!line.isNull());
        file.close();
        rb.clear();
    }
}

// Open connection to MIPS and read its name.
/*! \brief Comms::ConnectToMIPS
 * Opens a serial or TCP connection to MIPS and reads the device name and version.
 * Returns true if the connection was established successfully.
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

/*! \brief Comms::DisconnectFromMIPS
 * Closes the active TCP or serial connection and stops keep-alive timers.
 */
void Comms::DisconnectFromMIPS()
{
    if(client.isOpen())
    {
       client.close();
       keepAliveTimer->stop();
       reconnectTimer->stop();
    }
    closeSerialPort();
}

/*! \brief Comms::isAMPS
 * Opens the given serial port at the specified baud rate and checks whether
 * the connected device responds as an AMPS unit. Returns true if so.
 */
bool Comms::isAMPS(QString port, QString baud)
{
    QString res;

    if(serial->isOpen()) return false;  // If the port is open then return false
    disconnect(serial, &QSerialPort::errorOccurred, nullptr, nullptr);
    // Init the port parameters
    p.name = port;
    serial->setPortName(p.name);
#if defined(Q_OS_MAC)
    serial->setPortName("cu." + p.name);
#endif
    if(serial->isOpen()) return false;
    p.baudRate = baud.toInt();
    p.dataBits = QSerialPort::Data8;
    p.parity = QSerialPort::EvenParity;
    p.stopBits = QSerialPort::TwoStop;
    p.flowControl = QSerialPort::SoftwareControl;
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite))
    {
        serial->setDataTerminalReady(true);  // Required on PC but not on a MAC
        QApplication::processEvents(QEventLoop::AllEvents, 100);
        SendMessage("\n");   // Flush anything out!
        rb.clear();
        res = SendMessage("GVER\n");
        SendString("RTM,ON\n");
        QApplication::processEvents(QEventLoop::AllEvents, 100);
        serial->close();
        serial->close();
        serial->close();
        serial->close();
        serial->clearError();
        QApplication::processEvents(QEventLoop::AllEvents, 100);
        rb.clear();
        if(res.startsWith("V")) return true;
    }
    serial->clearError();
    rb.clear();
    return false;
}

/*! \brief Comms::isMIPS
 * Opens the given serial port at 115200 baud and checks whether the device
 * responds as a MIPS unit. Returns true if so.
 */
bool Comms::isMIPS(QString port)
{
    QString res;

    if(serial->isOpen()) return false;  // If the port is open then return false
    disconnect(serial, &QSerialPort::errorOccurred, nullptr, nullptr);
    // Init the port parameters
    p.name = port;
    serial->setPortName(p.name);
#if defined(Q_OS_MAC)
    serial->setPortName("cu." + p.name);
#endif
    if(serial->isOpen()) return false;
    p.baudRate = QSerialPort::Baud115200;
    p.dataBits = QSerialPort::Data8;
    p.parity = QSerialPort::NoParity;
    p.stopBits = QSerialPort::OneStop;
    p.flowControl = QSerialPort::NoFlowControl;
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite))
    {
        serial->setDataTerminalReady(true);  // Required on PC but not on a MAC
        QApplication::processEvents(QEventLoop::AllEvents, 100);
        res = SendMessage("GVER\n");
        SendString("ECHO,FALSE\n");
        QApplication::processEvents(QEventLoop::AllEvents, 100);
        serial->close();
        serial->close();
        serial->close();
        serial->close();
        serial->clearError();
        QApplication::processEvents(QEventLoop::AllEvents, 100);
        rb.clear();
        if(res.contains("Version")) return true;
        if(res.contains("version")) return true;
    }
    serial->clearError();
    rb.clear();
    return false;
}

/*! \brief Comms::msDelay
 * Blocking delay of the specified number of milliseconds.
 */
void Comms::msDelay(int ms)
{
    QThread::msleep(ms);

}

/*! \brief Comms::reopenSerialPort
 * Closes and reopens the serial port with a 250 ms pause between operations.
 */
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
            QApplication::processEvents();
            if(client_connected) return;
        }
        sb->showMessage(tr("MIPS failed to connect!"));
        client.abort();
        client.close();
        return;
    }
    else
    {
        serial->close();
        msDelay(250);
        serial->open(QIODevice::ReadWrite);
        serial->setDataTerminalReady(true);
    }
}

/*! \brief Comms::openSerialPort
 * Configures and opens the serial port using the stored settings.
 * Returns true on success; shows an error dialog on failure.
 */
bool Comms::openSerialPort()
{
    connect(serial, &QSerialPort::errorOccurred, this, &Comms::handleError);
    serial->setPortName(p.name);
    #if defined(Q_OS_MAC)
        serial->setPortName("cu." + p.name);
    #endif
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite))
    {
        sb->showMessage(QString("Connected to %1 : %2, %3, %4, %5, %6")
                            .arg(p.name,p.stringBaudRate,p.stringDataBits,p.stringParity,p.stringStopBits,p.stringFlowControl));
        return true;
    }
    else
    {
        QMessageBox::critical(this, QString("Error"), serial->errorString());
        sb->showMessage(tr("Open error: ") + serial->errorString());
    }
    return false;
}

/*! \brief Comms::closeSerialPort
 * Closes the serial port and disconnects the error-occurred signal handler.
 */
void Comms::closeSerialPort()
{
    if (serial->isOpen()) serial->close();
    serial->close();
    serial->close();
    serial->close();
    if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + " Closed!",2000);
    else sb->showMessage("Closed!",2000);
    disconnect(serial, &QSerialPort::errorOccurred, nullptr, nullptr);
}

/*! \brief Comms::handleError
 * Slot: handles serial port errors. On ResourceError, closes the port and
 * starts the reconnect timer if AutoRestore is enabled in properties.
 */
void Comms::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError)
    {
        QThread::sleep(1);
        closeSerialPort();
        if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + tr(" Critical Error, port closing: ") + serial->errorString());
        else sb->showMessage(tr("Critical Error, port closing: ") + serial->errorString());
        if(properties != nullptr)
        {
            if(properties->AutoRestore)
            {
                reconnectTimer->setInterval(2000);
                reconnectTimer->start();
            }
        }
    }
}

/*! \brief Comms::writeData
 * Writes a byte array to the open TCP socket or serial port in 100-byte chunks.
 */
void Comms::writeData(const QByteArray &data)
{
    if(client.isOpen())
    {
        for(int i=0;i<data.length();i++)
        {
            client.putChar(data.at(i));
            if(((i+1) % 100) == 0)
            {
                client.waitForBytesWritten();
                msDelay(10);
            }
        }
    }
    if(serial->isOpen())
    {
        for(int i=0;i<data.length();i++)
        {
            serial->putChar(data.at(i));
            if(((i+1) % 100) == 0)
            {
                serial->waitForBytesWritten();
                msDelay(10);
            }
        }
    }
}

/*! \brief Comms::readAvailableData2RingBuffer
 * Reads all currently available bytes from the open connection into the ring
 * buffer and emits DataReady. Does not block.
 */
void Comms::readAvailableData2RingBuffer(void)
{
    int i;

    if(client.isOpen())
    {
        if(client.bytesAvailable() > 0)
        {
           QByteArray data = client.readAll();
           for(i=0;i<data.size();i++) rb.putch(data[i]);
           emit DataReady();
        }
    }
    if(serial->isOpen())
    {
        if(serial->bytesAvailable() > 0)
        {
           QByteArray data = serial->readAll();
           for(i=0;i<data.size();i++) rb.putch(data[i]);
           emit DataReady();
        }
    }
}

/*! \brief Comms::readData2RingBuffer
 * Slot: reads incoming bytes into the ring buffer. Waits briefly if no bytes
 * are available. Emits lineAvailable when a complete line is present, and
 * DataReady after every read.
 */
void Comms::readData2RingBuffer(void)
{
    int i;

    if(client.isOpen())
    {
        if(client.bytesAvailable() == 0) client.waitForReadyRead(0);
        QByteArray data = client.readAll();
        for(i=0;i<data.size();i++) rb.putch(data[i]);
    }
    if(serial->isOpen())
    {
        if(serial->bytesAvailable() == 0) serial->waitForReadyRead(0);
        QByteArray data = serial->readAll();
        for(i=0;i<data.size();i++) rb.putch(data[i]);
    }
    if(rb.numLines() > 0) emit lineAvailable();
    emit DataReady();
}

/*! \brief Comms::connected
 * Slot: called when the TCP socket establishes a connection.
 */
void Comms::connected(void)
{
    if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + tr(" MIPS connected"));
    else sb->showMessage(tr("MIPS connected"));
    client_connected = true;
}

/*! \brief Comms::isConnected
 * Returns true if a TCP or serial connection is currently active.
 */
bool Comms::isConnected(void)
{
    if(client_connected) return true;
    if(serial->isOpen()) return true;
    return false;
}

/*! \brief Comms::disconnected
 * Slot: called when the TCP socket connection is lost.
 */
void Comms::disconnected(void)
{
    if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + " Disconnect signaled!",2000);
    else sb->showMessage("Disconnect signaled!!",2000);
}

/*! \brief Comms::getline
 * Returns the next complete line from the ring buffer.
 */
QString Comms::getline(void)
{
    return(rb.getline());
}

/*! \brief Comms::readall
 * Drains and returns all bytes currently in the ring buffer.
 */
QByteArray Comms::readall(void)
{
    QByteArray data;
    char c;

    while(1)
    {
      c = rb.getch();
      if(c==0) return data;
      data += c;
    }
}

/*! \brief Comms::slotAboutToClose
 * Slot: called when the TCP socket is about to close. Reserved for cleanup.
 */
void Comms::slotAboutToClose(void)
{
}

/*! \brief Comms::slotKeepAlive
 * Slot: sends a newline to keep the TCP connection alive.
 */
void Comms::slotKeepAlive(void)
{
   SendString("\n");
}

/*! \brief Comms::slotReconnect
 * Slot: attempts to reopen the serial port after a drop, stopping the
 * reconnect timer on success.
 */
void Comms::slotReconnect(void)
{
    if(!serial->isOpen())
    {
        serial->open(QIODevice::ReadWrite);
        serial->setDataTerminalReady(true);
        connect(serial, &QSerialPort::errorOccurred, this, &Comms::handleError);
    }
    if(serial->isOpen())
    {
        reconnectTimer->stop();
        if(!MIPSname.isEmpty()) sb->showMessage(MIPSname + tr(" Serial port reconnected!"));
        else sb->showMessage(tr("Serial port reconnected!"));
    }
}
