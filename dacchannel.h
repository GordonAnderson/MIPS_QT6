// =============================================================================
// dacchannel.h
//
// DACchannel — single DAC output channel widget for the MIPS custom control
// panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  comms.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef DACCHANNEL_H
#define DACCHANNEL_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleValidator>
#include "comms.h"

// -----------------------------------------------------------------------------
// DACchannel
//
// Displays a labelled line-edit for one DAC output channel. Supports optional
// linear scaling (display = m*raw + b) and a configurable format string.
// Update() polls the hardware; VdacChange() writes a new value back.
// The widget can be dragged to a new position on the control panel background.
// -----------------------------------------------------------------------------
class DACchannel : public QWidget
{
    Q_OBJECT
public:
    DACchannel(QWidget *parent, QString name, QString MIPSname, int x, int y);
    void Show(void);
    void Update(void);
    QString Report(void);
    bool SetValues(QString strVals);
    QString ProcessCommand(QString cmd);

    QWidget *p;
    QString  Title;
    int      X, Y;
    QString  MIPSnm;
    int      Channel;
    Comms   *comms;
    float    b, m;
    QString  Units;
    QString  Format;
    QFrame  *frmDAC;

private:
    QLineEdit *Vdac;
    QLabel    *labels[2];
    bool       Updating;
    bool       UpdateOff;

private slots:
    void VdacChange(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // DACCHANNEL_H
