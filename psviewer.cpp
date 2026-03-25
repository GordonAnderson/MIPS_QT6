// =============================================================================
// psviewer.cpp
//
// Pulse sequence viewer dialog for the MIPS host application.
//
// Depends on:  pse.h, qcustomplot.h, ui_psviewer.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "psviewer.h"
#include "ui_psviewer.h"
#include "qcustomplot.h"

static const int DC_BIAS_CHANNELS   = 16; // Number of DC bias channels per PSG point
static const int BRACKET_Y_STEP     = 25; // Vertical spacing between stacked loop brackets

// -----------------------------------------------------------------------------
// Constructor — builds the plot, annotates loop brackets, connects zoom/plot
// selection controls.
// -----------------------------------------------------------------------------
psviewer::psviewer(QList<psgPoint *> *P, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::psviewer)
{
    ui->setupUi(this);

    psg     = P;
    plotnum = 0;

    // Find the Y axis range across all DC bias values
    minY = 0;
    maxY = 5;
    for(int i = 0; i < psg->size(); i++)
    {
        for(int j = 0; j < DC_BIAS_CHANNELS; j++)
        {
            if((*psg)[i]->DCbias[j] < minY) minY = (*psg)[i]->DCbias[j];
            if((*psg)[i]->DCbias[j] > maxY) maxY = (*psg)[i]->DCbias[j];
        }
    }

    // X axis spans from 0 to the last time point
    minCycles = 0;
    maxCycles = (*psg)[psg->size()-1]->TimePoint;

    // Populate the channel selection combobox
    ui->comboPlotItem->clear();
    ui->comboPlotItem->addItem("");
    for(int i = 0; i < DC_BIAS_CHANNELS; i++)
    {
        ui->comboPlotItem->addItem("CH " + QString::number(i + 1));
    }

    // Configure plot axes
    ui->PSV->xAxis->setLabel("Clock cycles");
    ui->PSV->yAxis->setLabel("Voltage");
    ui->PSV->xAxis->setRange(minCycles, maxCycles);
    ui->PSV->yAxis->setRange(minY, maxY);
    ui->PSV->replot();

    // Annotate loop brackets — scan for Loop entries and find their matching start
    int BracketPos = 1;
    for(int i = 0; i < psg->size(); i++)
    {
        if((*psg)[i]->Loop)
        {
            for(int j = 0; j < psg->size(); j++)
            {
                if((*psg)[i]->LoopName == (*psg)[j]->Name)
                {
                    QCPItemBracket *bracket = new QCPItemBracket(ui->PSV);
                    bracket->left->setCoords((*psg)[j]->TimePoint, BracketPos);
                    bracket->right->setCoords((*psg)[i]->TimePoint, BracketPos);
                    QCPItemText *LoopText = new QCPItemText(ui->PSV);
                    LoopText->position->setParentAnchor(bracket->center);
                    LoopText->position->setCoords(0, -5);
                    LoopText->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
                    if((*psg)[i]->LoopCount == 0) LoopText->setText("Loop forever");
                    else LoopText->setText("Loop " + QString::number((*psg)[i]->LoopCount) + " times");
                    LoopText->setFont(QFont(font().family(), 10));
                    BracketPos += BRACKET_Y_STEP;
                }
            }
        }
    }

    connect(ui->chkZoomX,      &QCheckBox::clicked,              this, &psviewer::SetZoom);
    connect(ui->chkZoomY,      &QCheckBox::clicked,              this, &psviewer::SetZoom);
    connect(ui->comboPlotItem, &QComboBox::currentIndexChanged,  this, &psviewer::PlotSelectedItem);
}

// ~psviewer — destructor. Releases the UI form.
psviewer::~psviewer()
{
    delete ui;
}

// -----------------------------------------------------------------------------
// PlotDCBchannel — plots a single DC bias channel across the full time range.
// Each call adds a new graph with a distinct colour (up to 5 channels).
// -----------------------------------------------------------------------------
void psviewer::PlotDCBchannel(int channel)
{
    QVector<double> x(maxCycles), y(maxCycles);
    for(int j = 0; j < maxCycles; j++)
    {
        x[j] = j;
        y[j] = 0;
    }
    // Apply change points: each PSG entry sets the value from its time point forward
    for(int i = 0; i < psg->size(); i++)
    {
        for(int j = (*psg)[i]->TimePoint; j < maxCycles; j++)
        {
            y[j] = (*psg)[i]->DCbias[channel];
        }
    }

    ui->PSV->legend->setVisible(true);
    ui->PSV->addGraph();

    static const Qt::GlobalColor plotColors[] = { Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::cyan };
    if(plotnum < (int)(sizeof(plotColors) / sizeof(plotColors[0])))
        ui->PSV->graph(plotnum)->setPen(QPen(plotColors[plotnum]));

    ui->PSV->graph(plotnum)->setName("CH " + QString::number(channel + 1));
    ui->PSV->graph(plotnum++)->setData(x, y);
    ui->PSV->replot();
}

// -----------------------------------------------------------------------------
// SetZoom — updates the plot interaction mode based on the zoom checkboxes.
// -----------------------------------------------------------------------------
void psviewer::SetZoom(void)
{
    if(ui->chkZoomX->isChecked() && ui->chkZoomY->isChecked())
    {
        ui->PSV->yAxis->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);
        ui->PSV->yAxis->axisRect()->setRangeDrag(Qt::Vertical | Qt::Horizontal);
        ui->PSV->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    }
    else if(ui->chkZoomX->isChecked())
    {
        ui->PSV->yAxis->axisRect()->setRangeZoom(Qt::Horizontal);
        ui->PSV->yAxis->axisRect()->setRangeDrag(Qt::Horizontal);
        ui->PSV->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    }
    else if(ui->chkZoomY->isChecked())
    {
        ui->PSV->yAxis->axisRect()->setRangeZoom(Qt::Vertical);
        ui->PSV->yAxis->axisRect()->setRangeDrag(Qt::Vertical);
        ui->PSV->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    }
    else
    {
        ui->PSV->setInteractions(QCP::iNone);
    }
}

// -----------------------------------------------------------------------------
// PlotSelectedItem — plots the channel selected in the combobox.
// Index 0 is the blank placeholder; channel indices are 0-based.
// -----------------------------------------------------------------------------
void psviewer::PlotSelectedItem(void)
{
    int index = ui->comboPlotItem->currentIndex();
    if(index == 0) return;
    PlotDCBchannel(index - 1);
}
