#ifndef MODBUS_H
#define MODBUS_H

#include <QLineEdit>
#include <QModbusRtuSerialClient>
#include <QStandardItemModel>
#include <QLabel>

class ModBus : public QWidget
{
    Q_OBJECT
public:
    ModBus(QString Port, QString Baud, QString Parity);
    ~ModBus();
    QModbusRtuSerialClient  modbusDevice;
    QString Status;
};

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
