// =============================================================================
// adc.h
//
// Class declarations for ADC and ADCchannel.
// See adc.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef ADC_H
#define ADC_H

#include "ui_mips.h"
#include "mips.h"
#include "comms.h"

#include <QtCore/QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QStatusBar>
#include <QMessageBox>
#include <QObject>
#include <QApplication>
#include <QFileInfo>

// -----------------------------------------------------------------------------
// ADC — manages the high-speed digitizer tab. Handles buffer allocation,
// acquisition setup, triggering, abort, and accumulated average plotting.
// -----------------------------------------------------------------------------
class ADC: public QDialog
{
    Q_OBJECT

public:
    ADC(QObject *parent, Ui::MIPS *w, Comms *c);
    void Update(bool UpdateSelected = false);

    Ui::MIPS *ui;
    Comms    *comms;
    MIPS     *mips = NULL;

    // ADC sample buffers — allocated in ADCsetup() to match NumSamples
    quint16  *ADCbuffer;
    int      *ADCbufferSum;
    int       NumScans;

private slots:
    void ADCupdated(void);
    void ADCsetup(void);
    void ADCtrigger(void);
    void ADCabort(void);
    void ADCrecordingDone(void);
    void ADCvectorReady(void);
    void SetZoom(void);
};

// -----------------------------------------------------------------------------
// ADCchannel — control panel widget for a single ADC input channel.
// Displays a scaled readback value. Scaling: display = m * readValue + b.
// If U != 0, applies log scaling: display = 10^(readValue - U).
// -----------------------------------------------------------------------------
class ADCchannel : public QWidget
{
    Q_OBJECT
public:
    ADCchannel(QWidget *parent, QString name, QString MIPSname, int x, int y);
    void    Show(void);
    void    Update(void);
    QString Report(void);
    bool    SetValues(QString strVals);
    QString ProcessCommand(QString cmd);

    QWidget *p;
    QString  Title;
    int      X, Y;
    QString  MIPSnm;
    int      Channel;
    Comms   *comms;
    float    m;         // linear scale factor (display = m * raw + b)
    float    b;         // linear offset
    float    U;         // log scale exponent base (0 = linear mode)
    QString  Units;
    QString  Format;    // printf-style format string for display value
    QString  LLimit;    // lower limit — displays "OFF" if value is below this
    QFrame  *frmADC;

private:
    QLineEdit *Vadc;
    QLabel    *labels[2];

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // ADC_H
