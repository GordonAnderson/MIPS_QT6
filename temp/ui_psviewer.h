/********************************************************************************
** Form generated from reading UI file 'psviewer.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PSVIEWER_H
#define UI_PSVIEWER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_psviewer
{
public:
    QDialogButtonBox *buttonBox;
    QCustomPlot *PSV;
    QCheckBox *chkZoomY;
    QCheckBox *chkZoomX;
    QComboBox *comboPlotItem;
    QLabel *label;

    void setupUi(QDialog *psviewer)
    {
        if (psviewer->objectName().isEmpty())
            psviewer->setObjectName("psviewer");
        psviewer->resize(669, 521);
        buttonBox = new QDialogButtonBox(psviewer);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(550, 470, 81, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);
        PSV = new QCustomPlot(psviewer);
        PSV->setObjectName("PSV");
        PSV->setGeometry(QRect(20, 20, 631, 421));
        chkZoomY = new QCheckBox(psviewer);
        chkZoomY->setObjectName("chkZoomY");
        chkZoomY->setGeometry(QRect(324, 460, 161, 20));
        chkZoomX = new QCheckBox(psviewer);
        chkZoomX->setObjectName("chkZoomX");
        chkZoomX->setGeometry(QRect(324, 485, 161, 20));
        comboPlotItem = new QComboBox(psviewer);
        comboPlotItem->setObjectName("comboPlotItem");
        comboPlotItem->setGeometry(QRect(160, 470, 104, 26));
        label = new QLabel(psviewer);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 473, 131, 16));

        retranslateUi(psviewer);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, psviewer, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, psviewer, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(psviewer);
    } // setupUi

    void retranslateUi(QDialog *psviewer)
    {
        psviewer->setWindowTitle(QCoreApplication::translate("psviewer", "Pusle Sequence Viewer", nullptr));
        chkZoomY->setText(QCoreApplication::translate("psviewer", "Zoom and drag Y axis", nullptr));
        chkZoomX->setText(QCoreApplication::translate("psviewer", "Zoom and drag X axis", nullptr));
        label->setText(QCoreApplication::translate("psviewer", "Select signal to plot", nullptr));
    } // retranslateUi

};

namespace Ui {
    class psviewer: public Ui_psviewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PSVIEWER_H
