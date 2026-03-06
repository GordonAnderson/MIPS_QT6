/********************************************************************************
** Form generated from reading UI file 'rfamp.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RFAMP_H
#define UI_RFAMP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RFamp
{
public:
    QGroupBox *gbRFamp;
    QTabWidget *tabRFquad;
    QWidget *tabRFsettings;
    QCheckBox *chkSRFAENA_ON_OFF;
    QLabel *label;
    QLineEdit *leGFRAVPPB;
    QLineEdit *leSRFALEV;
    QRadioButton *rbSRFAMOD_CLOSED;
    QLineEdit *leGRFAPWR;
    QLineEdit *leSRFADRV;
    QLabel *label_4;
    QLabel *label_5;
    QRadioButton *rbSRFAMOD_OPEN;
    QLabel *label_2;
    QLineEdit *leSRFAFREQ;
    QLabel *label_6;
    QLabel *label_3;
    QLineEdit *leGRFAVPPA;
    QWidget *tabMassFilter;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLineEdit *leSRFAR0;
    QLineEdit *leSRFARDC;
    QLineEdit *leSRFAPB;
    QLineEdit *leSRFARES;
    QLineEdit *leSRFAMZ;
    QPushButton *pbRFAupdate;
    QLineEdit *leSRFAK;
    QLabel *label_21;
    QCheckBox *chkSDCPWR_ON_OFF;
    QWidget *tabRFamp;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *label_19;
    QLabel *label_20;
    QLineEdit *lelRRFAAMP_1;
    QLineEdit *lelRRFAAMP_2;
    QLineEdit *lelRRFAAMP_3;
    QLineEdit *lelRRFAAMP_4;
    QLineEdit *lelRRFAAMP_5;
    QLineEdit *lelRRFAAMP_6;
    QLineEdit *lelRRFAAMP_7;
    QLineEdit *lelRRFAAMP_8;
    QLineEdit *lelRRFAAMP_9;

    void setupUi(QDialog *RFamp)
    {
        if (RFamp->objectName().isEmpty())
            RFamp->setObjectName("RFamp");
        RFamp->resize(259, 271);
        gbRFamp = new QGroupBox(RFamp);
        gbRFamp->setObjectName("gbRFamp");
        gbRFamp->setGeometry(QRect(0, 0, 261, 271));
        tabRFquad = new QTabWidget(gbRFamp);
        tabRFquad->setObjectName("tabRFquad");
        tabRFquad->setGeometry(QRect(0, 20, 256, 249));
        tabRFsettings = new QWidget();
        tabRFsettings->setObjectName("tabRFsettings");
        chkSRFAENA_ON_OFF = new QCheckBox(tabRFsettings);
        chkSRFAENA_ON_OFF->setObjectName("chkSRFAENA_ON_OFF");
        chkSRFAENA_ON_OFF->setGeometry(QRect(15, 5, 87, 20));
        label = new QLabel(tabRFsettings);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 60, 86, 16));
        leGFRAVPPB = new QLineEdit(tabRFsettings);
        leGFRAVPPB->setObjectName("leGFRAVPPB");
        leGFRAVPPB->setGeometry(QRect(125, 155, 113, 21));
        leGFRAVPPB->setReadOnly(true);
        leSRFALEV = new QLineEdit(tabRFsettings);
        leSRFALEV->setObjectName("leSRFALEV");
        leSRFALEV->setGeometry(QRect(125, 105, 113, 21));
        rbSRFAMOD_CLOSED = new QRadioButton(tabRFsettings);
        rbSRFAMOD_CLOSED->setObjectName("rbSRFAMOD_CLOSED");
        rbSRFAMOD_CLOSED->setGeometry(QRect(130, 30, 100, 20));
        leGRFAPWR = new QLineEdit(tabRFsettings);
        leGRFAPWR->setObjectName("leGRFAPWR");
        leGRFAPWR->setGeometry(QRect(125, 180, 113, 21));
        leGRFAPWR->setReadOnly(true);
        leSRFADRV = new QLineEdit(tabRFsettings);
        leSRFADRV->setObjectName("leSRFADRV");
        leSRFADRV->setGeometry(QRect(125, 80, 113, 21));
        label_4 = new QLabel(tabRFsettings);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(20, 185, 86, 16));
        label_5 = new QLabel(tabRFsettings);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(20, 85, 86, 16));
        rbSRFAMOD_OPEN = new QRadioButton(tabRFsettings);
        rbSRFAMOD_OPEN->setObjectName("rbSRFAMOD_OPEN");
        rbSRFAMOD_OPEN->setGeometry(QRect(15, 30, 100, 20));
        label_2 = new QLabel(tabRFsettings);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 160, 86, 16));
        leSRFAFREQ = new QLineEdit(tabRFsettings);
        leSRFAFREQ->setObjectName("leSRFAFREQ");
        leSRFAFREQ->setGeometry(QRect(125, 55, 113, 21));
        label_6 = new QLabel(tabRFsettings);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(20, 110, 101, 16));
        label_3 = new QLabel(tabRFsettings);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(20, 135, 86, 16));
        leGRFAVPPA = new QLineEdit(tabRFsettings);
        leGRFAVPPA->setObjectName("leGRFAVPPA");
        leGRFAVPPA->setGeometry(QRect(125, 130, 113, 21));
        leGRFAVPPA->setReadOnly(true);
        tabRFquad->addTab(tabRFsettings, QString());
        tabMassFilter = new QWidget();
        tabMassFilter->setObjectName("tabMassFilter");
        label_7 = new QLabel(tabMassFilter);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(5, 32, 60, 16));
        label_8 = new QLabel(tabMassFilter);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(5, 84, 96, 16));
        label_9 = new QLabel(tabMassFilter);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(5, 108, 86, 16));
        label_10 = new QLabel(tabMassFilter);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(5, 136, 106, 16));
        label_11 = new QLabel(tabMassFilter);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(5, 160, 60, 16));
        leSRFAR0 = new QLineEdit(tabMassFilter);
        leSRFAR0->setObjectName("leSRFAR0");
        leSRFAR0->setGeometry(QRect(122, 30, 121, 21));
        leSRFARDC = new QLineEdit(tabMassFilter);
        leSRFARDC->setObjectName("leSRFARDC");
        leSRFARDC->setGeometry(QRect(122, 82, 121, 21));
        leSRFAPB = new QLineEdit(tabMassFilter);
        leSRFAPB->setObjectName("leSRFAPB");
        leSRFAPB->setGeometry(QRect(122, 108, 121, 21));
        leSRFARES = new QLineEdit(tabMassFilter);
        leSRFARES->setObjectName("leSRFARES");
        leSRFARES->setGeometry(QRect(122, 134, 121, 21));
        leSRFAMZ = new QLineEdit(tabMassFilter);
        leSRFAMZ->setObjectName("leSRFAMZ");
        leSRFAMZ->setGeometry(QRect(122, 160, 121, 21));
        pbRFAupdate = new QPushButton(tabMassFilter);
        pbRFAupdate->setObjectName("pbRFAupdate");
        pbRFAupdate->setGeometry(QRect(15, 185, 221, 32));
        pbRFAupdate->setAutoDefault(false);
        leSRFAK = new QLineEdit(tabMassFilter);
        leSRFAK->setObjectName("leSRFAK");
        leSRFAK->setGeometry(QRect(122, 56, 121, 21));
        label_21 = new QLabel(tabMassFilter);
        label_21->setObjectName("label_21");
        label_21->setGeometry(QRect(5, 56, 109, 16));
        chkSDCPWR_ON_OFF = new QCheckBox(tabMassFilter);
        chkSDCPWR_ON_OFF->setObjectName("chkSDCPWR_ON_OFF");
        chkSDCPWR_ON_OFF->setGeometry(QRect(15, 5, 176, 20));
        tabRFquad->addTab(tabMassFilter, QString());
        tabRFamp = new QWidget();
        tabRFamp->setObjectName("tabRFamp");
        label_12 = new QLabel(tabRFamp);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(10, 8, 60, 16));
        label_13 = new QLabel(tabRFamp);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(10, 30, 60, 16));
        label_14 = new QLabel(tabRFamp);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(10, 52, 81, 16));
        label_15 = new QLabel(tabRFamp);
        label_15->setObjectName("label_15");
        label_15->setGeometry(QRect(10, 76, 60, 16));
        label_16 = new QLabel(tabRFamp);
        label_16->setObjectName("label_16");
        label_16->setGeometry(QRect(10, 100, 66, 16));
        label_17 = new QLabel(tabRFamp);
        label_17->setObjectName("label_17");
        label_17->setGeometry(QRect(10, 126, 60, 16));
        label_18 = new QLabel(tabRFamp);
        label_18->setObjectName("label_18");
        label_18->setGeometry(QRect(10, 150, 91, 16));
        label_19 = new QLabel(tabRFamp);
        label_19->setObjectName("label_19");
        label_19->setGeometry(QRect(10, 174, 86, 16));
        label_20 = new QLabel(tabRFamp);
        label_20->setObjectName("label_20");
        label_20->setGeometry(QRect(10, 196, 60, 16));
        lelRRFAAMP_1 = new QLineEdit(tabRFamp);
        lelRRFAAMP_1->setObjectName("lelRRFAAMP_1");
        lelRRFAAMP_1->setGeometry(QRect(130, 5, 113, 21));
        lelRRFAAMP_1->setReadOnly(true);
        lelRRFAAMP_2 = new QLineEdit(tabRFamp);
        lelRRFAAMP_2->setObjectName("lelRRFAAMP_2");
        lelRRFAAMP_2->setGeometry(QRect(130, 28, 113, 21));
        lelRRFAAMP_2->setReadOnly(true);
        lelRRFAAMP_3 = new QLineEdit(tabRFamp);
        lelRRFAAMP_3->setObjectName("lelRRFAAMP_3");
        lelRRFAAMP_3->setGeometry(QRect(130, 52, 113, 21));
        lelRRFAAMP_3->setReadOnly(true);
        lelRRFAAMP_4 = new QLineEdit(tabRFamp);
        lelRRFAAMP_4->setObjectName("lelRRFAAMP_4");
        lelRRFAAMP_4->setGeometry(QRect(130, 76, 113, 21));
        lelRRFAAMP_4->setReadOnly(true);
        lelRRFAAMP_5 = new QLineEdit(tabRFamp);
        lelRRFAAMP_5->setObjectName("lelRRFAAMP_5");
        lelRRFAAMP_5->setGeometry(QRect(130, 100, 113, 21));
        lelRRFAAMP_5->setReadOnly(true);
        lelRRFAAMP_6 = new QLineEdit(tabRFamp);
        lelRRFAAMP_6->setObjectName("lelRRFAAMP_6");
        lelRRFAAMP_6->setGeometry(QRect(130, 124, 113, 21));
        lelRRFAAMP_6->setReadOnly(true);
        lelRRFAAMP_7 = new QLineEdit(tabRFamp);
        lelRRFAAMP_7->setObjectName("lelRRFAAMP_7");
        lelRRFAAMP_7->setGeometry(QRect(130, 148, 113, 21));
        lelRRFAAMP_7->setReadOnly(true);
        lelRRFAAMP_8 = new QLineEdit(tabRFamp);
        lelRRFAAMP_8->setObjectName("lelRRFAAMP_8");
        lelRRFAAMP_8->setGeometry(QRect(130, 172, 113, 21));
        lelRRFAAMP_8->setReadOnly(true);
        lelRRFAAMP_9 = new QLineEdit(tabRFamp);
        lelRRFAAMP_9->setObjectName("lelRRFAAMP_9");
        lelRRFAAMP_9->setGeometry(QRect(130, 196, 113, 21));
        lelRRFAAMP_9->setReadOnly(true);
        tabRFquad->addTab(tabRFamp, QString());

        retranslateUi(RFamp);

        tabRFquad->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(RFamp);
    } // setupUi

    void retranslateUi(QDialog *RFamp)
    {
        RFamp->setWindowTitle(QCoreApplication::translate("RFamp", "Dialog", nullptr));
        gbRFamp->setTitle(QCoreApplication::translate("RFamp", "RF quad", nullptr));
        chkSRFAENA_ON_OFF->setText(QCoreApplication::translate("RFamp", "Enable", nullptr));
        label->setText(QCoreApplication::translate("RFamp", "Frequency, Hz", nullptr));
        rbSRFAMOD_CLOSED->setText(QCoreApplication::translate("RFamp", "Closed loop", nullptr));
        label_4->setText(QCoreApplication::translate("RFamp", "RF pwr, Watts", nullptr));
        label_5->setText(QCoreApplication::translate("RFamp", "Drive, %", nullptr));
        rbSRFAMOD_OPEN->setText(QCoreApplication::translate("RFamp", "Open loop", nullptr));
        label_2->setText(QCoreApplication::translate("RFamp", "RF- Vp-p", nullptr));
        label_6->setText(QCoreApplication::translate("RFamp", "Setpoint, Vp-p", nullptr));
        label_3->setText(QCoreApplication::translate("RFamp", "RF+ Vp-p", nullptr));
        tabRFquad->setTabText(tabRFquad->indexOf(tabRFsettings), QCoreApplication::translate("RFamp", "RF settings", nullptr));
        label_7->setText(QCoreApplication::translate("RFamp", "Ro, mm", nullptr));
        label_8->setText(QCoreApplication::translate("RFamp", "Resolving DC, V", nullptr));
        label_9->setText(QCoreApplication::translate("RFamp", "Pole Bias, V", nullptr));
        label_10->setText(QCoreApplication::translate("RFamp", "Resolution, AMU", nullptr));
        label_11->setText(QCoreApplication::translate("RFamp", "m/z", nullptr));
        pbRFAupdate->setText(QCoreApplication::translate("RFamp", "Update", nullptr));
        label_21->setText(QCoreApplication::translate("RFamp", "RDC=RFpp/k, k=", nullptr));
        chkSDCPWR_ON_OFF->setText(QCoreApplication::translate("RFamp", "DC power supply enable", nullptr));
        tabRFquad->setTabText(tabRFquad->indexOf(tabMassFilter), QCoreApplication::translate("RFamp", "Mass filter", nullptr));
        label_12->setText(QCoreApplication::translate("RFamp", "DC V", nullptr));
        label_13->setText(QCoreApplication::translate("RFamp", "DC I in", nullptr));
        label_14->setText(QCoreApplication::translate("RFamp", "DR pwr in,W", nullptr));
        label_15->setText(QCoreApplication::translate("RFamp", "Temp, C", nullptr));
        label_16->setText(QCoreApplication::translate("RFamp", "RF Vrms", nullptr));
        label_17->setText(QCoreApplication::translate("RFamp", "RF Irms", nullptr));
        label_18->setText(QCoreApplication::translate("RFamp", "RF fwd pwr,W", nullptr));
        label_19->setText(QCoreApplication::translate("RFamp", "RF rev pwr,W", nullptr));
        label_20->setText(QCoreApplication::translate("RFamp", "SWR", nullptr));
        tabRFquad->setTabText(tabRFquad->indexOf(tabRFamp), QCoreApplication::translate("RFamp", "RF amp", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RFamp: public Ui_RFamp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RFAMP_H
