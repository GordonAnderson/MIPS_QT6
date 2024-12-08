/********************************************************************************
** Form generated from reading UI file 'mipscomms.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MIPSCOMMS_H
#define UI_MIPSCOMMS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>

QT_BEGIN_NAMESPACE

class Ui_MIPScomms
{
public:
    QComboBox *comboTermMIPS;
    QLabel *label;
    QComboBox *comboTermCMD;
    QPlainTextEdit *txtTerm;
    QLabel *label_2;

    void setupUi(QDialog *MIPScomms)
    {
        if (MIPScomms->objectName().isEmpty())
            MIPScomms->setObjectName("MIPScomms");
        MIPScomms->resize(776, 544);
        comboTermMIPS = new QComboBox(MIPScomms);
        comboTermMIPS->setObjectName("comboTermMIPS");
        comboTermMIPS->setGeometry(QRect(20, 30, 146, 26));
        label = new QLabel(MIPScomms);
        label->setObjectName("label");
        label->setGeometry(QRect(25, 10, 126, 16));
        comboTermCMD = new QComboBox(MIPScomms);
        comboTermCMD->setObjectName("comboTermCMD");
        comboTermCMD->setGeometry(QRect(215, 30, 531, 26));
        comboTermCMD->setEditable(true);
        comboTermCMD->setInsertPolicy(QComboBox::InsertAtTop);
        comboTermCMD->setDuplicatesEnabled(true);
        txtTerm = new QPlainTextEdit(MIPScomms);
        txtTerm->setObjectName("txtTerm");
        txtTerm->setGeometry(QRect(14, 60, 751, 471));
        label_2 = new QLabel(MIPScomms);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(220, 10, 256, 16));

        retranslateUi(MIPScomms);

        QMetaObject::connectSlotsByName(MIPScomms);
    } // setupUi

    void retranslateUi(QDialog *MIPScomms)
    {
        MIPScomms->setWindowTitle(QCoreApplication::translate("MIPScomms", "MIPS communications", nullptr));
        label->setText(QCoreApplication::translate("MIPScomms", "Select MIPS system", nullptr));
        label_2->setText(QCoreApplication::translate("MIPScomms", "Enter command to send to selected MIPS", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MIPScomms: public Ui_MIPScomms {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MIPSCOMMS_H
