/********************************************************************************
** Form generated from reading UI file 'timinggenerator.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TIMINGGENERATOR_H
#define UI_TIMINGGENERATOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_TimingGenerator
{
public:
    QLabel *label_11;
    QGroupBox *grpEventEditor;
    QLineEdit *leEventStart;
    QComboBox *comboEventSignal;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *leEventWidth;
    QLabel *label_4;
    QLineEdit *leEventValue;
    QLineEdit *leEventName;
    QLabel *label_12;
    QLineEdit *leEventValueOff;
    QLabel *label_14;
    QLabel *label_9;
    QComboBox *comboClockSource;
    QGroupBox *grpFrameParameters;
    QLabel *label_6;
    QLineEdit *leFrameStart;
    QLineEdit *leFrameWidth;
    QLabel *label_7;
    QLabel *label_8;
    QLineEdit *leAccumulations;
    QLabel *label_13;
    QComboBox *comboEnable;
    QComboBox *comboTriggerSource;
    QLabel *label_10;
    QLineEdit *leTable;
    QComboBox *comboSelectEvent;
    QPushButton *pbGenerate;
    QLabel *label_5;
    QPushButton *pbClearEvents;
    QPushButton *pbSave;
    QPushButton *pbLoad;
    QComboBox *comboMuxOrder;
    QLabel *label_15;
    QCheckBox *chkTimeMode;
    QLabel *label_16;
    QLineEdit *leExtClkFreq;

    void setupUi(QDialog *TimingGenerator)
    {
        if (TimingGenerator->objectName().isEmpty())
            TimingGenerator->setObjectName("TimingGenerator");
        TimingGenerator->resize(663, 253);
        label_11 = new QLabel(TimingGenerator);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(10, 225, 59, 16));
        grpEventEditor = new QGroupBox(TimingGenerator);
        grpEventEditor->setObjectName("grpEventEditor");
        grpEventEditor->setGeometry(QRect(10, 35, 221, 181));
        leEventStart = new QLineEdit(grpEventEditor);
        leEventStart->setObjectName("leEventStart");
        leEventStart->setGeometry(QRect(100, 75, 100, 21));
        comboEventSignal = new QComboBox(grpEventEditor);
        comboEventSignal->setObjectName("comboEventSignal");
        comboEventSignal->setGeometry(QRect(100, 50, 100, 26));
        comboEventSignal->setEditable(false);
        comboEventSignal->setInsertPolicy(QComboBox::InsertAtBottom);
        label = new QLabel(grpEventEditor);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 50, 59, 20));
        label_2 = new QLabel(grpEventEditor);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 75, 59, 16));
        label_3 = new QLabel(grpEventEditor);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(20, 100, 59, 16));
        leEventWidth = new QLineEdit(grpEventEditor);
        leEventWidth->setObjectName("leEventWidth");
        leEventWidth->setGeometry(QRect(100, 100, 100, 21));
        label_4 = new QLabel(grpEventEditor);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(20, 125, 59, 16));
        leEventValue = new QLineEdit(grpEventEditor);
        leEventValue->setObjectName("leEventValue");
        leEventValue->setGeometry(QRect(100, 125, 100, 21));
        leEventName = new QLineEdit(grpEventEditor);
        leEventName->setObjectName("leEventName");
        leEventName->setGeometry(QRect(100, 25, 100, 21));
        leEventName->setReadOnly(true);
        label_12 = new QLabel(grpEventEditor);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(20, 25, 59, 20));
        leEventValueOff = new QLineEdit(grpEventEditor);
        leEventValueOff->setObjectName("leEventValueOff");
        leEventValueOff->setGeometry(QRect(100, 150, 100, 21));
        label_14 = new QLabel(grpEventEditor);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(20, 150, 59, 16));
        label_9 = new QLabel(TimingGenerator);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(465, 100, 101, 16));
        comboClockSource = new QComboBox(TimingGenerator);
        comboClockSource->setObjectName("comboClockSource");
        comboClockSource->setGeometry(QRect(560, 65, 100, 26));
        grpFrameParameters = new QGroupBox(TimingGenerator);
        grpFrameParameters->setObjectName("grpFrameParameters");
        grpFrameParameters->setGeometry(QRect(250, 50, 201, 161));
        label_6 = new QLabel(grpFrameParameters);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(10, 30, 59, 16));
        leFrameStart = new QLineEdit(grpFrameParameters);
        leFrameStart->setObjectName("leFrameStart");
        leFrameStart->setGeometry(QRect(110, 30, 80, 21));
        leFrameWidth = new QLineEdit(grpFrameParameters);
        leFrameWidth->setObjectName("leFrameWidth");
        leFrameWidth->setGeometry(QRect(110, 60, 80, 21));
        label_7 = new QLabel(grpFrameParameters);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(10, 60, 59, 16));
        label_8 = new QLabel(grpFrameParameters);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(10, 90, 91, 16));
        leAccumulations = new QLineEdit(grpFrameParameters);
        leAccumulations->setObjectName("leAccumulations");
        leAccumulations->setGeometry(QRect(110, 90, 80, 21));
        label_13 = new QLabel(grpFrameParameters);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(10, 125, 91, 16));
        comboEnable = new QComboBox(grpFrameParameters);
        comboEnable->setObjectName("comboEnable");
        comboEnable->setGeometry(QRect(109, 120, 80, 26));
        comboTriggerSource = new QComboBox(TimingGenerator);
        comboTriggerSource->setObjectName("comboTriggerSource");
        comboTriggerSource->setGeometry(QRect(560, 95, 100, 26));
        label_10 = new QLabel(TimingGenerator);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(465, 65, 91, 16));
        leTable = new QLineEdit(TimingGenerator);
        leTable->setObjectName("leTable");
        leTable->setGeometry(QRect(50, 225, 601, 21));
        comboSelectEvent = new QComboBox(TimingGenerator);
        comboSelectEvent->setObjectName("comboSelectEvent");
        comboSelectEvent->setGeometry(QRect(100, 5, 131, 26));
        pbGenerate = new QPushButton(TimingGenerator);
        pbGenerate->setObjectName("pbGenerate");
        pbGenerate->setGeometry(QRect(460, 155, 191, 32));
        pbGenerate->setAutoDefault(false);
        label_5 = new QLabel(TimingGenerator);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 5, 81, 20));
        pbClearEvents = new QPushButton(TimingGenerator);
        pbClearEvents->setObjectName("pbClearEvents");
        pbClearEvents->setGeometry(QRect(235, 4, 141, 32));
        pbClearEvents->setAutoDefault(false);
        pbSave = new QPushButton(TimingGenerator);
        pbSave->setObjectName("pbSave");
        pbSave->setGeometry(QRect(565, 190, 86, 32));
        pbSave->setAutoDefault(false);
        pbLoad = new QPushButton(TimingGenerator);
        pbLoad->setObjectName("pbLoad");
        pbLoad->setGeometry(QRect(465, 190, 86, 32));
        pbLoad->setAutoDefault(false);
        comboMuxOrder = new QComboBox(TimingGenerator);
        comboMuxOrder->addItem(QString());
        comboMuxOrder->addItem(QString());
        comboMuxOrder->addItem(QString());
        comboMuxOrder->addItem(QString());
        comboMuxOrder->addItem(QString());
        comboMuxOrder->addItem(QString());
        comboMuxOrder->addItem(QString());
        comboMuxOrder->setObjectName("comboMuxOrder");
        comboMuxOrder->setGeometry(QRect(560, 125, 100, 26));
        label_15 = new QLabel(TimingGenerator);
        label_15->setObjectName("label_15");
        label_15->setGeometry(QRect(465, 130, 101, 16));
        chkTimeMode = new QCheckBox(TimingGenerator);
        chkTimeMode->setObjectName("chkTimeMode");
        chkTimeMode->setGeometry(QRect(460, 5, 161, 20));
        label_16 = new QLabel(TimingGenerator);
        label_16->setObjectName("label_16");
        label_16->setGeometry(QRect(465, 35, 91, 16));
        leExtClkFreq = new QLineEdit(TimingGenerator);
        leExtClkFreq->setObjectName("leExtClkFreq");
        leExtClkFreq->setGeometry(QRect(564, 35, 91, 21));
        leExtClkFreq->setReadOnly(true);

        retranslateUi(TimingGenerator);

        QMetaObject::connectSlotsByName(TimingGenerator);
    } // setupUi

    void retranslateUi(QDialog *TimingGenerator)
    {
        TimingGenerator->setWindowTitle(QCoreApplication::translate("TimingGenerator", "Dialog", nullptr));
        label_11->setText(QCoreApplication::translate("TimingGenerator", "Table", nullptr));
        grpEventEditor->setTitle(QCoreApplication::translate("TimingGenerator", "Event editor", nullptr));
#if QT_CONFIG(tooltip)
        leEventStart->setToolTip(QCoreApplication::translate("TimingGenerator", "Event start point in clock cycles", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        comboEventSignal->setToolTip(QCoreApplication::translate("TimingGenerator", "Select event signal you wish to control", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("TimingGenerator", "Signal", nullptr));
        label_2->setText(QCoreApplication::translate("TimingGenerator", "Start", nullptr));
        label_3->setText(QCoreApplication::translate("TimingGenerator", "Width", nullptr));
#if QT_CONFIG(tooltip)
        leEventWidth->setToolTip(QCoreApplication::translate("TimingGenerator", "Event pulse width in clock cycles", nullptr));
#endif // QT_CONFIG(tooltip)
        label_4->setText(QCoreApplication::translate("TimingGenerator", "Value", nullptr));
#if QT_CONFIG(tooltip)
        leEventValue->setToolTip(QCoreApplication::translate("TimingGenerator", "Event active value", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        leEventName->setToolTip(QCoreApplication::translate("TimingGenerator", "Event name, must be unique", nullptr));
#endif // QT_CONFIG(tooltip)
        label_12->setText(QCoreApplication::translate("TimingGenerator", "Name", nullptr));
#if QT_CONFIG(tooltip)
        leEventValueOff->setToolTip(QCoreApplication::translate("TimingGenerator", "Event inactive value", nullptr));
#endif // QT_CONFIG(tooltip)
        label_14->setText(QCoreApplication::translate("TimingGenerator", "Value,off", nullptr));
        label_9->setText(QCoreApplication::translate("TimingGenerator", "Trigger source", nullptr));
        grpFrameParameters->setTitle(QCoreApplication::translate("TimingGenerator", "Frame parameters", nullptr));
        label_6->setText(QCoreApplication::translate("TimingGenerator", "Start", nullptr));
        leFrameStart->setText(QCoreApplication::translate("TimingGenerator", "10", nullptr));
        leFrameWidth->setText(QCoreApplication::translate("TimingGenerator", "2000", nullptr));
        label_7->setText(QCoreApplication::translate("TimingGenerator", "Width", nullptr));
        label_8->setText(QCoreApplication::translate("TimingGenerator", "Accumulations", nullptr));
        leAccumulations->setText(QCoreApplication::translate("TimingGenerator", "10", nullptr));
        label_13->setText(QCoreApplication::translate("TimingGenerator", "Enable", nullptr));
        label_10->setText(QCoreApplication::translate("TimingGenerator", "Clock source", nullptr));
        pbGenerate->setText(QCoreApplication::translate("TimingGenerator", "Generate", nullptr));
        label_5->setText(QCoreApplication::translate("TimingGenerator", "Select event", nullptr));
        pbClearEvents->setText(QCoreApplication::translate("TimingGenerator", "Clear all events", nullptr));
        pbSave->setText(QCoreApplication::translate("TimingGenerator", "Save", nullptr));
        pbLoad->setText(QCoreApplication::translate("TimingGenerator", "Load", nullptr));
        comboMuxOrder->setItemText(0, QCoreApplication::translate("TimingGenerator", "None", nullptr));
        comboMuxOrder->setItemText(1, QCoreApplication::translate("TimingGenerator", "4 Bit", nullptr));
        comboMuxOrder->setItemText(2, QCoreApplication::translate("TimingGenerator", "5 Bit", nullptr));
        comboMuxOrder->setItemText(3, QCoreApplication::translate("TimingGenerator", "6 Bit", nullptr));
        comboMuxOrder->setItemText(4, QCoreApplication::translate("TimingGenerator", "7 Bit", nullptr));
        comboMuxOrder->setItemText(5, QCoreApplication::translate("TimingGenerator", "8 Bit", nullptr));
        comboMuxOrder->setItemText(6, QCoreApplication::translate("TimingGenerator", "9 Bit", nullptr));

        label_15->setText(QCoreApplication::translate("TimingGenerator", "Mux order", nullptr));
        chkTimeMode->setText(QCoreApplication::translate("TimingGenerator", "Time mode,  in mS", nullptr));
        label_16->setText(QCoreApplication::translate("TimingGenerator", "Ext Clock Freq", nullptr));
#if QT_CONFIG(tooltip)
        leExtClkFreq->setToolTip(QCoreApplication::translate("TimingGenerator", "Event name, must be unique", nullptr));
#endif // QT_CONFIG(tooltip)
        leExtClkFreq->setText(QCoreApplication::translate("TimingGenerator", "6135", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TimingGenerator: public Ui_TimingGenerator {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIMINGGENERATOR_H
