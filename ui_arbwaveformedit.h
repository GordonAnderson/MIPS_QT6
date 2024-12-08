/********************************************************************************
** Form generated from reading UI file 'arbwaveformedit.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ARBWAVEFORMEDIT_H
#define UI_ARBWAVEFORMEDIT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_ARBwaveformEdit
{
public:
    QDialogButtonBox *buttonBox;
    QPlainTextEdit *txtData;
    QLabel *label;
    QGroupBox *groupBox;
    QLineEdit *leUp;
    QLabel *label_2;
    QLabel *label_4;
    QPushButton *pbGenUpDown;
    QLabel *label_5;
    QLineEdit *lePhase;
    QPushButton *pbGenSine;
    QLineEdit *leDown;
    QLabel *label_6;
    QLineEdit *leCycles;
    QPushButton *pbPlot;
    QCustomPlot *plot;
    QPushButton *pbInvert;

    void setupUi(QDialog *ARBwaveformEdit)
    {
        if (ARBwaveformEdit->objectName().isEmpty())
            ARBwaveformEdit->setObjectName("ARBwaveformEdit");
        ARBwaveformEdit->resize(617, 494);
        buttonBox = new QDialogButtonBox(ARBwaveformEdit);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(455, 450, 151, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        txtData = new QPlainTextEdit(ARBwaveformEdit);
        txtData->setObjectName("txtData");
        txtData->setGeometry(QRect(15, 85, 181, 396));
        label = new QLabel(ARBwaveformEdit);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 5, 171, 66));
        label->setWordWrap(true);
        groupBox = new QGroupBox(ARBwaveformEdit);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(240, 10, 351, 121));
        leUp = new QLineEdit(groupBox);
        leUp->setObjectName("leUp");
        leUp->setGeometry(QRect(10, 35, 21, 21));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(40, 35, 31, 20));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(100, 35, 156, 16));
        pbGenUpDown = new QPushButton(groupBox);
        pbGenUpDown->setObjectName("pbGenUpDown");
        pbGenUpDown->setGeometry(QRect(250, 30, 91, 32));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(15, 65, 171, 16));
        lePhase = new QLineEdit(groupBox);
        lePhase->setObjectName("lePhase");
        lePhase->setGeometry(QRect(185, 65, 61, 21));
        pbGenSine = new QPushButton(groupBox);
        pbGenSine->setObjectName("pbGenSine");
        pbGenSine->setGeometry(QRect(250, 60, 91, 32));
        leDown = new QLineEdit(groupBox);
        leDown->setObjectName("leDown");
        leDown->setGeometry(QRect(70, 35, 21, 21));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(10, 90, 171, 16));
        leCycles = new QLineEdit(groupBox);
        leCycles->setObjectName("leCycles");
        leCycles->setGeometry(QRect(185, 90, 60, 21));
        pbPlot = new QPushButton(ARBwaveformEdit);
        pbPlot->setObjectName("pbPlot");
        pbPlot->setGeometry(QRect(330, 450, 121, 32));
        plot = new QCustomPlot(ARBwaveformEdit);
        plot->setObjectName("plot");
        plot->setGeometry(QRect(215, 145, 386, 291));
        pbInvert = new QPushButton(ARBwaveformEdit);
        pbInvert->setObjectName("pbInvert");
        pbInvert->setGeometry(QRect(200, 450, 131, 32));

        retranslateUi(ARBwaveformEdit);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ARBwaveformEdit, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ARBwaveformEdit, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(ARBwaveformEdit);
    } // setupUi

    void retranslateUi(QDialog *ARBwaveformEdit)
    {
        ARBwaveformEdit->setWindowTitle(QCoreApplication::translate("ARBwaveformEdit", "Arbirtary Waveform Editor", nullptr));
#if QT_CONFIG(tooltip)
        buttonBox->setToolTip(QCoreApplication::translate("ARBwaveformEdit", "Press OK to accept the waveform changes and download to the connected MIPS system.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        txtData->setToolTip(QCoreApplication::translate("ARBwaveformEdit", "Waveform data table. 32 total entries, one per line. Valid range is -100 to 100.", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("ARBwaveformEdit", "Arbitrary waveform data. Enter/edit values from -100 to 100 in the table below.", nullptr));
        groupBox->setTitle(QCoreApplication::translate("ARBwaveformEdit", "Waveform generation options", nullptr));
        leUp->setText(QCoreApplication::translate("ARBwaveformEdit", "4", nullptr));
        label_2->setText(QCoreApplication::translate("ARBwaveformEdit", "Up", nullptr));
        label_4->setText(QCoreApplication::translate("ARBwaveformEdit", "Down, square waveform", nullptr));
#if QT_CONFIG(tooltip)
        pbGenUpDown->setToolTip(QCoreApplication::translate("ARBwaveformEdit", "Generates a waveform and plots based on the Up and Down parameters defined. Up plus Down must be > 2 and less that 32.", nullptr));
#endif // QT_CONFIG(tooltip)
        pbGenUpDown->setText(QCoreApplication::translate("ARBwaveformEdit", "Generate", nullptr));
        label_5->setText(QCoreApplication::translate("ARBwaveformEdit", "Sine, with starting phase =", nullptr));
        lePhase->setText(QCoreApplication::translate("ARBwaveformEdit", "0", nullptr));
#if QT_CONFIG(tooltip)
        pbGenSine->setToolTip(QCoreApplication::translate("ARBwaveformEdit", "Generates a sine wave with user defined initial phase in degrees.", nullptr));
#endif // QT_CONFIG(tooltip)
        pbGenSine->setText(QCoreApplication::translate("ARBwaveformEdit", "Generate", nullptr));
        leDown->setText(QCoreApplication::translate("ARBwaveformEdit", "4", nullptr));
        label_6->setText(QCoreApplication::translate("ARBwaveformEdit", "Cycles per waveform", nullptr));
        leCycles->setText(QCoreApplication::translate("ARBwaveformEdit", "1", nullptr));
#if QT_CONFIG(tooltip)
        pbPlot->setToolTip(QCoreApplication::translate("ARBwaveformEdit", "Plots the waveform definded by the data in the wwaveform table.", nullptr));
#endif // QT_CONFIG(tooltip)
        pbPlot->setText(QCoreApplication::translate("ARBwaveformEdit", "Plot waveform", nullptr));
#if QT_CONFIG(tooltip)
        pbInvert->setToolTip(QCoreApplication::translate("ARBwaveformEdit", "Changes the sign of every entry in the data table and then plots the waveform.", nullptr));
#endif // QT_CONFIG(tooltip)
        pbInvert->setText(QCoreApplication::translate("ARBwaveformEdit", "Invert waveform", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ARBwaveformEdit: public Ui_ARBwaveformEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ARBWAVEFORMEDIT_H
