#include "modbus.h"
#include "Utilities.h"

// Control panel support for ModBus, specfically designed for the Novus temp controller.
// The ModBus command must be issued first as follows:
// ModBus,Port,Baud,Parity
// This command will connect to the interface and configure the connection
// The ModChannel commands will place controls on the panel using the following
// syntax
// ModChannel,Name,Add,Table,Chan,m,b,units,x,y

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
    qDebug() << Status;
}

ModBus::~ModBus()
{
    modbusDevice.disconnect();
}


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

void ModChannel::Show(void)
{
    // Display UI
    frmModChannel = new QFrame(p);                frmModChannel->setGeometry(X,Y,180,21);
    leModChannel  = new QLineEdit(frmModChannel); leModChannel->setGeometry(70,0,70,21);
    labels[0] = new QLabel(Title,frmModChannel);  labels[0]->setGeometry(0,0,59,16);
    labels[1] = new QLabel(Units,frmModChannel);  labels[1]->setGeometry(150,0,31,16);
    connect(leModChannel,SIGNAL(editingFinished()),this,SLOT(leModChannelChange()));
    frmModChannel->installEventFilter(this);
    frmModChannel->setMouseTracking(true);
    labels[0]->installEventFilter(this);
    labels[0]->setMouseTracking(true);
    Read();
}

bool ModChannel::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, frmModChannel, labels[0] , event)) return true;
    return false;
}

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
                    WriteError = tr("Write response error: %1 (Mobus exception: 0x%2)")
                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
                } else if (reply->error() != QModbusDevice::NoError)
                {
                    WriteError = tr("Write response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).arg(reply->error(), -1, 16);
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        WriteError = tr("Write error: ") + modbus->modbusDevice.errorString();
    }
    if(WriteError != "") qDebug() << WriteError;
}


void ModChannel::Read(void)
{
    if (modbus == NULL) return;

    if (auto *reply = modbus->modbusDevice.sendReadRequest(QModbusDataUnit((QModbusDataUnit::RegisterType)table,chan,1), address))
    {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &ModChannel::readReady);
        else
            delete reply; // broadcast replies return immediately
    } else ReadError = tr("Read error: ") + modbus->modbusDevice.errorString();
}

void ModChannel::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply) return;

    if (reply->error() == QModbusDevice::NoError)
    {
        const QModbusDataUnit unit = reply->result();
        for (uint i = 0; i < unit.valueCount(); i++)
        {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress() + i)
                                     .arg(QString::number(unit.value(i),
                                     unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            leModChannel->setText(QString::number(unit.value(i) * M + B));
            ReadError.clear();
        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        ReadError = tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16);
    } else {
        ReadError = tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16);
    }

    reply->deleteLater();
    if(ReadError != "") qDebug() << ReadError;
}

void ModChannel::Update(void)
{
    if((Writable) && (leModChannel->hasFocus())) return;
    Read();
}

void ModChannel::leModChannelChange(void)
{
    Write();
}

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
        // This option will return the interface status, a number of things are tested
        // including the ModBus connection status.
        if(modbus == NULL) return "ModBus interface not initalized!";
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
