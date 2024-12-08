/********************************************************************************
** Form generated from reading UI file 'pse.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PSE_H
#define UI_PSE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_pseDialog
{
public:
    QGroupBox *gbCurrentPoint;
    QLineEdit *leName;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *leClocks;
    QLineEdit *leTime;
    QGroupBox *gbDigitalOut;
    QCheckBox *chkDOA;
    QCheckBox *chkDOB;
    QCheckBox *chkDOC;
    QCheckBox *chkDOD;
    QCheckBox *chkDOE;
    QCheckBox *chkDOF;
    QCheckBox *chkDOG;
    QCheckBox *chkDOH;
    QCheckBox *chkDOM;
    QCheckBox *chkDOJ;
    QCheckBox *chkDOL;
    QCheckBox *chkDOK;
    QCheckBox *chkDOO;
    QCheckBox *chkDOP;
    QCheckBox *chkDOI;
    QCheckBox *chkDON;
    QGroupBox *gbDCbias;
    QLabel *label_5;
    QLineEdit *leDCB1;
    QLabel *label_7;
    QLineEdit *leDCB2;
    QLabel *label_8;
    QLabel *label_9;
    QLineEdit *leDCB3;
    QLineEdit *leDCB4;
    QLineEdit *leDCB7;
    QLineEdit *leDCB8;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_13;
    QLineEdit *leDCB5;
    QLineEdit *leDCB6;
    QLabel *label_6;
    QLineEdit *leDCB11;
    QLineEdit *leDCB12;
    QLineEdit *leDCB16;
    QLineEdit *leDCB13;
    QLabel *label_14;
    QLabel *label_15;
    QLineEdit *leDCB14;
    QLabel *label_16;
    QLabel *label_17;
    QLineEdit *leDCB9;
    QLineEdit *leDCB10;
    QLabel *label_18;
    QLabel *label_19;
    QLineEdit *leDCB15;
    QLabel *label_20;
    QPushButton *pbNext;
    QPushButton *pbInsert;
    QPushButton *pbDelete;
    QPushButton *pbPrevious;
    QFrame *frmLoop;
    QLabel *label_4;
    QLineEdit *leCycles;
    QComboBox *comboLoop;
    QCheckBox *chkLoop;
    QPushButton *pbInsertPulse;
    QGroupBox *gbInsertPulse;
    QLabel *label_21;
    QLabel *label_22;
    QLabel *label_23;
    QLabel *label_24;
    QLineEdit *lePulseWidth;
    QLineEdit *leRampUp;
    QLineEdit *leRampDwn;
    QLineEdit *leRampStepSize;
    QLabel *label_25;
    QLabel *label_26;
    QPushButton *pbInsertRamp;
    QLabel *label_27;
    QLabel *label_28;
    QLabel *label_29;
    QLineEdit *lePulseChannel;
    QLineEdit *lePulseVoltage;
    QLabel *label_30;
    QPushButton *pbInsertCancel;

    void setupUi(QDialog *pseDialog)
    {
        if (pseDialog->objectName().isEmpty())
            pseDialog->setObjectName("pseDialog");
        pseDialog->resize(460, 610);
        gbCurrentPoint = new QGroupBox(pseDialog);
        gbCurrentPoint->setObjectName("gbCurrentPoint");
        gbCurrentPoint->setGeometry(QRect(10, 0, 441, 121));
        leName = new QLineEdit(gbCurrentPoint);
        leName->setObjectName("leName");
        leName->setGeometry(QRect(310, 30, 113, 21));
        label = new QLabel(gbCurrentPoint);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 30, 101, 21));
        label_2 = new QLabel(gbCurrentPoint);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 60, 311, 21));
        label_3 = new QLabel(gbCurrentPoint);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 85, 276, 26));
        leClocks = new QLineEdit(gbCurrentPoint);
        leClocks->setObjectName("leClocks");
        leClocks->setGeometry(QRect(310, 60, 113, 21));
        leTime = new QLineEdit(gbCurrentPoint);
        leTime->setObjectName("leTime");
        leTime->setGeometry(QRect(310, 90, 113, 21));
        gbDigitalOut = new QGroupBox(pseDialog);
        gbDigitalOut->setObjectName("gbDigitalOut");
        gbDigitalOut->setGeometry(QRect(15, 235, 131, 361));
        chkDOA = new QCheckBox(gbDigitalOut);
        chkDOA->setObjectName("chkDOA");
        chkDOA->setGeometry(QRect(20, 30, 60, 20));
        chkDOB = new QCheckBox(gbDigitalOut);
        chkDOB->setObjectName("chkDOB");
        chkDOB->setGeometry(QRect(20, 50, 60, 20));
        chkDOC = new QCheckBox(gbDigitalOut);
        chkDOC->setObjectName("chkDOC");
        chkDOC->setGeometry(QRect(20, 70, 60, 20));
        chkDOD = new QCheckBox(gbDigitalOut);
        chkDOD->setObjectName("chkDOD");
        chkDOD->setGeometry(QRect(20, 90, 60, 20));
        chkDOE = new QCheckBox(gbDigitalOut);
        chkDOE->setObjectName("chkDOE");
        chkDOE->setGeometry(QRect(20, 110, 60, 20));
        chkDOF = new QCheckBox(gbDigitalOut);
        chkDOF->setObjectName("chkDOF");
        chkDOF->setGeometry(QRect(20, 130, 60, 20));
        chkDOG = new QCheckBox(gbDigitalOut);
        chkDOG->setObjectName("chkDOG");
        chkDOG->setGeometry(QRect(20, 150, 60, 20));
        chkDOH = new QCheckBox(gbDigitalOut);
        chkDOH->setObjectName("chkDOH");
        chkDOH->setGeometry(QRect(20, 170, 60, 20));
        chkDOM = new QCheckBox(gbDigitalOut);
        chkDOM->setObjectName("chkDOM");
        chkDOM->setGeometry(QRect(20, 270, 60, 20));
        chkDOJ = new QCheckBox(gbDigitalOut);
        chkDOJ->setObjectName("chkDOJ");
        chkDOJ->setGeometry(QRect(20, 210, 60, 20));
        chkDOL = new QCheckBox(gbDigitalOut);
        chkDOL->setObjectName("chkDOL");
        chkDOL->setGeometry(QRect(20, 250, 60, 20));
        chkDOK = new QCheckBox(gbDigitalOut);
        chkDOK->setObjectName("chkDOK");
        chkDOK->setGeometry(QRect(20, 230, 60, 20));
        chkDOO = new QCheckBox(gbDigitalOut);
        chkDOO->setObjectName("chkDOO");
        chkDOO->setGeometry(QRect(20, 310, 60, 20));
        chkDOP = new QCheckBox(gbDigitalOut);
        chkDOP->setObjectName("chkDOP");
        chkDOP->setGeometry(QRect(20, 330, 60, 20));
        chkDOI = new QCheckBox(gbDigitalOut);
        chkDOI->setObjectName("chkDOI");
        chkDOI->setGeometry(QRect(20, 190, 60, 20));
        chkDON = new QCheckBox(gbDigitalOut);
        chkDON->setObjectName("chkDON");
        chkDON->setGeometry(QRect(20, 290, 60, 20));
        gbDCbias = new QGroupBox(pseDialog);
        gbDCbias->setObjectName("gbDCbias");
        gbDCbias->setGeometry(QRect(155, 235, 296, 361));
        label_5 = new QLabel(gbDCbias);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 40, 50, 16));
        leDCB1 = new QLineEdit(gbDCbias);
        leDCB1->setObjectName("leDCB1");
        leDCB1->setGeometry(QRect(60, 40, 71, 21));
        label_7 = new QLabel(gbDCbias);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(10, 80, 50, 16));
        leDCB2 = new QLineEdit(gbDCbias);
        leDCB2->setObjectName("leDCB2");
        leDCB2->setGeometry(QRect(60, 80, 71, 21));
        label_8 = new QLabel(gbDCbias);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(10, 120, 50, 16));
        label_9 = new QLabel(gbDCbias);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(10, 160, 50, 16));
        leDCB3 = new QLineEdit(gbDCbias);
        leDCB3->setObjectName("leDCB3");
        leDCB3->setGeometry(QRect(60, 120, 71, 21));
        leDCB4 = new QLineEdit(gbDCbias);
        leDCB4->setObjectName("leDCB4");
        leDCB4->setGeometry(QRect(60, 160, 71, 21));
        leDCB7 = new QLineEdit(gbDCbias);
        leDCB7->setObjectName("leDCB7");
        leDCB7->setGeometry(QRect(60, 280, 71, 21));
        leDCB8 = new QLineEdit(gbDCbias);
        leDCB8->setObjectName("leDCB8");
        leDCB8->setGeometry(QRect(60, 320, 71, 21));
        label_10 = new QLabel(gbDCbias);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(10, 200, 50, 16));
        label_11 = new QLabel(gbDCbias);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(10, 280, 50, 16));
        label_12 = new QLabel(gbDCbias);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(10, 240, 50, 16));
        label_13 = new QLabel(gbDCbias);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(10, 320, 50, 16));
        leDCB5 = new QLineEdit(gbDCbias);
        leDCB5->setObjectName("leDCB5");
        leDCB5->setGeometry(QRect(60, 200, 71, 21));
        leDCB6 = new QLineEdit(gbDCbias);
        leDCB6->setObjectName("leDCB6");
        leDCB6->setGeometry(QRect(60, 240, 71, 21));
        label_6 = new QLabel(gbDCbias);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(140, 45, 60, 16));
        leDCB11 = new QLineEdit(gbDCbias);
        leDCB11->setObjectName("leDCB11");
        leDCB11->setGeometry(QRect(205, 120, 71, 21));
        leDCB12 = new QLineEdit(gbDCbias);
        leDCB12->setObjectName("leDCB12");
        leDCB12->setGeometry(QRect(205, 160, 71, 21));
        leDCB16 = new QLineEdit(gbDCbias);
        leDCB16->setObjectName("leDCB16");
        leDCB16->setGeometry(QRect(205, 320, 71, 21));
        leDCB13 = new QLineEdit(gbDCbias);
        leDCB13->setObjectName("leDCB13");
        leDCB13->setGeometry(QRect(205, 200, 71, 21));
        label_14 = new QLabel(gbDCbias);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(140, 325, 60, 16));
        label_15 = new QLabel(gbDCbias);
        label_15->setObjectName("label_15");
        label_15->setGeometry(QRect(140, 125, 60, 16));
        leDCB14 = new QLineEdit(gbDCbias);
        leDCB14->setObjectName("leDCB14");
        leDCB14->setGeometry(QRect(205, 240, 71, 21));
        label_16 = new QLabel(gbDCbias);
        label_16->setObjectName("label_16");
        label_16->setGeometry(QRect(140, 85, 60, 16));
        label_17 = new QLabel(gbDCbias);
        label_17->setObjectName("label_17");
        label_17->setGeometry(QRect(140, 165, 60, 16));
        leDCB9 = new QLineEdit(gbDCbias);
        leDCB9->setObjectName("leDCB9");
        leDCB9->setGeometry(QRect(205, 40, 71, 21));
        leDCB10 = new QLineEdit(gbDCbias);
        leDCB10->setObjectName("leDCB10");
        leDCB10->setGeometry(QRect(205, 80, 71, 21));
        label_18 = new QLabel(gbDCbias);
        label_18->setObjectName("label_18");
        label_18->setGeometry(QRect(140, 285, 60, 16));
        label_19 = new QLabel(gbDCbias);
        label_19->setObjectName("label_19");
        label_19->setGeometry(QRect(140, 245, 60, 16));
        leDCB15 = new QLineEdit(gbDCbias);
        leDCB15->setObjectName("leDCB15");
        leDCB15->setGeometry(QRect(205, 280, 71, 21));
        label_20 = new QLabel(gbDCbias);
        label_20->setObjectName("label_20");
        label_20->setGeometry(QRect(140, 205, 60, 16));
        pbNext = new QPushButton(pseDialog);
        pbNext->setObjectName("pbNext");
        pbNext->setGeometry(QRect(0, 130, 90, 32));
        pbNext->setAutoDefault(false);
        pbInsert = new QPushButton(pseDialog);
        pbInsert->setObjectName("pbInsert");
        pbInsert->setGeometry(QRect(90, 130, 90, 32));
        pbInsert->setAutoDefault(false);
        pbDelete = new QPushButton(pseDialog);
        pbDelete->setObjectName("pbDelete");
        pbDelete->setGeometry(QRect(360, 130, 90, 32));
        pbDelete->setAutoDefault(false);
        pbPrevious = new QPushButton(pseDialog);
        pbPrevious->setObjectName("pbPrevious");
        pbPrevious->setGeometry(QRect(273, 130, 90, 32));
        pbPrevious->setAutoDefault(false);
        frmLoop = new QFrame(pseDialog);
        frmLoop->setObjectName("frmLoop");
        frmLoop->setGeometry(QRect(10, 174, 441, 51));
        frmLoop->setStyleSheet(QString::fromUtf8("border: 0"));
        frmLoop->setFrameShape(QFrame::StyledPanel);
        frmLoop->setFrameShadow(QFrame::Plain);
        frmLoop->setLineWidth(0);
        label_4 = new QLabel(frmLoop);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(220, 11, 171, 26));
        leCycles = new QLineEdit(frmLoop);
        leCycles->setObjectName("leCycles");
        leCycles->setGeometry(QRect(381, 15, 51, 21));
        comboLoop = new QComboBox(frmLoop);
        comboLoop->setObjectName("comboLoop");
        comboLoop->setGeometry(QRect(105, 12, 106, 26));
        chkLoop = new QCheckBox(frmLoop);
        chkLoop->setObjectName("chkLoop");
        chkLoop->setGeometry(QRect(5, 11, 106, 26));
        pbInsertPulse = new QPushButton(pseDialog);
        pbInsertPulse->setObjectName("pbInsertPulse");
        pbInsertPulse->setGeometry(QRect(173, 130, 105, 32));
        pbInsertPulse->setAutoDefault(false);
        gbInsertPulse = new QGroupBox(pseDialog);
        gbInsertPulse->setObjectName("gbInsertPulse");
        gbInsertPulse->setGeometry(QRect(8, 162, 441, 431));
        label_21 = new QLabel(gbInsertPulse);
        label_21->setObjectName("label_21");
        label_21->setGeometry(QRect(30, 225, 86, 16));
        label_22 = new QLabel(gbInsertPulse);
        label_22->setObjectName("label_22");
        label_22->setGeometry(QRect(30, 250, 111, 36));
        label_22->setWordWrap(true);
        label_23 = new QLabel(gbInsertPulse);
        label_23->setObjectName("label_23");
        label_23->setGeometry(QRect(30, 295, 111, 36));
        label_23->setWordWrap(true);
        label_24 = new QLabel(gbInsertPulse);
        label_24->setObjectName("label_24");
        label_24->setGeometry(QRect(30, 335, 111, 26));
        label_24->setWordWrap(true);
        lePulseWidth = new QLineEdit(gbInsertPulse);
        lePulseWidth->setObjectName("lePulseWidth");
        lePulseWidth->setGeometry(QRect(180, 225, 113, 21));
        leRampUp = new QLineEdit(gbInsertPulse);
        leRampUp->setObjectName("leRampUp");
        leRampUp->setGeometry(QRect(180, 260, 113, 21));
        leRampDwn = new QLineEdit(gbInsertPulse);
        leRampDwn->setObjectName("leRampDwn");
        leRampDwn->setGeometry(QRect(180, 305, 113, 21));
        leRampStepSize = new QLineEdit(gbInsertPulse);
        leRampStepSize->setObjectName("leRampStepSize");
        leRampStepSize->setGeometry(QRect(180, 340, 113, 21));
        label_25 = new QLabel(gbInsertPulse);
        label_25->setObjectName("label_25");
        label_25->setGeometry(QRect(315, 230, 86, 16));
        label_26 = new QLabel(gbInsertPulse);
        label_26->setObjectName("label_26");
        label_26->setGeometry(QRect(315, 340, 86, 16));
        pbInsertRamp = new QPushButton(gbInsertPulse);
        pbInsertRamp->setObjectName("pbInsertRamp");
        pbInsertRamp->setGeometry(QRect(297, 385, 131, 32));
        label_27 = new QLabel(gbInsertPulse);
        label_27->setObjectName("label_27");
        label_27->setGeometry(QRect(25, 25, 396, 91));
        label_27->setWordWrap(true);
        label_28 = new QLabel(gbInsertPulse);
        label_28->setObjectName("label_28");
        label_28->setGeometry(QRect(30, 120, 106, 16));
        label_29 = new QLabel(gbInsertPulse);
        label_29->setObjectName("label_29");
        label_29->setGeometry(QRect(30, 145, 141, 66));
        label_29->setWordWrap(true);
        lePulseChannel = new QLineEdit(gbInsertPulse);
        lePulseChannel->setObjectName("lePulseChannel");
        lePulseChannel->setGeometry(QRect(180, 120, 113, 21));
        lePulseVoltage = new QLineEdit(gbInsertPulse);
        lePulseVoltage->setObjectName("lePulseVoltage");
        lePulseVoltage->setGeometry(QRect(180, 165, 113, 21));
        label_30 = new QLabel(gbInsertPulse);
        label_30->setObjectName("label_30");
        label_30->setGeometry(QRect(315, 170, 86, 16));
        pbInsertCancel = new QPushButton(gbInsertPulse);
        pbInsertCancel->setObjectName("pbInsertCancel");
        pbInsertCancel->setGeometry(QRect(160, 386, 131, 32));
        label_21->raise();
        label_22->raise();
        label_23->raise();
        label_24->raise();
        lePulseWidth->raise();
        leRampUp->raise();
        leRampDwn->raise();
        leRampStepSize->raise();
        label_25->raise();
        label_26->raise();
        pbInsertRamp->raise();
        label_28->raise();
        label_29->raise();
        lePulseChannel->raise();
        lePulseVoltage->raise();
        label_30->raise();
        label_27->raise();
        pbInsertCancel->raise();
        gbDigitalOut->raise();
        gbDCbias->raise();
        gbCurrentPoint->raise();
        pbNext->raise();
        pbInsert->raise();
        pbDelete->raise();
        pbPrevious->raise();
        frmLoop->raise();
        pbInsertPulse->raise();
        gbInsertPulse->raise();

        retranslateUi(pseDialog);

        QMetaObject::connectSlotsByName(pseDialog);
    } // setupUi

    void retranslateUi(QDialog *pseDialog)
    {
        pseDialog->setWindowTitle(QCoreApplication::translate("pseDialog", "Pulse Sequence Editor", nullptr));
        gbCurrentPoint->setTitle(QCoreApplication::translate("pseDialog", "Current time point: 1 of 1", nullptr));
#if QT_CONFIG(tooltip)
        leName->setToolTip(QCoreApplication::translate("pseDialog", "Defines the name of this time point.", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("pseDialog", "Name", nullptr));
        label_2->setText(QCoreApplication::translate("pseDialog", "Number of clocks from start of sequence", nullptr));
        label_3->setText(QCoreApplication::translate("pseDialog", "Time from start of sequence (not used)", nullptr));
#if QT_CONFIG(tooltip)
        leClocks->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        leTime->setToolTip(QCoreApplication::translate("pseDialog", "Not currently used", nullptr));
#endif // QT_CONFIG(tooltip)
        gbDigitalOut->setTitle(QCoreApplication::translate("pseDialog", "Digital out", nullptr));
        chkDOA->setText(QCoreApplication::translate("pseDialog", "A", nullptr));
        chkDOB->setText(QCoreApplication::translate("pseDialog", "B", nullptr));
        chkDOC->setText(QCoreApplication::translate("pseDialog", "C", nullptr));
        chkDOD->setText(QCoreApplication::translate("pseDialog", "D", nullptr));
        chkDOE->setText(QCoreApplication::translate("pseDialog", "E", nullptr));
        chkDOF->setText(QCoreApplication::translate("pseDialog", "F", nullptr));
        chkDOG->setText(QCoreApplication::translate("pseDialog", "G", nullptr));
        chkDOH->setText(QCoreApplication::translate("pseDialog", "H", nullptr));
        chkDOM->setText(QCoreApplication::translate("pseDialog", "M", nullptr));
        chkDOJ->setText(QCoreApplication::translate("pseDialog", "J", nullptr));
        chkDOL->setText(QCoreApplication::translate("pseDialog", "L", nullptr));
        chkDOK->setText(QCoreApplication::translate("pseDialog", "K", nullptr));
        chkDOO->setText(QCoreApplication::translate("pseDialog", "O", nullptr));
        chkDOP->setText(QCoreApplication::translate("pseDialog", "P", nullptr));
        chkDOI->setText(QCoreApplication::translate("pseDialog", "I", nullptr));
        chkDON->setText(QCoreApplication::translate("pseDialog", "N", nullptr));
        gbDCbias->setTitle(QCoreApplication::translate("pseDialog", "DC bias outputs", nullptr));
        label_5->setText(QCoreApplication::translate("pseDialog", "CH 1", nullptr));
        label_7->setText(QCoreApplication::translate("pseDialog", "CH 2", nullptr));
        label_8->setText(QCoreApplication::translate("pseDialog", "CH 3", nullptr));
        label_9->setText(QCoreApplication::translate("pseDialog", "CH 4", nullptr));
        label_10->setText(QCoreApplication::translate("pseDialog", "CH 5", nullptr));
        label_11->setText(QCoreApplication::translate("pseDialog", "CH 7", nullptr));
        label_12->setText(QCoreApplication::translate("pseDialog", "CH 6", nullptr));
        label_13->setText(QCoreApplication::translate("pseDialog", "CH 8", nullptr));
        label_6->setText(QCoreApplication::translate("pseDialog", "CH 9", nullptr));
        label_14->setText(QCoreApplication::translate("pseDialog", "CH 16", nullptr));
        label_15->setText(QCoreApplication::translate("pseDialog", "CH 11", nullptr));
        label_16->setText(QCoreApplication::translate("pseDialog", "CH 10", nullptr));
        label_17->setText(QCoreApplication::translate("pseDialog", "CH 12", nullptr));
        label_18->setText(QCoreApplication::translate("pseDialog", "CH 15", nullptr));
        label_19->setText(QCoreApplication::translate("pseDialog", "CH 14", nullptr));
        label_20->setText(QCoreApplication::translate("pseDialog", "CH 13", nullptr));
#if QT_CONFIG(tooltip)
        pbNext->setToolTip(QCoreApplication::translate("pseDialog", "Advance to next time point", nullptr));
#endif // QT_CONFIG(tooltip)
        pbNext->setText(QCoreApplication::translate("pseDialog", "Next", nullptr));
#if QT_CONFIG(tooltip)
        pbInsert->setToolTip(QCoreApplication::translate("pseDialog", "Insert a time point after this one", nullptr));
#endif // QT_CONFIG(tooltip)
        pbInsert->setText(QCoreApplication::translate("pseDialog", "Insert", nullptr));
#if QT_CONFIG(tooltip)
        pbDelete->setToolTip(QCoreApplication::translate("pseDialog", "Delete this time point", nullptr));
#endif // QT_CONFIG(tooltip)
        pbDelete->setText(QCoreApplication::translate("pseDialog", "Delete", nullptr));
#if QT_CONFIG(tooltip)
        pbPrevious->setToolTip(QCoreApplication::translate("pseDialog", "Go back to previous time point", nullptr));
#endif // QT_CONFIG(tooltip)
        pbPrevious->setText(QCoreApplication::translate("pseDialog", "Previous", nullptr));
        label_4->setText(QCoreApplication::translate("pseDialog", "Number of cycles", nullptr));
#if QT_CONFIG(tooltip)
        leCycles->setToolTip(QCoreApplication::translate("pseDialog", "Number of times to loop", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        comboLoop->setToolTip(QCoreApplication::translate("pseDialog", "Point to loop back to", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        chkLoop->setToolTip(QCoreApplication::translate("pseDialog", "Check this to cause this time point to loop back to the labeled point", nullptr));
#endif // QT_CONFIG(tooltip)
        chkLoop->setText(QCoreApplication::translate("pseDialog", "Loop to", nullptr));
#if QT_CONFIG(tooltip)
        pbInsertPulse->setToolTip(QCoreApplication::translate("pseDialog", "Insert a time point after this one", nullptr));
#endif // QT_CONFIG(tooltip)
        pbInsertPulse->setText(QCoreApplication::translate("pseDialog", "Insert pulse", nullptr));
        gbInsertPulse->setTitle(QCoreApplication::translate("pseDialog", "Insert pulse parameters", nullptr));
        label_21->setText(QCoreApplication::translate("pseDialog", "Pulse width", nullptr));
        label_22->setText(QCoreApplication::translate("pseDialog", "Ramp up number of steps", nullptr));
        label_23->setText(QCoreApplication::translate("pseDialog", "Ramp down number of steps", nullptr));
        label_24->setText(QCoreApplication::translate("pseDialog", "Ramp step size", nullptr));
        lePulseWidth->setText(QCoreApplication::translate("pseDialog", "1000", nullptr));
        leRampUp->setText(QCoreApplication::translate("pseDialog", "10", nullptr));
        leRampDwn->setText(QCoreApplication::translate("pseDialog", "10", nullptr));
        leRampStepSize->setText(QCoreApplication::translate("pseDialog", "10", nullptr));
        label_25->setText(QCoreApplication::translate("pseDialog", "Clock cycles", nullptr));
        label_26->setText(QCoreApplication::translate("pseDialog", "Clock cycles", nullptr));
        pbInsertRamp->setText(QCoreApplication::translate("pseDialog", "Generate pulse", nullptr));
        label_27->setText(QCoreApplication::translate("pseDialog", "This function will generate the time points needed to make a voltage pulse on the DC bias channel number selected. If you are inserting this between existing time points in a sequence then there must be adequate space or an error will be reported.", nullptr));
        label_28->setText(QCoreApplication::translate("pseDialog", "DC bias channel ", nullptr));
        label_29->setText(QCoreApplication::translate("pseDialog", "Pulse voltage, note the currrect time point value will define the resting voltage", nullptr));
        lePulseChannel->setText(QCoreApplication::translate("pseDialog", "1", nullptr));
        lePulseVoltage->setText(QCoreApplication::translate("pseDialog", "100", nullptr));
        label_30->setText(QCoreApplication::translate("pseDialog", "Volts", nullptr));
        pbInsertCancel->setText(QCoreApplication::translate("pseDialog", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class pseDialog: public Ui_pseDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PSE_H
