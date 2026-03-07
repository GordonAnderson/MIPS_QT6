// =============================================================================
// adc.cpp
//
// Implements two classes for ADC (Analog-to-Digital Converter) interaction:
//
//   ADC         — controls the high-speed digitizer tab. Manages buffer
//                 allocation, acquisition setup, triggering, abort, and
//                 accumulated average plotting via QCustomPlot.
//
//   ADCchannel  — lightweight control panel widget that reads and displays
//                 a single ADC input channel value, with optional linear
//                 scaling (display = m * readValue + b) and log scaling.
//
// Hardware:    MIPS ADC module (commands: ADCINIT, ADCTRIG, ADCABORT, ADC,n)
// Depends on:  comms.h, Utilities.h, QCustomPlot
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "adc.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// ADC constructor — connects UI signals to slots and initialises the plot axes.
// -----------------------------------------------------------------------------
ADC::ADC(QObject *parent, Ui::MIPS *w, Comms *c)
{
    ui = w;
    comms = c;

    if(parent != NULL) mips = qobject_cast<MIPS*>(parent);

    ADCbuffer = NULL;
    ADCbufferSum = NULL;

    QObjectList widgetList = ui->gbADCdigitizer->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("leS"))
        {
            ((QLineEdit *)w)->setValidator(new QIntValidator);
            connect(((QLineEdit *)w),SIGNAL(editingFinished()),this,SLOT(ADCupdated()));
        }
    }

    connect(ui->pbADCinit,    SIGNAL(pressed()),          this, SLOT(ADCsetup()));
    connect(ui->pbADCtrigger, SIGNAL(pressed()),          this, SLOT(ADCtrigger()));
    connect(ui->pbADCabort,   SIGNAL(pressed()),          this, SLOT(ADCabort()));
    connect(comms, SIGNAL(ADCvectorReady()),              this, SLOT(ADCvectorReady()));
    connect(comms, SIGNAL(ADCrecordingDone()),            this, SLOT(ADCrecordingDone()));
    connect(ui->chkADCzoomX,  SIGNAL(clicked(bool)),     this, SLOT(SetZoom()));
    connect(ui->chkADCzoomY,  SIGNAL(clicked(bool)),     this, SLOT(SetZoom()));

    // Setup the plot axes
    ui->ADCdata->xAxis->setLabel("Time, sec");
    ui->ADCdata->yAxis->setLabel("ADC counts");
    ui->ADCdata->addGraph();
}

// -----------------------------------------------------------------------------
// Update — polls all ADC parameter line edits from hardware on each update
// cycle. Skips fields that currently have focus unless UpdateSelected is true.
// -----------------------------------------------------------------------------
void ADC::Update(bool UpdateSelected)
{
    QString res;

    QObjectList widgetList = ui->gbADCdigitizer->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("le"))
        {
            if((!((QLineEdit *)w)->hasFocus()) || (UpdateSelected))
            {
                res = "G" + w->objectName().mid(3).replace("_",",") + "\n";
                ((QLineEdit *)w)->setText(comms->SendMess(res));
            }
        }
    }
}

// -----------------------------------------------------------------------------
// ADCupdated — called when any ADC parameter line edit finishes editing.
// Derives the MIPS set command from the widget object name and sends it.
// -----------------------------------------------------------------------------
void ADC::ADCupdated(void)
{
    QObject* obj = sender();
    QString res;

    if(!((QLineEdit *)obj)->isModified()) return;
    res = obj->objectName().mid(2).replace("_",",") + "," + ((QLineEdit *)obj)->text() + "\n";
    comms->SendCommand(res.toStdString().c_str());
    ((QLineEdit *)obj)->setModified(false);
}

// -----------------------------------------------------------------------------
// ADCsetup — allocates sample buffers and sends ADCINIT to arm the digitizer.
// Waits for the comms layer to confirm buffer transfer before returning.
// -----------------------------------------------------------------------------
void ADC::ADCsetup(void)
{
    int NumSamples;

    NumSamples = ui->leSADCSAMPS->text().toInt();
    if(ADCbuffer != NULL) delete ADCbuffer;
    ADCbuffer = new quint16 [NumSamples];
    if(ADCbufferSum != NULL) delete ADCbufferSum;
    ADCbufferSum = new int [NumSamples];
    for(int i=0; i<NumSamples; i++) ADCbufferSum[i] = 0;
    NumScans = 0;
    while(true)
    {
        if(!comms->SendCommand("ADCINIT\n")) break;
        comms->GetADCbuffer(ADCbuffer,NumSamples);
        if(mips!= NULL) mips->statusMessage("ADC setup complete! Waiting for trigger.");
        return;
    }
    if(mips!= NULL) mips->statusMessage("ADC setup failed! Press ADC abort.");
}

// ADCtrigger — sends ADCTRIG to start acquisition and reports status.
void ADC::ADCtrigger(void)
{
    comms->SendString("ADCTRIG\n");
    if(mips!= NULL) mips->statusMessage("ADC Triggered!");
}

// ADCabort — sends ADCABORT to halt acquisition.
void ADC::ADCabort(void)
{
    comms->SendString("ADCABORT\n");
    if(mips!= NULL) mips->statusMessage("ADC aborted! Press ADC setup to restart.");
}

// ADCrecordingDone — slot called when the comms layer signals the DMA
// recording is complete. Releases the ADC buffer and reports status.
void ADC::ADCrecordingDone(void)
{
    comms->ADCrelease();
    if(mips!= NULL) mips->statusMessage("ADC recording complete!, Press ADC setup to restart.");
}

// -----------------------------------------------------------------------------
// ADCvectorReady — called each time a complete ADC vector arrives. Accumulates
// into ADCbufferSum for averaging, then replots the running average waveform.
// -----------------------------------------------------------------------------
void ADC::ADCvectorReady(void)
{
    int NumSamples;
    int Rate;

    NumScans++;
    NumSamples = ui->leSADCSAMPS->text().toInt();
    for(int i=0; i<NumSamples; i++) ADCbufferSum[i] += ADCbuffer[i];
    Rate = ui->leSADCRATE->text().toInt();
    QVector<double> x(NumSamples), y(NumSamples);
    for(int i=0; i<NumSamples; i++)
    {
        y[i] = ADCbufferSum[i] / NumScans;
        x[i] = (float)i / (float)Rate;
    }
    ui->ADCdata->graph(0)->setData(x, y);
    ui->ADCdata->yAxis->rescale(true);
    ui->ADCdata->xAxis->rescale(true);
    ui->ADCdata->replot();
    if(mips!= NULL) mips->statusMessage("ADC vector read!, waiting for next trigger.");
}

// -----------------------------------------------------------------------------
// SetZoom — configures QCustomPlot interaction mode based on the X/Y zoom
// checkboxes. Both unchecked disables all interaction.
// -----------------------------------------------------------------------------
void ADC::SetZoom(void)
{
    if(ui->chkADCzoomX->isChecked() && ui->chkADCzoomY->isChecked())
    {
        ui->ADCdata->yAxis->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);
        ui->ADCdata->yAxis->axisRect()->setRangeDrag(Qt::Vertical | Qt::Horizontal);
        ui->ADCdata->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    }
    else if(ui->chkADCzoomX->isChecked())
    {
        ui->ADCdata->yAxis->axisRect()->setRangeZoom(Qt::Horizontal);
        ui->ADCdata->yAxis->axisRect()->setRangeDrag(Qt::Horizontal);
        ui->ADCdata->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    }
    else if(ui->chkADCzoomY->isChecked())
    {
        ui->ADCdata->yAxis->axisRect()->setRangeZoom(Qt::Vertical);
        ui->ADCdata->yAxis->axisRect()->setRangeDrag(Qt::Vertical);
        ui->ADCdata->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    }
    else
    {
        ui->ADCdata->setInteractions(QCP::iNone);
    }
}

// =============================================================================
// ADCchannel — control panel widget for a single ADC input channel
// =============================================================================

// ADCchannel — constructor. Records widget identity and defaults for scale
// factors (m=1, b=0), format, and limits. Call Show() to create the widgets.
ADCchannel::ADCchannel(QWidget *parent, QString name, QString MIPSname, int x, int y) : QWidget(parent)
{
    p      = parent;
    Title  = name;
    MIPSnm = MIPSname;
    X      = x;
    Y      = y;
    comms  = NULL;
    Units  = "V";
    m      = 1;
    b      = 0;
    U      = 0;
    Format = "%.3f";
    LLimit.clear();
}

// Show — creates and positions the widget frame, value display, title label,
// and units label on the parent control panel.
void ADCchannel::Show(void)
{
    frmADC    = new QFrame(p);              frmADC->setGeometry(X, Y, 180, 21);
    Vadc      = new QLineEdit(frmADC);      Vadc->setGeometry(70, 0, 70, 21);
    Vadc->setValidator(new QDoubleValidator);
    labels[0] = new QLabel(Title, frmADC);  labels[0]->setGeometry(0,   0, 59, 16);
    labels[1] = new QLabel(Units, frmADC);  labels[1]->setGeometry(150, 0, 31, 16);
    Vadc->setToolTip("ADC input CH" + QString::number(Channel) + ", " + MIPSnm);
    frmADC->installEventFilter(this);
    frmADC->setMouseTracking(true);
    labels[0]->installEventFilter(this);
    labels[0]->setMouseTracking(true);
}

// eventFilter — delegates drag-to-move to moveWidget().
bool ADCchannel::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, frmADC, labels[0], event)) return true;
    return false;
}

// Report — returns a "title,value" CSV string with the current displayed value.
QString ADCchannel::Report(void)
{
    QString res;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    res = title + "," + Vadc->text();
    return(res);
}

// SetValues — parses a "title,value" CSV string and applies it to the display.
// Returns false if the title does not match.
bool ADCchannel::SetValues(QString strVals)
{
    QStringList resList;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!strVals.startsWith(title)) return false;
    resList = strVals.split(",");
    if(resList.count() < 2) return false;
    Vadc->setText(resList[1]);
    Vadc->setModified(true);
    emit Vadc->editingFinished();
    return true;
}

// ProcessCommand — handles scripting API commands for this widget.
// Supported commands:
//   title        — returns the current ADC reading
//   (no set command — ADC channels are read-only)
// Returns "?" if the command is not recognised.
QString ADCchannel::ProcessCommand(QString cmd)
{
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";
    if(cmd == title) return Vadc->text();
    return "?";
}

// Update — reads the ADC channel from hardware and updates the display.
// Applies linear scaling: display = m * readValue + b.
// If U != 0, applies log scaling: display = 10^(readValue - U).
// If LLimit is set and the value is below it, displays "OFF".
void ADCchannel::Update(void)
{
    QString res;

    if(comms == NULL) return;
    comms->rb.clear();
    res = "ADC," + QString::number(Channel) + "\n";
    res = comms->SendMess(res);
    if(res == "") return;
    float Volts = res.toFloat() * m + b;
    if(U != 0)
    {
        res = res.asprintf(Format.toStdString().c_str(), pow(10, Volts - U));
        if(!LLimit.isEmpty()) if(Volts < LLimit.toFloat()) res = "OFF";
    }
    else res = res.asprintf(Format.toStdString().c_str(), Volts);
    if(!Vadc->hasFocus()) Vadc->setText(res);
}
