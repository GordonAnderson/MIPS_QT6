/********************************************************************************
** Form generated from reading UI file 'scriptingconsole.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCRIPTINGCONSOLE_H
#define UI_SCRIPTINGCONSOLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_ScriptingConsole
{
public:
    QPushButton *pbEvaluate;
    QTextEdit *txtScript;
    QLabel *lblStatus;
    QPushButton *pbSave;
    QPushButton *pbLoad;
    QPushButton *pbAbort;
    QPushButton *pbHelp;

    void setupUi(QDialog *ScriptingConsole)
    {
        if (ScriptingConsole->objectName().isEmpty())
            ScriptingConsole->setObjectName("ScriptingConsole");
        ScriptingConsole->resize(501, 366);
        pbEvaluate = new QPushButton(ScriptingConsole);
        pbEvaluate->setObjectName("pbEvaluate");
        pbEvaluate->setGeometry(QRect(0, 220, 501, 32));
        pbEvaluate->setAutoDefault(false);
        txtScript = new QTextEdit(ScriptingConsole);
        txtScript->setObjectName("txtScript");
        txtScript->setGeometry(QRect(5, 5, 491, 216));
        lblStatus = new QLabel(ScriptingConsole);
        lblStatus->setObjectName("lblStatus");
        lblStatus->setGeometry(QRect(10, 255, 481, 51));
        lblStatus->setWordWrap(true);
        pbSave = new QPushButton(ScriptingConsole);
        pbSave->setObjectName("pbSave");
        pbSave->setGeometry(QRect(380, 330, 113, 32));
        pbSave->setAutoDefault(false);
        pbLoad = new QPushButton(ScriptingConsole);
        pbLoad->setObjectName("pbLoad");
        pbLoad->setGeometry(QRect(0, 330, 113, 32));
        pbLoad->setAutoDefault(false);
        pbAbort = new QPushButton(ScriptingConsole);
        pbAbort->setObjectName("pbAbort");
        pbAbort->setGeometry(QRect(250, 330, 113, 32));
        pbAbort->setAutoDefault(false);
        pbHelp = new QPushButton(ScriptingConsole);
        pbHelp->setObjectName("pbHelp");
        pbHelp->setGeometry(QRect(120, 330, 113, 32));
        pbHelp->setAutoDefault(false);

        retranslateUi(ScriptingConsole);

        QMetaObject::connectSlotsByName(ScriptingConsole);
    } // setupUi

    void retranslateUi(QDialog *ScriptingConsole)
    {
        ScriptingConsole->setWindowTitle(QCoreApplication::translate("ScriptingConsole", "Scripting", nullptr));
        pbEvaluate->setText(QCoreApplication::translate("ScriptingConsole", "Execute", nullptr));
        lblStatus->setText(QString());
        pbSave->setText(QCoreApplication::translate("ScriptingConsole", "Save", nullptr));
        pbLoad->setText(QCoreApplication::translate("ScriptingConsole", "Load", nullptr));
        pbAbort->setText(QCoreApplication::translate("ScriptingConsole", "Abort", nullptr));
        pbHelp->setText(QCoreApplication::translate("ScriptingConsole", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ScriptingConsole: public Ui_ScriptingConsole {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCRIPTINGCONSOLE_H
