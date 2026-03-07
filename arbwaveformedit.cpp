// =============================================================================
// arbwaveformedit.cpp
//
// ARB waveform editor dialog for the MIPS host application.
//
// Depends on:  qcustomplot.h, ui_arbwaveformedit.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "arbwaveformedit.h"
#include "ui_arbwaveformedit.h"
#include "qcustomplot.h"

static const int ARB_PPP_MIN        = 8;    // Minimum points per period
static const int ARB_PPP_MAX        = 32;   // Maximum points per period (also waveform buffer size)
static const double ARB_AMP_MAX     = 100.0; // Full-scale amplitude (percent)
static const double ARB_YAXIS_MARGIN = 5.0; // Y axis margin beyond ±100%

// -----------------------------------------------------------------------------
// Constructor — clamps PPP, connects buttons, initialises the plot.
// -----------------------------------------------------------------------------
ARBwaveformEdit::ARBwaveformEdit(QWidget *parent, int ppp) :
    QDialog(parent),
    ui(new Ui::ARBwaveformEdit)
{
    ui->setupUi(this);
    this->setFixedSize(617, 494);

    PPP = ppp;
    if(PPP < ARB_PPP_MIN) PPP = ARB_PPP_MIN;
    if(PPP > ARB_PPP_MAX) PPP = ARB_PPP_MAX;

    connect(ui->pbGenUpDown,  &QPushButton::pressed,  this, &ARBwaveformEdit::GenerateUpDown);
    connect(ui->pbGenSine,    &QPushButton::pressed,  this, &ARBwaveformEdit::GenerateSine);
    connect(ui->pbPlot,       &QPushButton::pressed,  this, &ARBwaveformEdit::PlotData);
    connect(ui->pbInvert,     &QPushButton::pressed,  this, &ARBwaveformEdit::InvertData);
    connect(ui->buttonBox,    &QDialogButtonBox::accepted, this, &ARBwaveformEdit::WaveformAccepted);

    // Initialise the waveform preview plot
    ui->plot->addGraph();
    ui->plot->xAxis->setLabel("index");
    ui->plot->yAxis->setLabel("amplitude");
    ui->plot->xAxis->setRange(0, PPP - 1);
    ui->plot->yAxis->setRange(-(ARB_AMP_MAX + ARB_YAXIS_MARGIN), ARB_AMP_MAX + ARB_YAXIS_MARGIN);
    ui->plot->replot();
}

ARBwaveformEdit::~ARBwaveformEdit()
{
    delete ui;
}

// -----------------------------------------------------------------------------
// SetWaveform — loads an external waveform buffer into the editor and plots it.
// -----------------------------------------------------------------------------
void ARBwaveformEdit::SetWaveform(int *wf)
{
    if(wf == NULL) return;
    ui->txtData->clear();
    for(int i = 0; i < PPP; i++)
    {
        Waveform[i] = wf[i];
        ui->txtData->appendPlainText(QString::number(Waveform[i]));
    }
    ui->txtData->moveCursor(QTextCursor::Start);

    QVector<double> x(PPP), y(PPP);
    for(int i = 0; i < PPP; i++)
    {
        x[i] = i;
        y[i] = Waveform[i];
    }
    ui->plot->graph(0)->setData(x, y);
    ui->plot->replot();
}

// -----------------------------------------------------------------------------
// GetWaveform — copies the internal waveform buffer to the caller.
// Note: copies the full ARB_PPP_MAX buffer regardless of current PPP, so the
// caller's buffer must be at least ARB_PPP_MAX elements.
// -----------------------------------------------------------------------------
void ARBwaveformEdit::GetWaveform(int *wf)
{
    for(int i = 0; i < ARB_PPP_MAX; i++) wf[i] = Waveform[i];
}

// -----------------------------------------------------------------------------
// GenerateUpDown — generates a square wave with the specified up/down ratio.
// -----------------------------------------------------------------------------
void ARBwaveformEdit::GenerateUpDown(void)
{
    int up      = ui->leUp->text().toInt();
    int down    = ui->leDown->text().toInt();
    int binsper = PPP / (up + down);

    ui->txtData->clear();
    for(int i = 0; i < PPP; i++)
    {
        Waveform[i] = (i < up * binsper) ? (int)ARB_AMP_MAX : (int)-ARB_AMP_MAX;
        ui->txtData->appendPlainText(QString::number(Waveform[i]));
    }
    ui->txtData->moveCursor(QTextCursor::Start);

    QVector<double> x(PPP), y(PPP);
    for(int i = 0; i < PPP; i++) { x[i] = i; y[i] = Waveform[i]; }
    ui->plot->graph(0)->setData(x, y);
    ui->plot->replot();
}

// -----------------------------------------------------------------------------
// GenerateSine — generates a sine wave with the specified cycles and phase.
// -----------------------------------------------------------------------------
void ARBwaveformEdit::GenerateSine(void)
{
    ui->txtData->clear();
    for(int i = 0; i < PPP; i++)
    {
        Waveform[i] = qSin(qDegreesToRadians((ui->leCycles->text().toFloat() * 360.0 * i) / PPP)
                           + qDegreesToRadians(ui->lePhase->text().toFloat())) * ARB_AMP_MAX;
        ui->txtData->appendPlainText(QString::number(Waveform[i]));
    }
    ui->txtData->moveCursor(QTextCursor::Start);

    QVector<double> x(PPP), y(PPP);
    for(int i = 0; i < PPP; i++) { x[i] = i; y[i] = Waveform[i]; }
    ui->plot->graph(0)->setData(x, y);
    ui->plot->replot();
}

// -----------------------------------------------------------------------------
// PlotData — reads the text box, parses values into the waveform buffer,
// and refreshes the plot.
// -----------------------------------------------------------------------------
void ARBwaveformEdit::PlotData(void)
{
    QStringList valStrings = ui->txtData->toPlainText().split("\n");
    for(int i = 0; i < PPP; i++)
    {
        Waveform[i] = (valStrings.count() > i) ? valStrings[i].toFloat() : 0;
    }

    QVector<double> x(PPP), y(PPP);
    for(int i = 0; i < PPP; i++) { x[i] = i; y[i] = Waveform[i]; }
    ui->plot->graph(0)->setData(x, y);
    ui->plot->replot();
}

// -----------------------------------------------------------------------------
// InvertData — negates all waveform values, updates the text box and plot.
// -----------------------------------------------------------------------------
void ARBwaveformEdit::InvertData(void)
{
    QStringList valStrings = ui->txtData->toPlainText().split("\n");
    QVector<double> x(PPP), y(PPP);
    ui->txtData->clear();
    for(int i = 0; i < PPP; i++)
    {
        Waveform[i] = (valStrings.count() > i) ? -valStrings[i].toFloat() : 0;
        x[i] = i;
        y[i] = Waveform[i];
        ui->txtData->appendPlainText(QString::number(Waveform[i]));
    }
    ui->txtData->moveCursor(QTextCursor::Start);
    ui->plot->graph(0)->setData(x, y);
    ui->plot->replot();
}

// -----------------------------------------------------------------------------
// WaveformAccepted — emits WaveformReady() when the dialog is accepted.
// -----------------------------------------------------------------------------
void ARBwaveformEdit::WaveformAccepted(void)
{
    emit WaveformReady();
}
