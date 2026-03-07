// =============================================================================
// device.cpp
//
// Implements the Device control panel widget — a generic serial device reader
// that connects to an external instrument via a serial port, sends a read
// command on each update cycle, and displays the response as a scaled value.
//
// Configuration is loaded from a text .cfg file at panel load time. The file
// defines the port name, baud/parity/stop settings, end-of-line sequence,
// initialisation commands, read command, sscanf format string, and linear
// scaling factors M and B (display = M * rawValue + B).
//
// Hardware:    Any RS-232/USB-serial instrument
// Depends on:  mips.h, Utilities.h, QSerialPort
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "device.h"
#include "Utilities.h"

Device::Device(QWidget *parent, QString Name, QString FileName, QString units, int x, int y)
{
    InitList.clear();
    p          = parent;
    Title      = Name;
    X          = x;
    Y          = y;
    ConfigFile = FileName;
    M          = 1;
    B          = 0;
    Units      = units;
    serial = new QSerialPort(this);
}

Device::~Device()
{
    if(serial->isOpen()) serial->close();
}

// Show — creates the widget frame, value display, title and units labels,
// loads the device config file, and sends initialisation strings to the port.
void Device::Show(void)
{
    frmDevice = new QFrame(p);               frmDevice->setGeometry(X, Y, 180, 21);
    leDevice  = new QLineEdit(frmDevice);    leDevice->setGeometry(70, 0, 70, 21);
    labels[0] = new QLabel(Title, frmDevice); labels[0]->setGeometry(0,   0, 59, 16);
    labels[1] = new QLabel(Units, frmDevice); labels[1]->setGeometry(150, 0, 31, 16);

    LoadDeviceConfig();

    serial->setPortName(Port);
    if(serial->open(QIODevice::ReadWrite))
    {
        // Port opened — send initialisation strings then close
        for(int i = 0; i < InitList.count(); i++)
        {
            serial->write(InitList[i].toStdString().c_str());
            serial->write(EOL.toStdString().c_str());
        }
        serial->close();
    }
    else
    {
        leDevice->setText("Port: " + Port + " failed to open!");
    }
    frmDevice->installEventFilter(this);
    frmDevice->setMouseTracking(true);
    labels[0]->installEventFilter(this);
    labels[0]->setMouseTracking(true);
}

bool Device::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, frmDevice, labels[0], event)) return true;
    return false;
}

// LoadDeviceConfig — parses the device configuration file. Supported keywords:
//   PORT,name              — serial port name
//   SETTING,baud,bits,parity,stop — serial port parameters
//   EOL,CR|LF,...          — end-of-line sequence
//   INIT / END INIT        — block of initialisation strings sent at startup
//   READ COMMAND,cmd       — command sent each update cycle to request a reading
//   SCAN FORMAT,fmt        — sscanf format string to parse the response
//   M,value                — linear scale factor (display = M * raw + B)
//   B,value                — linear offset
void Device::LoadDeviceConfig(void)
{
    QStringList resList;

    QFile file(ConfigFile);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            if(line.trimmed().startsWith("#")) continue;
            resList = line.split(",");
            if((resList[0].toUpper() == "PORT") && (resList.length()==2))
            {
                Port = resList[1];
            }
            else if((resList[0].toUpper() == "SETTING") && (resList.length()==5))
            {
                if(resList[1] == "9600")   serial->setBaudRate(QSerialPort::Baud9600);
                else if(resList[1] == "19200")  serial->setBaudRate(QSerialPort::Baud19200);
                else if(resList[1] == "38400")  serial->setBaudRate(QSerialPort::Baud38400);
                else if(resList[1] == "57600")  serial->setBaudRate(QSerialPort::Baud57600);
                else if(resList[1] == "115200") serial->setBaudRate(QSerialPort::Baud115200);

                if(resList[2] == "5")      serial->setDataBits(QSerialPort::Data5);
                else if(resList[2] == "6") serial->setDataBits(QSerialPort::Data6);
                else if(resList[2] == "7") serial->setDataBits(QSerialPort::Data7);
                else if(resList[2] == "8") serial->setDataBits(QSerialPort::Data8);

                if(resList[3].toUpper() == "N")      serial->setParity(QSerialPort::NoParity);
                else if(resList[3].toUpper() == "O") serial->setParity(QSerialPort::OddParity);
                else if(resList[3].toUpper() == "E") serial->setParity(QSerialPort::EvenParity);
                else if(resList[3].toUpper() == "M") serial->setParity(QSerialPort::MarkParity);
                else if(resList[3].toUpper() == "S") serial->setParity(QSerialPort::SpaceParity);

                if(resList[4] == "1")      serial->setStopBits(QSerialPort::OneStop);
#ifdef Q_OS_WIN
                else if(resList[4] == "1.5") serial->setStopBits(QSerialPort::OneAndHalfStop);
#endif
                else if(resList[4] == "2") serial->setStopBits(QSerialPort::TwoStop);
                serial->setFlowControl(QSerialPort::NoFlowControl);
            }
            else if(resList[0].toUpper() == "EOL")
            {
                EOL.clear();
                for(int i = 1; i < resList.count(); i++)
                {
                    if(resList[i] == "CR") EOL += "\r";
                    if(resList[i] == "LF") EOL += "\n";
                }
            }
            else if((resList[0].toUpper() == "INIT") && (resList.length()==1))
            {
                do
                {
                    line = stream.readLine();
                    if(line.trimmed().startsWith("#")) continue;
                    if(line.toUpper() == "END INIT") break;
                    InitList.append(line);
                } while(!line.isNull());
            }
            else if((resList[0].toUpper() == "READ COMMAND") && (resList.length()==2)) ReadCommand = resList[1];
            else if((resList[0].toUpper() == "SCAN FORMAT")  && (resList.length()==2)) ScanFormat  = resList[1];
            else if((resList[0].toUpper() == "M")            && (resList.length()==2)) M = resList[1].toFloat();
            else if((resList[0].toUpper() == "B")            && (resList.length()==2)) B = resList[1].toFloat();
        } while(!line.isNull());
    }
    file.close();
}

// Update — called at the system update rate. Uses a two-call open/read/close
// pattern to allow the serial port to be shared among multiple device widgets:
//   Call 1: port is closed  — open port, send ReadCommand
//   Call 2: port is open    — read response, parse with ScanFormat, close port
// Display value = M * parsedValue + B.
void Device::Update(void)
{
    float fval = 0;

    if(serial->isOpen())
    {
        QByteArray data = serial->readAll();
        RecString = QString::fromStdString(data.toStdString());
        if(std::sscanf(RecString.toStdString().c_str(), ScanFormat.toStdString().c_str(), &fval) == 1)
        {
            leDevice->setText(QString::number(fval * M + B));
        }
        serial->close();
    }
    else
    {
        if(serial->open(QIODevice::ReadWrite))
        {
            serial->write(ReadCommand.toStdString().c_str());
            serial->write(EOL.toStdString().c_str());
        }
    }
}

QString Device::Report(void)
{
    QString res;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    res = title + "," + leDevice->text();
    return(res);
}

// ProcessCommand — handles scripting API commands for this widget.
// Supported commands:
//   title          — returns the current displayed value
//   title.Update   — forces an immediate hardware read
//   title.String   — returns the raw unparsed response string
// Returns "?" if the command is not recognised.
QString Device::ProcessCommand(QString cmd)
{
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";
    if(cmd == title)           return leDevice->text();
    if(cmd == title + ".Update") { Update(); return ""; }
    if(cmd == title + ".String") return RecString;
    return "?";
}
