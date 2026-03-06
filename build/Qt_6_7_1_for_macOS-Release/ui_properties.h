/********************************************************************************
** Form generated from reading UI file 'properties.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROPERTIES_H
#define UI_PROPERTIES_H

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

class Ui_Properties
{
public:
    QPushButton *pbOK;
    QGroupBox *groupBox;
    QLabel *label_2;
    QLineEdit *leMinMIPS;
    QPushButton *pbLoadControlPanel;
    QLineEdit *leControlPanel;
    QLabel *label;
    QComboBox *comboTCPIPlist;
    QCheckBox *chkAutoConnect;
    QPushButton *pbClear;
    QCheckBox *chkSearchAMPS;
    QLineEdit *leAMPSbaud;
    QCheckBox *chkAutoFileName;
    QGroupBox *groupBox_2;
    QPushButton *pbDataFilePath;
    QPushButton *pbScriptPath;
    QLineEdit *leDataFilePath;
    QLineEdit *leScriptPath;
    QPushButton *pbMethodesPath;
    QLineEdit *leMethodesPath;
    QLineEdit *leFileName;
    QLabel *label_3;
    QPushButton *pbLogFile;
    QLineEdit *leLogFile;
    QLabel *label_4;
    QLineEdit *leUpdateSecs;
    QCheckBox *chkAutoRestore;
    QLabel *label_5;
    QLineEdit *leSysFontSize;

    void setupUi(QDialog *Properties)
    {
        if (Properties->objectName().isEmpty())
            Properties->setObjectName("Properties");
        Properties->resize(538, 457);
        pbOK = new QPushButton(Properties);
        pbOK->setObjectName("pbOK");
        pbOK->setGeometry(QRect(410, 410, 113, 32));
        pbOK->setAutoDefault(false);
        groupBox = new QGroupBox(Properties);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(10, 0, 516, 146));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(20, 115, 141, 16));
        leMinMIPS = new QLineEdit(groupBox);
        leMinMIPS->setObjectName("leMinMIPS");
        leMinMIPS->setGeometry(QRect(235, 85, 36, 21));
        pbLoadControlPanel = new QPushButton(groupBox);
        pbLoadControlPanel->setObjectName("pbLoadControlPanel");
        pbLoadControlPanel->setGeometry(QRect(10, 55, 151, 32));
        pbLoadControlPanel->setAutoDefault(false);
        leControlPanel = new QLineEdit(groupBox);
        leControlPanel->setObjectName("leControlPanel");
        leControlPanel->setGeometry(QRect(165, 60, 346, 21));
        label = new QLabel(groupBox);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 90, 216, 16));
        comboTCPIPlist = new QComboBox(groupBox);
        comboTCPIPlist->setObjectName("comboTCPIPlist");
        comboTCPIPlist->setGeometry(QRect(155, 110, 176, 26));
        comboTCPIPlist->setEditable(true);
        chkAutoConnect = new QCheckBox(groupBox);
        chkAutoConnect->setObjectName("chkAutoConnect");
        chkAutoConnect->setGeometry(QRect(15, 30, 171, 20));
        pbClear = new QPushButton(groupBox);
        pbClear->setObjectName("pbClear");
        pbClear->setGeometry(QRect(340, 107, 116, 32));
        pbClear->setAutoDefault(false);
        chkSearchAMPS = new QCheckBox(groupBox);
        chkSearchAMPS->setObjectName("chkSearchAMPS");
        chkSearchAMPS->setGeometry(QRect(195, 30, 226, 20));
        leAMPSbaud = new QLineEdit(groupBox);
        leAMPSbaud->setObjectName("leAMPSbaud");
        leAMPSbaud->setGeometry(QRect(425, 30, 86, 21));
        chkAutoFileName = new QCheckBox(Properties);
        chkAutoFileName->setObjectName("chkAutoFileName");
        chkAutoFileName->setGeometry(QRect(10, 155, 251, 20));
        groupBox_2 = new QGroupBox(Properties);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(10, 180, 516, 151));
        pbDataFilePath = new QPushButton(groupBox_2);
        pbDataFilePath->setObjectName("pbDataFilePath");
        pbDataFilePath->setGeometry(QRect(5, 15, 116, 32));
        pbDataFilePath->setAutoDefault(false);
        pbScriptPath = new QPushButton(groupBox_2);
        pbScriptPath->setObjectName("pbScriptPath");
        pbScriptPath->setGeometry(QRect(5, 75, 116, 32));
        pbScriptPath->setAutoDefault(false);
        leDataFilePath = new QLineEdit(groupBox_2);
        leDataFilePath->setObjectName("leDataFilePath");
        leDataFilePath->setGeometry(QRect(125, 20, 381, 21));
        leScriptPath = new QLineEdit(groupBox_2);
        leScriptPath->setObjectName("leScriptPath");
        leScriptPath->setGeometry(QRect(125, 80, 381, 21));
        pbMethodesPath = new QPushButton(groupBox_2);
        pbMethodesPath->setObjectName("pbMethodesPath");
        pbMethodesPath->setGeometry(QRect(5, 110, 116, 32));
        pbMethodesPath->setAutoDefault(false);
        leMethodesPath = new QLineEdit(groupBox_2);
        leMethodesPath->setObjectName("leMethodesPath");
        leMethodesPath->setGeometry(QRect(125, 115, 381, 21));
        leFileName = new QLineEdit(groupBox_2);
        leFileName->setObjectName("leFileName");
        leFileName->setGeometry(QRect(125, 50, 126, 21));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 57, 141, 16));
        pbLogFile = new QPushButton(Properties);
        pbLogFile->setObjectName("pbLogFile");
        pbLogFile->setGeometry(QRect(15, 340, 116, 32));
        pbLogFile->setAutoDefault(false);
        leLogFile = new QLineEdit(Properties);
        leLogFile->setObjectName("leLogFile");
        leLogFile->setGeometry(QRect(135, 345, 381, 21));
        label_4 = new QLabel(Properties);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(20, 390, 171, 16));
        leUpdateSecs = new QLineEdit(Properties);
        leUpdateSecs->setObjectName("leUpdateSecs");
        leUpdateSecs->setGeometry(QRect(205, 390, 66, 21));
        chkAutoRestore = new QCheckBox(Properties);
        chkAutoRestore->setObjectName("chkAutoRestore");
        chkAutoRestore->setGeometry(QRect(285, 155, 236, 20));
        label_5 = new QLabel(Properties);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(20, 420, 171, 16));
        leSysFontSize = new QLineEdit(Properties);
        leSysFontSize->setObjectName("leSysFontSize");
        leSysFontSize->setGeometry(QRect(205, 420, 66, 21));

        retranslateUi(Properties);

        QMetaObject::connectSlotsByName(Properties);
    } // setupUi

    void retranslateUi(QDialog *Properties)
    {
        Properties->setWindowTitle(QCoreApplication::translate("Properties", "MIPS Properties", nullptr));
        pbOK->setText(QCoreApplication::translate("Properties", "OK", nullptr));
        groupBox->setTitle(QCoreApplication::translate("Properties", "Startup options", nullptr));
        label_2->setText(QCoreApplication::translate("Properties", "MIPS TCP/IP list ", nullptr));
        leMinMIPS->setText(QCoreApplication::translate("Properties", "0", nullptr));
        pbLoadControlPanel->setText(QCoreApplication::translate("Properties", "Load control panel", nullptr));
        leControlPanel->setText(QString());
        label->setText(QCoreApplication::translate("Properties", "Minimum number of MIPS systems", nullptr));
        chkAutoConnect->setText(QCoreApplication::translate("Properties", "Auto connect at startup", nullptr));
        pbClear->setText(QCoreApplication::translate("Properties", "Clear list", nullptr));
        chkSearchAMPS->setText(QCoreApplication::translate("Properties", "Search for AMPS, AMS baud rate", nullptr));
        leAMPSbaud->setText(QCoreApplication::translate("Properties", "38400", nullptr));
        chkAutoFileName->setText(QCoreApplication::translate("Properties", "Automatic unique file name selection", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("Properties", "Directories", nullptr));
        pbDataFilePath->setText(QCoreApplication::translate("Properties", "Data file path", nullptr));
        pbScriptPath->setText(QCoreApplication::translate("Properties", "Script path", nullptr));
        leDataFilePath->setText(QString());
        leScriptPath->setText(QString());
        pbMethodesPath->setText(QCoreApplication::translate("Properties", "Methodes path", nullptr));
        leMethodesPath->setText(QString());
        leFileName->setText(QCoreApplication::translate("Properties", "Test.0000", nullptr));
        label_3->setText(QCoreApplication::translate("Properties", "Base name", nullptr));
        pbLogFile->setText(QCoreApplication::translate("Properties", "Log file", nullptr));
        leLogFile->setText(QString());
        label_4->setText(QCoreApplication::translate("Properties", "Seconds between updates", nullptr));
        leUpdateSecs->setText(QCoreApplication::translate("Properties", "1", nullptr));
        chkAutoRestore->setText(QCoreApplication::translate("Properties", "Automatically restore connections", nullptr));
        label_5->setText(QCoreApplication::translate("Properties", "System font size", nullptr));
#if QT_CONFIG(tooltip)
        leSysFontSize->setToolTip(QCoreApplication::translate("Properties", "This will change the system font size but you will have to stop and restart the application to apply changes to the main MIPS dilog", nullptr));
#endif // QT_CONFIG(tooltip)
        leSysFontSize->setText(QCoreApplication::translate("Properties", "10", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Properties: public Ui_Properties {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROPERTIES_H
