/********************************************************************************
** Form generated from reading UI file 'plot.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOT_H
#define UI_PLOT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_Plot
{
public:
    QCustomPlot *Graph;
    QPlainTextEdit *txtComments;
    QPushButton *pbCloseComments;
    QCustomPlot *HeatMap1;
    QCustomPlot *HeatMap2;

    void setupUi(QDialog *Plot)
    {
        if (Plot->objectName().isEmpty())
            Plot->setObjectName("Plot");
        Plot->resize(667, 470);
        Graph = new QCustomPlot(Plot);
        Graph->setObjectName("Graph");
        Graph->setGeometry(QRect(15, 15, 616, 441));
        txtComments = new QPlainTextEdit(Plot);
        txtComments->setObjectName("txtComments");
        txtComments->setGeometry(QRect(15, 15, 471, 431));
        pbCloseComments = new QPushButton(Plot);
        pbCloseComments->setObjectName("pbCloseComments");
        pbCloseComments->setGeometry(QRect(500, 400, 136, 32));
        pbCloseComments->setAutoDefault(false);
        HeatMap1 = new QCustomPlot(Plot);
        HeatMap1->setObjectName("HeatMap1");
        HeatMap1->setGeometry(QRect(0, 0, 326, 461));
        HeatMap2 = new QCustomPlot(Plot);
        HeatMap2->setObjectName("HeatMap2");
        HeatMap2->setGeometry(QRect(375, 0, 326, 461));

        retranslateUi(Plot);

        QMetaObject::connectSlotsByName(Plot);
    } // setupUi

    void retranslateUi(QDialog *Plot)
    {
        Plot->setWindowTitle(QCoreApplication::translate("Plot", "Dialog", nullptr));
        pbCloseComments->setText(QCoreApplication::translate("Plot", "Close comments", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Plot: public Ui_Plot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOT_H
