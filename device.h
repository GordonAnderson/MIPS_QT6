// =============================================================================
// device.h
//
// Class declaration for Device.
// See device.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef DEVICE_H
#define DEVICE_H

#include "mips.h"

#include <QtCore/QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QStatusBar>
#include <QMessageBox>
#include <QObject>
#include <QLabel>
#include <QApplication>
#include <QFileInfo>

// -----------------------------------------------------------------------------
// Device — control panel widget for a generic serial-port instrument.
// Loads its configuration from a text file at panel load time, sends
// initialisation strings to the port, then on each update cycle sends a
// read command and parses the response with sscanf. The parsed value is
// scaled by M and B before display (display = M * rawValue + B).
// -----------------------------------------------------------------------------
class Device : public QWidget
{
    Q_OBJECT
public:
    Device(QWidget *parent, QString Name, QString FileName, QString units, int x, int y);
    ~Device();
    void    Show(void);
    void    Update(void);
    void    LoadDeviceConfig(void);
    QString Report(void);
    QString ProcessCommand(QString cmd);

    QString      RecString;  // raw unparsed response from last read cycle
    QSerialPort *serial;
    QWidget     *p;
    QString      Title;
    int          X, Y;
    QFrame      *frmDevice;

private:
    QLineEdit   *leDevice;
    QLabel      *labels[2];
    QString      ConfigFile; // path to the device configuration file
    QString      Port;       // serial port name, e.g. "/dev/ttyUSB0"
    QString      Units;      // display units label
    QString      EOL;        // end-of-line sequence sent after each command
    QStringList  InitList;   // initialisation strings sent at Show() time
    QString      ReadCommand;// command sent each update cycle to request data
    QString      ScanFormat; // sscanf format string to parse the response
    float        M;          // linear scale factor (display = M * raw + B)
    float        B;          // linear offset

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // DEVICE_H
