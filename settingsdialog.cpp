/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 or version 3 as published by the Free Software
** Foundation. See LICENSE.LGPLv21 and LICENSE.LGPLv3 for details.
** $QT_END_LICENSE$
**
** GAA Modifications: adapted for MIPS host application, March 2026.
**
****************************************************************************/
// =============================================================================
// settingsdialog.cpp
//
// Serial port configuration dialog. Enumerates available ports via
// QSerialPortInfo, populates baud rate / data bits / parity / stop bits /
// flow control combo boxes, and stores the result in a Settings struct
// retrieved by settings(). Supports custom baud rate entry and custom
// device path entry. Re-enumerates ports on every showEvent() so the list
// stays current as devices are plugged/unplugged.
//
// On macOS, tty.* entries are suppressed and cu.* prefixes are stripped so
// only the bare port name is shown.
// =============================================================================
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>

QT_USE_NAMESPACE

// Placeholder string shown when a port info field is unavailable
static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    intValidator = new QIntValidator(0, 4000000, this);
    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(ui->applyButton,            SIGNAL(clicked()),               this, SLOT(apply()));
    connect(ui->serialPortInfoListBox,  SIGNAL(currentIndexChanged(int)), this, SLOT(showPortInfo(int)));
    connect(ui->baudRateBox,            SIGNAL(currentIndexChanged(int)), this, SLOT(checkCustomBaudRatePolicy(int)));
    connect(ui->serialPortInfoListBox,  SIGNAL(currentIndexChanged(int)), this, SLOT(checkCustomDevicePathPolicy(int)));

    fillPortsParameters();
    fillPortsInfo();
    updateSettings();
}

void SettingsDialog::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    fillPortsParameters();
    fillPortsInfo();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

// numberOfPorts — returns the number of detected serial ports (excluding
// the trailing "Custom" entry).
int SettingsDialog::numberOfPorts()
{
    return(ui->serialPortInfoListBox->count() - 1);
}

QString SettingsDialog::getPortName(int num)
{
    return(ui->serialPortInfoListBox->itemText(num));
}

SettingsDialog::Settings SettingsDialog::settings() const
{
    return currentSettings;
}

// showPortInfo — populates the description/manufacturer/serial/VID/PID labels
// for the port at combo index idx.
void SettingsDialog::showPortInfo(int idx)
{
    if(idx == -1) return;
    QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
    ui->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    ui->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void SettingsDialog::apply()
{
    updateSettings();
    hide();
}

// checkCustomBaudRatePolicy — makes the baud rate combo editable when the
// "Custom" item is selected and attaches an integer validator.
void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if(isCustomBaudRate)
    {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setValidator(intValidator);
    }
}

// checkCustomDevicePathPolicy — makes the port combo editable when the
// "Custom" item is selected so the user can type an arbitrary device path.
void SettingsDialog::checkCustomDevicePathPolicy(int idx)
{
    bool isCustomPath = !ui->serialPortInfoListBox->itemData(idx).isValid();
    ui->serialPortInfoListBox->setEditable(isCustomPath);
    if(isCustomPath) ui->serialPortInfoListBox->clearEditText();
}

// fillPortsParameters — populates baud rate, data bits, parity, stop bits,
// and flow control combo boxes with standard QSerialPort values.
void SettingsDialog::fillPortsParameters()
{
    ui->baudRateBox->clear();
    ui->baudRateBox->addItem(QStringLiteral("9600"),   QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"),  QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"),  QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(tr("Custom"));

    ui->dataBitsBox->clear();
    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->clear();
    ui->parityBox->addItem(tr("None"),  QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Even"),  QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Odd"),   QSerialPort::OddParity);
    ui->parityBox->addItem(tr("Mark"),  QSerialPort::MarkParity);
    ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->clear();
    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->clear();
    ui->flowControlBox->addItem(tr("None"),     QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(tr("RTS/CTS"),  QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

// fillPortsInfo — enumerates available serial ports. On macOS, tty.* entries
// are suppressed and the cu.* prefix is stripped to show only the device name.
void SettingsDialog::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    QString description, manufacturer, serialNumber;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QStringList list;
        description  = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        QString pname = info.portName();
        if(pname.startsWith("tty.")) pname.clear();
        if(pname.startsWith("cu."))  pname = pname.mid(3);
        if(!pname.isEmpty())
        {
            list << pname
                 << (!description.isEmpty()  ? description  : blankString)
                 << (!manufacturer.isEmpty() ? manufacturer : blankString)
                 << (!serialNumber.isEmpty() ? serialNumber : blankString)
                 << info.systemLocation()
                 << (info.vendorIdentifier()  ? QString::number(info.vendorIdentifier(),  16) : blankString)
                 << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);
            ui->serialPortInfoListBox->addItem(list.first(), list);
        }
    }
    ui->serialPortInfoListBox->addItem(tr("Custom"));
}

// updateSettings — reads the current UI state into currentSettings.
void SettingsDialog::updateSettings()
{
    currentSettings.name = ui->serialPortInfoListBox->currentText();

    if(ui->baudRateBox->currentIndex() == 4)
        currentSettings.baudRate = ui->baudRateBox->currentText().toInt();
    else
        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
            ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
        ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->dataBitsBox->currentText();

    currentSettings.parity = static_cast<QSerialPort::Parity>(
        ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    currentSettings.stringParity = ui->parityBox->currentText();

    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
        ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->stopBitsBox->currentText();

    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
        ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->flowControlBox->currentText();

    currentSettings.localEchoEnabled = ui->localEchoCheckBox->isChecked();
}
