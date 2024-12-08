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

class Device : public QWidget
{
    Q_OBJECT
public:
    Device(QWidget *parent, QString Name, QString FileName, QString units, int x, int y);
    ~Device();
    void Show(void);
    void Update(void);
    void LoadDeviceConfig(void);
    QString Report(void);
    QString ProcessCommand(QString cmd);
    QString RecString;
    QSerialPort *serial;
    QWidget *p;
    QString Title;
    int     X,Y;

    QFrame      *frmDevice;
private:
    QLineEdit   *leDevice;
    QLabel      *labels[2];
    QString     ConfigFile;
    QString     Port;
    QString     Units;
    QString     EOL;
    QStringList InitList;
    QString     ReadCommand;
    QString     ScanFormat;
    float       M;
    float       B;
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // DEVICE_H
