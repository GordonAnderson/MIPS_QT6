/********************************************************************************
** Form generated from reading UI file 'controlpanel.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLPANEL_H
#define UI_CONTROLPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_ControlPanel
{
public:
    QLabel *lblBackground;

    void setupUi(QDialog *ControlPanel)
    {
        if (ControlPanel->objectName().isEmpty())
            ControlPanel->setObjectName("ControlPanel");
        ControlPanel->resize(617, 526);
        lblBackground = new QLabel(ControlPanel);
        lblBackground->setObjectName("lblBackground");
        lblBackground->setGeometry(QRect(2, 4, 611, 521));
        lblBackground->setContextMenuPolicy(Qt::CustomContextMenu);
        lblBackground->setScaledContents(true);

        retranslateUi(ControlPanel);

        QMetaObject::connectSlotsByName(ControlPanel);
    } // setupUi

    void retranslateUi(QDialog *ControlPanel)
    {
        ControlPanel->setWindowTitle(QCoreApplication::translate("ControlPanel", "Custom control panel, right click for options", nullptr));
        lblBackground->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ControlPanel: public Ui_ControlPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLPANEL_H
