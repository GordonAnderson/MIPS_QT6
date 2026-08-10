// =============================================================================
// modbus.cpp
//
// Modbus RTU interface classes for the MIPS host application.
// Specifically designed for the Novus temperature controller but usable
// with any Modbus RTU device.
//
// Control panel config syntax:
//   ModBus,Port,Baud,Parity
//   ModChannel,Name,Add,Table,Chan,m,b,units,x,y
//
// Depends on:  Utilities.h, Qt Modbus (QModbusRtuSerialClient)
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "modbus.h"
#include "Utilities.h"

// ---------------------------------------------------------------------------
// ModBus
// ---------------------------------------------------------------------------

// ModBus — constructor. Configures and connects a QModbusRtuSerialClient to
// the given port, baud rate, and parity (N/E/O). Sets Status to "Connected"
// on success, or an error string on failure.
ModBus::ModBus(QString Port, QString Baud, QString Parity)
{
    int p = 0;
    if(Parity.toUpper() == "N") p = 0;
    if(Parity.toUpper() == "E") p = 2;
    if(Parity.toUpper() == "O") p = 3;
    if (modbusDevice.state() != QModbusDevice::ConnectedState)
    {
        modbusDevice.setConnectionParameter(QModbusDevice::SerialPortNameParameter,Port);
        modbusDevice.setConnectionParameter(QModbusDevice::SerialParityParameter,p);
        modbusDevice.setConnectionParameter(QModbusDevice::SerialBaudRateParameter,Baud);
        modbusDevice.setConnectionParameter(QModbusDevice::SerialDataBitsParameter,8);
        modbusDevice.setConnectionParameter(QModbusDevice::SerialStopBitsParameter,1);
    }
    modbusDevice.setTimeout(1000);
    modbusDevice.setNumberOfRetries(3);
    if (!modbusDevice.connectDevice()) Status = tr("Connect failed: ") + modbusDevice.errorString();
    else Status = "Connected";
}

// ~ModBus — disconnects the Modbus device.
ModBus::~ModBus()
{
    modbusDevice.disconnect();
}

// ---------------------------------------------------------------------------
// ModChannel
// ---------------------------------------------------------------------------

// ModChannel — constructor. Records the channel identity, Modbus address,
// register table, channel index, linear scale (M) and offset (B), and
// display position. Call Show() to create the visible widgets.
ModChannel::ModChannel(QWidget *parent, QString Name, int Add, int Table, int Chan, float m, float b, QString units,int x, int y)
{
    modbus     = NULL;
    p          = parent;
    Title      = Name;
    X          = x;
    Y          = y;
    M          = m;
    B          = b;
    Units      = units;
    address    = Add;
    table      = Table;
    chan       = Chan;
    Writable   = false;
}

// Show — creates the channel frame with a value line edit and unit label,
// installs drag support, and issues an initial Read() to populate the field.
void ModChannel::Show(void)
{
    frmModChannel = new QFrame(p);                frmModChannel->setGeometry(X,Y,180,21);
    leModChannel  = new QLineEdit(frmModChannel); leModChannel->setGeometry(70,0,70,21);
    labels[0] = new QLabel(Title,frmModChannel);  labels[0]->setGeometry(0,0,59,16);
    labels[1] = new QLabel(Units,frmModChannel);  labels[1]->setGeometry(150,0,31,16);
    if(leModChannel)
    {
        connect(leModChannel, &QLineEdit::returnPressed,   this, [this]() {leModChannel->setModified(true);});
        connect(leModChannel, &QLineEdit::editingFinished, this, &ModChannel::leModChannelChange);
    }
    frmModChannel->installEventFilter(this);
    frmModChannel->setMouseTracking(true);
    labels[0]->installEventFilter(this);
    labels[0]->setMouseTracking(true);
    Read();
}

// eventFilter — delegates drag-to-move to moveWidget().
bool ModChannel::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, frmModChannel, labels[0] , event)) return true;
    return false;
}

// Write — converts the displayed value through (val - B) / M and sends an
// asynchronous Modbus write request. Stores any error in WriteError.
void ModChannel::Write(void)
{
    if (modbus == NULL) return;

    QModbusDataUnit writeUnit = QModbusDataUnit((QModbusDataUnit::RegisterType)table,chan,1);
    float val = leModChannel->text().toFloat();
    val = (val - B)/M;
    WriteError.clear();
    QModbusDataUnit::RegisterType tbl = writeUnit.registerType();
    for (uint i = 0; i < writeUnit.valueCount(); i++) {
        if (tbl == QModbusDataUnit::Coils)
            writeUnit.setValue(i, (quint16)val);
        else
            writeUnit.setValue(i, (quint16)val);
    }
    if (auto *reply = modbus->modbusDevice.sendWriteRequest(writeUnit, address))
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, this, [this, reply]()
                    {
                        if (reply->error() == QModbusDevice::ProtocolError)
                        {
                            WriteError = tr("Write response error: %1 (Modbus exception: 0x%2)")
                            .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
                        } else if (reply->error() != QModbusDevice::NoError)
                        {
                            WriteError = tr("Write response error: %1 (code: 0x%2)").
                                         arg(reply->errorString()).arg(reply->error(), -1, 16);
                        }
                        reply->deleteLater();
                    });
        } else {
            // Broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        WriteError = tr("Write error: ") + modbus->modbusDevice.errorString();
    }
}

// Read — issues an asynchronous Modbus read request for one register. The
// result is applied in readReady() when the reply signal fires.
void ModChannel::Read(void)
{
    if (modbus == NULL) return;

    if (auto *reply = modbus->modbusDevice.sendReadRequest(QModbusDataUnit((QModbusDataUnit::RegisterType)table,chan,1), address))
    {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &ModChannel::readReady);
        else
            delete reply; // Broadcast replies return immediately
    } else ReadError = tr("Read error: ") + modbus->modbusDevice.errorString();
}

// readReady — slot called when the Modbus read reply arrives. Applies the
// linear scale (val * M + B) and updates the line edit, or stores the error
// string in ReadError on failure.
void ModChannel::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply) return;

    if (reply->error() == QModbusDevice::NoError)
    {
        const QModbusDataUnit unit = reply->result();
        for (uint i = 0; i < unit.valueCount(); i++)
        {
            leModChannel->setText(QString::number(unit.value(i) * M + B));
            ReadError.clear();
        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        ReadError = tr("Read response error: %1 (Modbus exception: 0x%2)").
                    arg(reply->errorString()).
                    arg(reply->rawResult().exceptionCode(), -1, 16);
    } else {
        ReadError = tr("Read response error: %1 (code: 0x%2)").
                    arg(reply->errorString()).
                    arg(reply->error(), -1, 16);
    }

    reply->deleteLater();
}

// Update — called on each control panel update cycle. Issues a Read() unless
// the channel is writable and the field currently has focus.
void ModChannel::Update(void)
{
    if((Writable) && (leModChannel->hasFocus())) return;
    Read();
}

// leModChannelChange — slot called on returnPressed. Triggers a Modbus write
// with the current line edit value.
void ModChannel::leModChannelChange(void)
{
    if(!leModChannel->isModified()) return;
    Write();
    leModChannel->setModified(false);
}

// Report — returns a "title,value" CSV string for method file save.
QString ModChannel::Report(void)
{
    QString res;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    res = title + "," + leModChannel->text();
    return(res);
}

// ProcessCommand — scripting API for this channel. Supports:
//   title          — returns the current value
//   title=val      — sets the value (only if Writable)
//   title.Update   — triggers an immediate read
//   title.Status   — returns connection/read/write status
// Returns "?" for unrecognised commands.
QString ModChannel::ProcessCommand(QString cmd)
{
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";
    if(cmd == title)
    {
        return leModChannel->text();
    }
    if(cmd == title + ".Update")
    {
        Update();
        return "";
    }
    if(cmd == title + ".Status")
    {
        // Returns the interface status — tests ModBus connection, read errors,
        // and write errors in order of severity.
        if(modbus == NULL) return "ModBus interface not initialized!";
        if(modbus->Status != "Connected") return modbus->Status;
        if(ReadError != "") return ReadError;
        if(WriteError != "") return WriteError;
        return "Ok";
    }
    if(!Writable) return "?";
    QStringList resList = cmd.split("=");
    if((resList.count()==2) && (resList[0].trimmed() == title.trimmed()))
    {
        leModChannel->setText(resList[1].trimmed());
        leModChannel->setModified(true);
        emit leModChannel->editingFinished();
        return "";
    }
    return "?";
}

// SetValues — parses a "title,value" CSV string and applies the value.
// No-op and returns false if the channel is not writable or the title
// does not match.
bool ModChannel::SetValues(QString strVals)
{
    QStringList resList;
    QString title;

    if(!Writable) return false;
    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!strVals.startsWith(title)) return false;
    resList = strVals.split(",");
    if(resList[0] != title) return false;
    if(resList.count() < 2) return false;
    leModChannel->setText(resList[1]);
    leModChannel->setModified(true);
    emit leModChannel->editingFinished();
    return true;
}
