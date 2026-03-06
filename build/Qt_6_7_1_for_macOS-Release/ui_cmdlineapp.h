/********************************************************************************
** Form generated from reading UI file 'cmdlineapp.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CMDLINEAPP_H
#define UI_CMDLINEAPP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_cmdlineapp
{
public:
    QPlainTextEdit *txtTerm;
    QLineEdit *leCommand;
    QLabel *label;
    QCustomPlot *plot;

    void setupUi(QDialog *cmdlineapp)
    {
        if (cmdlineapp->objectName().isEmpty())
            cmdlineapp->setObjectName("cmdlineapp");
        cmdlineapp->resize(770, 490);
        txtTerm = new QPlainTextEdit(cmdlineapp);
        txtTerm->setObjectName("txtTerm");
        txtTerm->setEnabled(true);
        txtTerm->setGeometry(QRect(10, 40, 751, 441));
        txtTerm->setReadOnly(true);
        leCommand = new QLineEdit(cmdlineapp);
        leCommand->setObjectName("leCommand");
        leCommand->setGeometry(QRect(210, 10, 551, 21));
        label = new QLabel(cmdlineapp);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 10, 181, 16));
        plot = new QCustomPlot(cmdlineapp);
        plot->setObjectName("plot");
        plot->setGeometry(QRect(10, 39, 751, 441));

        retranslateUi(cmdlineapp);

        QMetaObject::connectSlotsByName(cmdlineapp);
    } // setupUi

    void retranslateUi(QDialog *cmdlineapp)
    {
        cmdlineapp->setWindowTitle(QCoreApplication::translate("cmdlineapp", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("cmdlineapp", "Send message to app", nullptr));
    } // retranslateUi

};

namespace Ui {
    class cmdlineapp: public Ui_cmdlineapp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CMDLINEAPP_H
