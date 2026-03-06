/********************************************************************************
** Form generated from reading UI file 'script.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCRIPT_H
#define UI_SCRIPT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Script
{
public:
    QGroupBox *gbScript;
    QPushButton *pbLoadScript;
    QLabel *label_2;
    QLineEdit *leScriptFile;
    QLabel *label_36;
    QPlainTextEdit *pteScriptStatus;
    QPushButton *pbAbort;

    void setupUi(QDialog *Script)
    {
        if (Script->objectName().isEmpty())
            Script->setObjectName("Script");
        Script->resize(306, 181);
        gbScript = new QGroupBox(Script);
        gbScript->setObjectName("gbScript");
        gbScript->setGeometry(QRect(0, 0, 306, 181));
        pbLoadScript = new QPushButton(gbScript);
        pbLoadScript->setObjectName("pbLoadScript");
        pbLoadScript->setGeometry(QRect(50, 15, 211, 32));
        pbLoadScript->setAutoDefault(false);
        label_2 = new QLabel(gbScript);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 55, 59, 16));
        leScriptFile = new QLineEdit(gbScript);
        leScriptFile->setObjectName("leScriptFile");
        leScriptFile->setGeometry(QRect(85, 50, 206, 21));
        leScriptFile->setReadOnly(true);
        label_36 = new QLabel(gbScript);
        label_36->setObjectName("label_36");
        label_36->setGeometry(QRect(10, 80, 59, 16));
        pteScriptStatus = new QPlainTextEdit(gbScript);
        pteScriptStatus->setObjectName("pteScriptStatus");
        pteScriptStatus->setGeometry(QRect(85, 80, 206, 91));
        pteScriptStatus->setReadOnly(true);
        pbAbort = new QPushButton(gbScript);
        pbAbort->setObjectName("pbAbort");
        pbAbort->setEnabled(false);
        pbAbort->setGeometry(QRect(5, 100, 76, 71));
        pbAbort->setAutoDefault(false);

        retranslateUi(Script);

        QMetaObject::connectSlotsByName(Script);
    } // setupUi

    void retranslateUi(QDialog *Script)
    {
        Script->setWindowTitle(QCoreApplication::translate("Script", "Dialog", nullptr));
        gbScript->setTitle(QCoreApplication::translate("Script", "Script", nullptr));
        pbLoadScript->setText(QCoreApplication::translate("Script", "Select script file and play", nullptr));
        label_2->setText(QCoreApplication::translate("Script", "Filename", nullptr));
        label_36->setText(QCoreApplication::translate("Script", "Status", nullptr));
        pbAbort->setText(QCoreApplication::translate("Script", "Abort", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Script: public Ui_Script {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCRIPT_H
