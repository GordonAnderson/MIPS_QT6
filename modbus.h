#ifndef MODBUS_H
#define MODBUS_H

// =============================================================================
// modbus.h
//
// Modbus RTU interface classes for the MIPS host application.
//
// Provides two classes:
//   ModBus     — owns the QModbusRtuSerialClient connection. Created once per
//                control panel via the ModBus config command and shared by all
//                ModChannel instances on that panel.
//   ModChannel — a single read/write channel widget backed by a ModBus instance.
//                Placed on a control panel via the ModChannel config command.
//
// Depends on:  Qt Modbus (QModbusRtuSerialClient)
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include <QLineEdit>
#include <QModbusRtuSerialClient>
#include <QStandardItemModel>
#include <QLabel>

// ModBus — owns a single QModbusRtuSerialClient connection.
// One instance is created per control panel and shared across all ModChannel
// widgets on that panel.
class ModBus : public QWidget
{
    Q_OBJECT
public:
    ModBus(QString Port, QString Baud, QString Parity);
    ~ModBus();
    QModbusRtuSerialClient  modbusDevice;
    QString Status;
};

// ModChannel — a single Modbus register channel displayed as a labeled
// line-edit widget. Supports read, write, and ProcessCommand scripting access.
class ModChannel : public QWidget
{
    Q_OBJECT
public:
    ModChannel(QWidget *parent, QString Name, int Add, int Table, int Chan, float m, float b, QString units,int x, int y);
    void Show(void);
    void Update(void);
    QString Report(void);
    QString ProcessCommand(QString cmd);
    bool SetValues(QString strVals);
    void Read(void);
    void Write(void);
    void readReady();
    ModBus  *modbus;
    QWidget *p;
    QString Title;
    QString ReadError;
    QString WriteError;
    int     X,Y;
    bool    Writable;
    QFrame      *frmModChannel;
private:
    QLineEdit   *leModChannel;
    QLabel      *labels[2];
    QString     Units;
    int         address;
    int         table;
    int         chan;
    float       M;
    float       B;
private slots:
    void leModChannelChange(void);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MODBUS_H
