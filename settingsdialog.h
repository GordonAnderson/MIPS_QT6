/****************************************************************************
** Copyright (C) 2012 Denis Shienkov, Laszlo Papp — Qt LGPL v2.1/v3
** GAA Modifications: adapted for MIPS host application, March 2026.
****************************************************************************/
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>

QT_USE_NAMESPACE
QT_BEGIN_NAMESPACE

namespace Ui { class SettingsDialog; }
class QIntValidator;

QT_END_NAMESPACE

// -----------------------------------------------------------------------------
// SettingsDialog — serial port configuration dialog. Populates available
// ports via QSerialPortInfo and exposes the result through settings().
// Re-enumerates ports on every showEvent().
// -----------------------------------------------------------------------------
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    // All parameters needed to open and configure a QSerialPort
    struct Settings {
        QString                  name;
        qint32                   baudRate;
        QString                  stringBaudRate;
        QSerialPort::DataBits    dataBits;
        QString                  stringDataBits;
        QSerialPort::Parity      parity;
        QString                  stringParity;
        QSerialPort::StopBits    stopBits;
        QString                  stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString                  stringFlowControl;
        bool                     localEchoEnabled;
    };

    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void     showEvent(QShowEvent *event);
    void     fillPortsInfo();
    void     fillPortsParameters();
    int      numberOfPorts();
    QString  getPortName(int num);
    Settings settings() const;

private slots:
    void showPortInfo(int idx);
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

private:
    void updateSettings();

    Ui::SettingsDialog *ui;
    Settings            currentSettings;
    QIntValidator      *intValidator;
};

#endif // SETTINGSDIALOG_H
