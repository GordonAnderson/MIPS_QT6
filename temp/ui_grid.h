/********************************************************************************
** Form generated from reading UI file 'grid.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRID_H
#define UI_GRID_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_Grid
{
public:
    QTabWidget *tabGrid;
    QWidget *tab;
    QFrame *frmGRID;
    QLineEdit *leSHV_1;
    QFrame *line_5;
    QFrame *line_8;
    QLabel *label_2;
    QFrame *line_2;
    QFrame *line_3;
    QFrame *line_6;
    QFrame *line_7;
    QFrame *line_10;
    QFrame *line_4;
    QFrame *line_9;
    QFrame *line;
    QLabel *label;
    QLineEdit *leGHVV_1;
    QLineEdit *leSHV_2;
    QLineEdit *leGHVV_2;
    QCheckBox *chkGRID1enable;
    QCheckBox *chkGRID2enable;
    QLabel *label_3;
    QLabel *label_4;
    QGroupBox *gbGrid1RF;
    QLabel *label_5;
    QLineEdit *leSRFDRV_1;
    QLabel *label_6;
    QLabel *label_7;
    QLineEdit *leSRFFRQ_1;
    QLineEdit *leGRFPPVP_1;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QPushButton *pbTuneG1;
    QPushButton *pbRetuneG1;
    QLineEdit *leGRFPWR_1;
    QLabel *label_71;
    QLabel *label_72;
    QLineEdit *leSRFVLT_1;
    QLabel *label_14;
    QLabel *label_15;
    QRadioButton *rbModeManual;
    QRadioButton *rbModeAuto;
    QPushButton *pbShutdown;
    QLabel *label_8;
    QCustomPlot *Vplot;
    QLineEdit *leYmax;
    QLineEdit *leYmin;
    QPushButton *pbResetPlot;
    QLabel *label_12;
    QLabel *label_13;
    QCheckBox *chkYauto;
    QCheckBox *chkEnablePlot;
    QPushButton *pbPaste_2;
    QWidget *tab_2;
    QFrame *frmPulseSequence;
    QLabel *label_16;
    QLabel *label_17;
    QLineEdit *leTp;
    QLabel *label_18;
    QLineEdit *leTs1;
    QLabel *label_19;
    QLineEdit *leTs2;
    QLabel *label_20;
    QLineEdit *leN;
    QGroupBox *gbSequenceRepeats;
    QComboBox *comboSelectN;
    QLabel *label_21;
    QLabel *label_22;
    QLineEdit *leTdn;
    QLabel *label_23;
    QLineEdit *leM;
    QPushButton *pbPaste;
    QPushButton *pbDownload;
    QPushButton *pbStart;
    QLabel *label_24;
    QLineEdit *leFullRepeat;
    QPushButton *pbShow;
    QLabel *label_25;
    QLabel *label_26;
    QLabel *label_27;
    QLabel *label_28;
    QLabel *label_29;
    QLabel *label_30;
    QCheckBox *chkTs1eTs2;
    QCheckBox *chkTs2eTs1;
    QLabel *label_31;
    QCheckBox *chkSwapAD;

    void setupUi(QDialog *Grid)
    {
        if (Grid->objectName().isEmpty())
            Grid->setObjectName("Grid");
        Grid->resize(955, 434);
        tabGrid = new QTabWidget(Grid);
        tabGrid->setObjectName("tabGrid");
        tabGrid->setGeometry(QRect(0, 0, 951, 431));
        tab = new QWidget();
        tab->setObjectName("tab");
        frmGRID = new QFrame(tab);
        frmGRID->setObjectName("frmGRID");
        frmGRID->setGeometry(QRect(0, 0, 951, 411));
        frmGRID->setFrameShape(QFrame::NoFrame);
        frmGRID->setFrameShadow(QFrame::Plain);
        frmGRID->setLineWidth(0);
        leSHV_1 = new QLineEdit(frmGRID);
        leSHV_1->setObjectName("leSHV_1");
        leSHV_1->setGeometry(QRect(110, 100, 61, 21));
        line_5 = new QFrame(frmGRID);
        line_5->setObjectName("line_5");
        line_5->setGeometry(QRect(230, 215, 16, 16));
        line_5->setFrameShadow(QFrame::Plain);
        line_5->setLineWidth(3);
        line_5->setFrameShape(QFrame::Shape::VLine);
        line_8 = new QFrame(frmGRID);
        line_8->setObjectName("line_8");
        line_8->setGeometry(QRect(245, 195, 16, 16));
        line_8->setFrameShadow(QFrame::Plain);
        line_8->setLineWidth(3);
        line_8->setFrameShape(QFrame::Shape::VLine);
        label_2 = new QLabel(frmGRID);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(260, 135, 36, 16));
        line_2 = new QFrame(frmGRID);
        line_2->setObjectName("line_2");
        line_2->setGeometry(QRect(230, 155, 16, 16));
        line_2->setFrameShadow(QFrame::Plain);
        line_2->setLineWidth(3);
        line_2->setFrameShape(QFrame::Shape::VLine);
        line_3 = new QFrame(frmGRID);
        line_3->setObjectName("line_3");
        line_3->setGeometry(QRect(230, 195, 16, 16));
        line_3->setFrameShadow(QFrame::Plain);
        line_3->setLineWidth(3);
        line_3->setFrameShape(QFrame::Shape::VLine);
        line_6 = new QFrame(frmGRID);
        line_6->setObjectName("line_6");
        line_6->setGeometry(QRect(245, 215, 16, 16));
        line_6->setFrameShadow(QFrame::Plain);
        line_6->setLineWidth(3);
        line_6->setFrameShape(QFrame::Shape::VLine);
        line_7 = new QFrame(frmGRID);
        line_7->setObjectName("line_7");
        line_7->setGeometry(QRect(245, 155, 16, 16));
        line_7->setFrameShadow(QFrame::Plain);
        line_7->setLineWidth(3);
        line_7->setFrameShape(QFrame::Shape::VLine);
        line_10 = new QFrame(frmGRID);
        line_10->setObjectName("line_10");
        line_10->setGeometry(QRect(245, 135, 16, 16));
        line_10->setFrameShadow(QFrame::Plain);
        line_10->setLineWidth(3);
        line_10->setFrameShape(QFrame::Shape::VLine);
        line_4 = new QFrame(frmGRID);
        line_4->setObjectName("line_4");
        line_4->setGeometry(QRect(230, 175, 16, 16));
        line_4->setFrameShadow(QFrame::Plain);
        line_4->setLineWidth(3);
        line_4->setFrameShape(QFrame::Shape::VLine);
        line_9 = new QFrame(frmGRID);
        line_9->setObjectName("line_9");
        line_9->setGeometry(QRect(245, 175, 16, 16));
        line_9->setFrameShadow(QFrame::Plain);
        line_9->setLineWidth(3);
        line_9->setFrameShape(QFrame::Shape::VLine);
        line = new QFrame(frmGRID);
        line->setObjectName("line");
        line->setGeometry(QRect(230, 135, 16, 16));
        line->setFrameShadow(QFrame::Plain);
        line->setLineWidth(3);
        line->setFrameShape(QFrame::Shape::VLine);
        label = new QLabel(frmGRID);
        label->setObjectName("label");
        label->setGeometry(QRect(195, 135, 36, 16));
        leGHVV_1 = new QLineEdit(frmGRID);
        leGHVV_1->setObjectName("leGHVV_1");
        leGHVV_1->setGeometry(QRect(175, 100, 61, 21));
        leGHVV_1->setReadOnly(true);
        leSHV_2 = new QLineEdit(frmGRID);
        leSHV_2->setObjectName("leSHV_2");
        leSHV_2->setGeometry(QRect(255, 100, 61, 21));
        leGHVV_2 = new QLineEdit(frmGRID);
        leGHVV_2->setObjectName("leGHVV_2");
        leGHVV_2->setGeometry(QRect(320, 100, 61, 21));
        leGHVV_2->setReadOnly(true);
        chkGRID1enable = new QCheckBox(frmGRID);
        chkGRID1enable->setObjectName("chkGRID1enable");
        chkGRID1enable->setGeometry(QRect(110, 75, 86, 20));
        chkGRID2enable = new QCheckBox(frmGRID);
        chkGRID2enable->setObjectName("chkGRID2enable");
        chkGRID2enable->setGeometry(QRect(255, 70, 86, 20));
        label_3 = new QLabel(frmGRID);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(110, 55, 66, 16));
        label_4 = new QLabel(frmGRID);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(255, 55, 66, 16));
        gbGrid1RF = new QGroupBox(frmGRID);
        gbGrid1RF->setObjectName("gbGrid1RF");
        gbGrid1RF->setGeometry(QRect(10, 155, 201, 241));
        label_5 = new QLabel(gbGrid1RF);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 71, 59, 16));
        leSRFDRV_1 = new QLineEdit(gbGrid1RF);
        leSRFDRV_1->setObjectName("leSRFDRV_1");
        leSRFDRV_1->setGeometry(QRect(60, 70, 91, 21));
        label_6 = new QLabel(gbGrid1RF);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(11, 123, 59, 16));
        label_7 = new QLabel(gbGrid1RF);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(11, 148, 59, 16));
        leSRFFRQ_1 = new QLineEdit(gbGrid1RF);
        leSRFFRQ_1->setObjectName("leSRFFRQ_1");
        leSRFFRQ_1->setGeometry(QRect(60, 121, 91, 21));
        leGRFPPVP_1 = new QLineEdit(gbGrid1RF);
        leGRFPPVP_1->setObjectName("leGRFPPVP_1");
        leGRFPPVP_1->setGeometry(QRect(60, 145, 91, 21));
        leGRFPPVP_1->setReadOnly(true);
        label_9 = new QLabel(gbGrid1RF);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(159, 70, 31, 16));
        label_10 = new QLabel(gbGrid1RF);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(159, 123, 31, 16));
        label_11 = new QLabel(gbGrid1RF);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(158, 148, 31, 16));
        pbTuneG1 = new QPushButton(gbGrid1RF);
        pbTuneG1->setObjectName("pbTuneG1");
        pbTuneG1->setGeometry(QRect(10, 195, 81, 32));
        pbTuneG1->setAutoDefault(false);
        pbRetuneG1 = new QPushButton(gbGrid1RF);
        pbRetuneG1->setObjectName("pbRetuneG1");
        pbRetuneG1->setGeometry(QRect(105, 195, 81, 32));
        pbRetuneG1->setAutoDefault(false);
        leGRFPWR_1 = new QLineEdit(gbGrid1RF);
        leGRFPWR_1->setObjectName("leGRFPWR_1");
        leGRFPWR_1->setGeometry(QRect(60, 170, 91, 21));
        leGRFPWR_1->setReadOnly(true);
        label_71 = new QLabel(gbGrid1RF);
        label_71->setObjectName("label_71");
        label_71->setGeometry(QRect(10, 170, 59, 16));
        label_72 = new QLabel(gbGrid1RF);
        label_72->setObjectName("label_72");
        label_72->setGeometry(QRect(155, 175, 31, 16));
        leSRFVLT_1 = new QLineEdit(gbGrid1RF);
        leSRFVLT_1->setObjectName("leSRFVLT_1");
        leSRFVLT_1->setEnabled(false);
        leSRFVLT_1->setGeometry(QRect(60, 95, 91, 21));
        label_14 = new QLabel(gbGrid1RF);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(10, 100, 59, 16));
        label_15 = new QLabel(gbGrid1RF);
        label_15->setObjectName("label_15");
        label_15->setGeometry(QRect(160, 100, 31, 16));
        rbModeManual = new QRadioButton(gbGrid1RF);
        rbModeManual->setObjectName("rbModeManual");
        rbModeManual->setGeometry(QRect(70, 25, 71, 20));
        rbModeManual->setChecked(true);
        rbModeAuto = new QRadioButton(gbGrid1RF);
        rbModeAuto->setObjectName("rbModeAuto");
        rbModeAuto->setGeometry(QRect(70, 45, 71, 20));
        pbShutdown = new QPushButton(frmGRID);
        pbShutdown->setObjectName("pbShutdown");
        pbShutdown->setGeometry(QRect(10, 10, 101, 71));
        pbShutdown->setAutoDefault(false);
        label_8 = new QLabel(frmGRID);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(120, 10, 256, 26));
        QFont font;
        font.setPointSize(16);
        label_8->setFont(font);
        label_8->setAlignment(Qt::AlignCenter);
        Vplot = new QCustomPlot(frmGRID);
        Vplot->setObjectName("Vplot");
        Vplot->setGeometry(QRect(470, 0, 476, 406));
        leYmax = new QLineEdit(frmGRID);
        leYmax->setObjectName("leYmax");
        leYmax->setGeometry(QRect(360, 290, 91, 21));
        leYmin = new QLineEdit(frmGRID);
        leYmin->setObjectName("leYmin");
        leYmin->setGeometry(QRect(360, 320, 91, 21));
        pbResetPlot = new QPushButton(frmGRID);
        pbResetPlot->setObjectName("pbResetPlot");
        pbResetPlot->setGeometry(QRect(360, 350, 91, 32));
        pbResetPlot->setAutoDefault(false);
        label_12 = new QLabel(frmGRID);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(310, 290, 59, 16));
        label_13 = new QLabel(frmGRID);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(310, 320, 59, 16));
        chkYauto = new QCheckBox(frmGRID);
        chkYauto->setObjectName("chkYauto");
        chkYauto->setGeometry(QRect(310, 260, 131, 20));
        chkEnablePlot = new QCheckBox(frmGRID);
        chkEnablePlot->setObjectName("chkEnablePlot");
        chkEnablePlot->setGeometry(QRect(310, 235, 126, 20));
        pbPaste_2 = new QPushButton(tab);
        pbPaste_2->setObjectName("pbPaste_2");
        pbPaste_2->setGeometry(QRect(710, 375, 206, 32));
        pbPaste_2->setAutoDefault(false);
        tabGrid->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        frmPulseSequence = new QFrame(tab_2);
        frmPulseSequence->setObjectName("frmPulseSequence");
        frmPulseSequence->setGeometry(QRect(-15, 10, 931, 391));
        frmPulseSequence->setFrameShape(QFrame::NoFrame);
        frmPulseSequence->setFrameShadow(QFrame::Raised);
        label_16 = new QLabel(frmPulseSequence);
        label_16->setObjectName("label_16");
        label_16->setGeometry(QRect(20, -10, 381, 281));
        label_16->setPixmap(QPixmap(QString::fromUtf8(":/Images/IMStiming.png")));
        label_16->setScaledContents(true);
        label_17 = new QLabel(frmPulseSequence);
        label_17->setObjectName("label_17");
        label_17->setGeometry(QRect(630, 20, 101, 16));
        leTp = new QLineEdit(frmPulseSequence);
        leTp->setObjectName("leTp");
        leTp->setGeometry(QRect(730, 20, 113, 21));
        label_18 = new QLabel(frmPulseSequence);
        label_18->setObjectName("label_18");
        label_18->setGeometry(QRect(550, 40, 181, 16));
        leTs1 = new QLineEdit(frmPulseSequence);
        leTs1->setObjectName("leTs1");
        leTs1->setGeometry(QRect(730, 40, 113, 21));
        label_19 = new QLabel(frmPulseSequence);
        label_19->setObjectName("label_19");
        label_19->setGeometry(QRect(550, 60, 181, 16));
        leTs2 = new QLineEdit(frmPulseSequence);
        leTs2->setObjectName("leTs2");
        leTs2->setGeometry(QRect(730, 60, 113, 21));
        label_20 = new QLabel(frmPulseSequence);
        label_20->setObjectName("label_20");
        label_20->setGeometry(QRect(570, 80, 181, 16));
        leN = new QLineEdit(frmPulseSequence);
        leN->setObjectName("leN");
        leN->setGeometry(QRect(730, 80, 113, 21));
        gbSequenceRepeats = new QGroupBox(frmPulseSequence);
        gbSequenceRepeats->setObjectName("gbSequenceRepeats");
        gbSequenceRepeats->setGeometry(QRect(570, 110, 231, 141));
        comboSelectN = new QComboBox(gbSequenceRepeats);
        comboSelectN->setObjectName("comboSelectN");
        comboSelectN->setGeometry(QRect(110, 30, 104, 26));
        comboSelectN->setMaxCount(20);
        label_21 = new QLabel(gbSequenceRepeats);
        label_21->setObjectName("label_21");
        label_21->setGeometry(QRect(10, 30, 61, 16));
        label_22 = new QLabel(gbSequenceRepeats);
        label_22->setObjectName("label_22");
        label_22->setGeometry(QRect(10, 60, 101, 16));
        leTdn = new QLineEdit(gbSequenceRepeats);
        leTdn->setObjectName("leTdn");
        leTdn->setGeometry(QRect(120, 60, 81, 21));
        label_23 = new QLabel(gbSequenceRepeats);
        label_23->setObjectName("label_23");
        label_23->setGeometry(QRect(10, 80, 101, 16));
        leM = new QLineEdit(gbSequenceRepeats);
        leM->setObjectName("leM");
        leM->setGeometry(QRect(120, 80, 81, 21));
        pbPaste = new QPushButton(gbSequenceRepeats);
        pbPaste->setObjectName("pbPaste");
        pbPaste->setGeometry(QRect(10, 105, 206, 32));
        pbPaste->setAutoDefault(false);
        pbDownload = new QPushButton(frmPulseSequence);
        pbDownload->setObjectName("pbDownload");
        pbDownload->setGeometry(QRect(420, 290, 211, 32));
        pbDownload->setAutoDefault(false);
        pbStart = new QPushButton(frmPulseSequence);
        pbStart->setObjectName("pbStart");
        pbStart->setGeometry(QRect(800, 290, 113, 32));
        pbStart->setAutoDefault(false);
        label_24 = new QLabel(frmPulseSequence);
        label_24->setObjectName("label_24");
        label_24->setGeometry(QRect(540, 260, 181, 16));
        leFullRepeat = new QLineEdit(frmPulseSequence);
        leFullRepeat->setObjectName("leFullRepeat");
        leFullRepeat->setGeometry(QRect(700, 260, 113, 21));
        pbShow = new QPushButton(frmPulseSequence);
        pbShow->setObjectName("pbShow");
        pbShow->setGeometry(QRect(630, 290, 171, 32));
        pbShow->setAutoDefault(false);
        label_25 = new QLabel(frmPulseSequence);
        label_25->setObjectName("label_25");
        label_25->setGeometry(QRect(60, 290, 151, 81));
        label_26 = new QLabel(frmPulseSequence);
        label_26->setObjectName("label_26");
        label_26->setGeometry(QRect(720, 320, 191, 51));
        label_26->setWordWrap(true);
        label_27 = new QLabel(frmPulseSequence);
        label_27->setObjectName("label_27");
        label_27->setGeometry(QRect(850, 45, 66, 16));
        label_28 = new QLabel(frmPulseSequence);
        label_28->setObjectName("label_28");
        label_28->setGeometry(QRect(850, 65, 81, 16));
        label_29 = new QLabel(frmPulseSequence);
        label_29->setObjectName("label_29");
        label_29->setGeometry(QRect(400, 130, 146, 16));
        label_30 = new QLabel(frmPulseSequence);
        label_30->setObjectName("label_30");
        label_30->setGeometry(QRect(815, 125, 106, 76));
        label_30->setWordWrap(true);
        chkTs1eTs2 = new QCheckBox(frmPulseSequence);
        chkTs1eTs2->setObjectName("chkTs1eTs2");
        chkTs1eTs2->setGeometry(QRect(425, 170, 86, 20));
        chkTs2eTs1 = new QCheckBox(frmPulseSequence);
        chkTs2eTs1->setObjectName("chkTs2eTs1");
        chkTs2eTs1->setGeometry(QRect(425, 195, 86, 20));
        label_31 = new QLabel(frmPulseSequence);
        label_31->setObjectName("label_31");
        label_31->setGeometry(QRect(400, 90, 146, 16));
        chkSwapAD = new QCheckBox(frmPulseSequence);
        chkSwapAD->setObjectName("chkSwapAD");
        chkSwapAD->setGeometry(QRect(195, 355, 161, 20));
        tabGrid->addTab(tab_2, QString());

        retranslateUi(Grid);

        tabGrid->setCurrentIndex(0);
        comboSelectN->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(Grid);
    } // setupUi

    void retranslateUi(QDialog *Grid)
    {
        Grid->setWindowTitle(QCoreApplication::translate("Grid", "Grid control panel", nullptr));
        label_2->setText(QCoreApplication::translate("Grid", "Grid 2", nullptr));
        label->setText(QCoreApplication::translate("Grid", "Grid 1", nullptr));
        chkGRID1enable->setText(QCoreApplication::translate("Grid", "Enable", nullptr));
        chkGRID2enable->setText(QCoreApplication::translate("Grid", "Enable", nullptr));
        label_3->setText(QCoreApplication::translate("Grid", "Grid 1 DC", nullptr));
        label_4->setText(QCoreApplication::translate("Grid", "Grid 2 DC", nullptr));
        gbGrid1RF->setTitle(QCoreApplication::translate("Grid", "Grid 1 RF driver", nullptr));
        label_5->setText(QCoreApplication::translate("Grid", "Drive", nullptr));
        label_6->setText(QCoreApplication::translate("Grid", "Freq", nullptr));
        label_7->setText(QCoreApplication::translate("Grid", "RF+", nullptr));
        label_9->setText(QCoreApplication::translate("Grid", "%", nullptr));
        label_10->setText(QCoreApplication::translate("Grid", "Hz", nullptr));
        label_11->setText(QCoreApplication::translate("Grid", "Vp-p", nullptr));
        pbTuneG1->setText(QCoreApplication::translate("Grid", "Tune", nullptr));
        pbRetuneG1->setText(QCoreApplication::translate("Grid", "Retune", nullptr));
        label_71->setText(QCoreApplication::translate("Grid", "Power", nullptr));
        label_72->setText(QCoreApplication::translate("Grid", "W", nullptr));
        label_14->setText(QCoreApplication::translate("Grid", "Voltage", nullptr));
        label_15->setText(QCoreApplication::translate("Grid", "Vp-p", nullptr));
        rbModeManual->setText(QCoreApplication::translate("Grid", "Manual", nullptr));
        rbModeAuto->setText(QCoreApplication::translate("Grid", "Auto", nullptr));
#if QT_CONFIG(tooltip)
        pbShutdown->setToolTip(QCoreApplication::translate("Grid", "Pressing this button will set all values to zero.", nullptr));
#endif // QT_CONFIG(tooltip)
        pbShutdown->setText(QCoreApplication::translate("Grid", "Shutdown", nullptr));
        label_8->setText(QCoreApplication::translate("Grid", "Grid Ion Fragmenter", nullptr));
        leYmax->setText(QString());
        leYmax->setPlaceholderText(QCoreApplication::translate("Grid", "5000", nullptr));
        leYmin->setText(QString());
        leYmin->setPlaceholderText(QCoreApplication::translate("Grid", "0", nullptr));
        pbResetPlot->setText(QCoreApplication::translate("Grid", "Reset plot", nullptr));
        label_12->setText(QCoreApplication::translate("Grid", "Y max", nullptr));
        label_13->setText(QCoreApplication::translate("Grid", "Y min", nullptr));
        chkYauto->setText(QCoreApplication::translate("Grid", "Y axis auto range", nullptr));
        chkEnablePlot->setText(QCoreApplication::translate("Grid", "Enable plotting", nullptr));
#if QT_CONFIG(tooltip)
        pbPaste_2->setToolTip(QCoreApplication::translate("Grid", "Paste sequence data from the clipboard, must be text, you can not paste from excel. Copy from excel to a text file and then copy from the text file and paste here.", nullptr));
#endif // QT_CONFIG(tooltip)
        pbPaste_2->setText(QCoreApplication::translate("Grid", "Paste text from clipboard", nullptr));
        tabGrid->setTabText(tabGrid->indexOf(tab), QCoreApplication::translate("Grid", "Grid", nullptr));
        label_16->setText(QString());
        label_17->setText(QCoreApplication::translate("Grid", "Tp, period, mS", nullptr));
        leTp->setText(QCoreApplication::translate("Grid", "100", nullptr));
        label_18->setText(QCoreApplication::translate("Grid", "Ts1, sutter 1 pulse width, uS", nullptr));
        leTs1->setText(QCoreApplication::translate("Grid", "50", nullptr));
        label_19->setText(QCoreApplication::translate("Grid", "Ts2, sutter 2 pulse width, uS", nullptr));
        leTs2->setText(QCoreApplication::translate("Grid", "50", nullptr));
        label_20->setText(QCoreApplication::translate("Grid", "n, number of sequences", nullptr));
        leN->setText(QCoreApplication::translate("Grid", "10", nullptr));
        gbSequenceRepeats->setTitle(QCoreApplication::translate("Grid", "Sequence repeats", nullptr));
        comboSelectN->setCurrentText(QString());
        label_21->setText(QCoreApplication::translate("Grid", "Select n", nullptr));
        label_22->setText(QCoreApplication::translate("Grid", "Tdn, in mS", nullptr));
        leTdn->setText(QCoreApplication::translate("Grid", "10", nullptr));
        label_23->setText(QCoreApplication::translate("Grid", "m, repeat count", nullptr));
        leM->setText(QCoreApplication::translate("Grid", "5", nullptr));
#if QT_CONFIG(tooltip)
        pbPaste->setToolTip(QCoreApplication::translate("Grid", "Paste sequence data from the clipboard, must be text, you can not paste from excel. Copy from excel to a text file and then copy from the text file and paste here.", nullptr));
#endif // QT_CONFIG(tooltip)
        pbPaste->setText(QCoreApplication::translate("Grid", "Paste text from clipboard", nullptr));
        pbDownload->setText(QCoreApplication::translate("Grid", "Download sequence to MIPS", nullptr));
        pbStart->setText(QCoreApplication::translate("Grid", "Start", nullptr));
        label_24->setText(QCoreApplication::translate("Grid", "Full series repeat count", nullptr));
        leFullRepeat->setText(QCoreApplication::translate("Grid", "10", nullptr));
        pbShow->setText(QCoreApplication::translate("Grid", "Show Pulse Sequence", nullptr));
        label_25->setText(QCoreApplication::translate("Grid", "<html><head/><body><p>MIPS digital I/O map<br/>A = Sync out<br/>B = A, ADC trigger<br/>C = Shutter 1<br/>D = Shutter 2</p><p><br/></p></body></html>", nullptr));
        label_26->setText(QCoreApplication::translate("Grid", "Press start to start generation of pulse sequence or apply trigger to R input", nullptr));
        label_27->setText(QCoreApplication::translate("Grid", "20uS min", nullptr));
        label_28->setText(QCoreApplication::translate("Grid", "20uS min", nullptr));
        label_29->setText(QCoreApplication::translate("Grid", "Set Ts2 to 0 to disable", nullptr));
        label_30->setText(QCoreApplication::translate("Grid", "Tp must exceed the largest Tdn value.", nullptr));
#if QT_CONFIG(tooltip)
        chkTs1eTs2->setToolTip(QCoreApplication::translate("Grid", "If checked make sure Ts1 is set to 0.", nullptr));
#endif // QT_CONFIG(tooltip)
        chkTs1eTs2->setText(QCoreApplication::translate("Grid", "Ts1=Ts2", nullptr));
#if QT_CONFIG(tooltip)
        chkTs2eTs1->setToolTip(QCoreApplication::translate("Grid", "If checked make sure Ts2 is set to 0.", nullptr));
#endif // QT_CONFIG(tooltip)
        chkTs2eTs1->setText(QCoreApplication::translate("Grid", "Ts2=Ts1", nullptr));
        label_31->setText(QCoreApplication::translate("Grid", "Set Ts1 to 0 to disable", nullptr));
        chkSwapAD->setText(QCoreApplication::translate("Grid", "Swap outputs A and D", nullptr));
        tabGrid->setTabText(tabGrid->indexOf(tab_2), QCoreApplication::translate("Grid", "Pulse sequence", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Grid: public Ui_Grid {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRID_H
