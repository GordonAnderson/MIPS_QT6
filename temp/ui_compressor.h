/********************************************************************************
** Form generated from reading UI file 'compressor.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPRESSOR_H
#define UI_COMPRESSOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_Compressor
{
public:
    QFrame *frmCompressor;
    QLabel *label_133;
    QLineEdit *leSARBCORDER;
    QGroupBox *gbARBmode;
    QRadioButton *rbSARBCMODE_Normal;
    QRadioButton *rbSARBCMODE_Compress;
    QGroupBox *gbARBswitch;
    QRadioButton *rbSARBCSW_Close;
    QRadioButton *rbSARBCSW_Open;
    QGroupBox *gbARBtiming;
    QLabel *label_142;
    QLabel *label_143;
    QLabel *label_144;
    QLabel *label_145;
    QLineEdit *leSARBCTD;
    QLabel *label_146;
    QLineEdit *leSARBCTC;
    QLineEdit *leSARBCTN;
    QLineEdit *leSARBCTNC;
    QLabel *label_147;
    QLabel *label_148;
    QLabel *label_149;
    QLineEdit *leSARBCTBL;
    QPushButton *pbARBforceTrigger;
    QLabel *label_150;

    void setupUi(QDialog *Compressor)
    {
        if (Compressor->objectName().isEmpty())
            Compressor->setObjectName("Compressor");
        Compressor->resize(490, 249);
        frmCompressor = new QFrame(Compressor);
        frmCompressor->setObjectName("frmCompressor");
        frmCompressor->setGeometry(QRect(0, 0, 491, 251));
        frmCompressor->setFrameShape(QFrame::StyledPanel);
        frmCompressor->setFrameShadow(QFrame::Plain);
        frmCompressor->setLineWidth(0);
        label_133 = new QLabel(frmCompressor);
        label_133->setObjectName("label_133");
        label_133->setGeometry(QRect(5, 175, 59, 16));
        leSARBCORDER = new QLineEdit(frmCompressor);
        leSARBCORDER->setObjectName("leSARBCORDER");
        leSARBCORDER->setGeometry(QRect(55, 175, 113, 21));
        gbARBmode = new QGroupBox(frmCompressor);
        gbARBmode->setObjectName("gbARBmode");
        gbARBmode->setGeometry(QRect(5, 5, 161, 81));
        rbSARBCMODE_Normal = new QRadioButton(gbARBmode);
        rbSARBCMODE_Normal->setObjectName("rbSARBCMODE_Normal");
        rbSARBCMODE_Normal->setGeometry(QRect(15, 30, 100, 20));
        rbSARBCMODE_Normal->setChecked(true);
        rbSARBCMODE_Compress = new QRadioButton(gbARBmode);
        rbSARBCMODE_Compress->setObjectName("rbSARBCMODE_Compress");
        rbSARBCMODE_Compress->setGeometry(QRect(15, 50, 100, 20));
        gbARBswitch = new QGroupBox(frmCompressor);
        gbARBswitch->setObjectName("gbARBswitch");
        gbARBswitch->setGeometry(QRect(5, 85, 161, 81));
        rbSARBCSW_Close = new QRadioButton(gbARBswitch);
        rbSARBCSW_Close->setObjectName("rbSARBCSW_Close");
        rbSARBCSW_Close->setGeometry(QRect(15, 30, 100, 20));
        rbSARBCSW_Close->setChecked(true);
        rbSARBCSW_Open = new QRadioButton(gbARBswitch);
        rbSARBCSW_Open->setObjectName("rbSARBCSW_Open");
        rbSARBCSW_Open->setGeometry(QRect(15, 50, 100, 20));
        gbARBtiming = new QGroupBox(frmCompressor);
        gbARBtiming->setObjectName("gbARBtiming");
        gbARBtiming->setGeometry(QRect(185, 15, 301, 131));
        label_142 = new QLabel(gbARBtiming);
        label_142->setObjectName("label_142");
        label_142->setGeometry(QRect(10, 25, 96, 16));
        label_143 = new QLabel(gbARBtiming);
        label_143->setObjectName("label_143");
        label_143->setGeometry(QRect(10, 50, 96, 16));
        label_144 = new QLabel(gbARBtiming);
        label_144->setObjectName("label_144");
        label_144->setGeometry(QRect(10, 75, 96, 16));
        label_145 = new QLabel(gbARBtiming);
        label_145->setObjectName("label_145");
        label_145->setGeometry(QRect(10, 100, 121, 16));
        leSARBCTD = new QLineEdit(gbARBtiming);
        leSARBCTD->setObjectName("leSARBCTD");
        leSARBCTD->setGeometry(QRect(140, 25, 113, 21));
        label_146 = new QLabel(gbARBtiming);
        label_146->setObjectName("label_146");
        label_146->setGeometry(QRect(260, 25, 26, 16));
        leSARBCTC = new QLineEdit(gbARBtiming);
        leSARBCTC->setObjectName("leSARBCTC");
        leSARBCTC->setGeometry(QRect(140, 50, 113, 21));
        leSARBCTN = new QLineEdit(gbARBtiming);
        leSARBCTN->setObjectName("leSARBCTN");
        leSARBCTN->setGeometry(QRect(140, 75, 113, 21));
        leSARBCTNC = new QLineEdit(gbARBtiming);
        leSARBCTNC->setObjectName("leSARBCTNC");
        leSARBCTNC->setGeometry(QRect(140, 100, 113, 21));
        label_147 = new QLabel(gbARBtiming);
        label_147->setObjectName("label_147");
        label_147->setGeometry(QRect(260, 50, 26, 16));
        label_148 = new QLabel(gbARBtiming);
        label_148->setObjectName("label_148");
        label_148->setGeometry(QRect(260, 75, 26, 16));
        label_149 = new QLabel(gbARBtiming);
        label_149->setObjectName("label_149");
        label_149->setGeometry(QRect(260, 100, 26, 16));
        leSARBCTBL = new QLineEdit(frmCompressor);
        leSARBCTBL->setObjectName("leSARBCTBL");
        leSARBCTBL->setGeometry(QRect(10, 225, 476, 21));
        pbARBforceTrigger = new QPushButton(frmCompressor);
        pbARBforceTrigger->setObjectName("pbARBforceTrigger");
        pbARBforceTrigger->setGeometry(QRect(175, 170, 311, 32));
        pbARBforceTrigger->setAutoDefault(false);
        label_150 = new QLabel(frmCompressor);
        label_150->setObjectName("label_150");
        label_150->setGeometry(QRect(10, 205, 191, 16));

        retranslateUi(Compressor);

        QMetaObject::connectSlotsByName(Compressor);
    } // setupUi

    void retranslateUi(QDialog *Compressor)
    {
        Compressor->setWindowTitle(QCoreApplication::translate("Compressor", "Dialog", nullptr));
        label_133->setText(QCoreApplication::translate("Compressor", "Order", nullptr));
        leSARBCORDER->setText(QCoreApplication::translate("Compressor", "2", nullptr));
        gbARBmode->setTitle(QCoreApplication::translate("Compressor", "Mode", nullptr));
        rbSARBCMODE_Normal->setText(QCoreApplication::translate("Compressor", "Normal", nullptr));
        rbSARBCMODE_Compress->setText(QCoreApplication::translate("Compressor", "Compress", nullptr));
        gbARBswitch->setTitle(QCoreApplication::translate("Compressor", "Switch", nullptr));
        rbSARBCSW_Close->setText(QCoreApplication::translate("Compressor", "Close", nullptr));
        rbSARBCSW_Open->setText(QCoreApplication::translate("Compressor", "Open", nullptr));
        gbARBtiming->setTitle(QCoreApplication::translate("Compressor", "Multi-pass cycle times", nullptr));
        label_142->setText(QCoreApplication::translate("Compressor", "Trigger delay", nullptr));
        label_143->setText(QCoreApplication::translate("Compressor", "Compress time", nullptr));
        label_144->setText(QCoreApplication::translate("Compressor", "Normal time", nullptr));
        label_145->setText(QCoreApplication::translate("Compressor", "Non compress time", nullptr));
        leSARBCTD->setText(QCoreApplication::translate("Compressor", "1.0", nullptr));
        label_146->setText(QCoreApplication::translate("Compressor", "mS", nullptr));
        leSARBCTC->setText(QCoreApplication::translate("Compressor", "1.0", nullptr));
        leSARBCTN->setText(QCoreApplication::translate("Compressor", "1.0", nullptr));
        leSARBCTNC->setText(QCoreApplication::translate("Compressor", "1.0", nullptr));
        label_147->setText(QCoreApplication::translate("Compressor", "mS", nullptr));
        label_148->setText(QCoreApplication::translate("Compressor", "mS", nullptr));
        label_149->setText(QCoreApplication::translate("Compressor", "mS", nullptr));
        leSARBCTBL->setText(QCoreApplication::translate("Compressor", "C", nullptr));
        pbARBforceTrigger->setText(QCoreApplication::translate("Compressor", "Force Multi-pass trigger", nullptr));
        label_150->setText(QCoreApplication::translate("Compressor", "Multi-pass compressor table", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Compressor: public Ui_Compressor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPRESSOR_H
