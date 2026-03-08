// =============================================================================
// esi.h
//
// ESI — electrospray ionisation high-voltage channel widget for the MIPS
// custom control panel. Extracted from controlpanel.cpp/.h during Phase 3
// refactoring.
//
// Depends on:  comms.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef ESI_H
#define ESI_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QDoubleValidator>

#include "comms.h"

// -----------------------------------------------------------------------------
// ESI
//
// Displays a setpoint line-edit, a read-back line-edit, and an Enable checkbox
// for one ESI high-voltage channel. Supports Shutdown/Restore to save and
// restore the enable state during a system shutdown. The widget can be dragged
// to a new position on the control panel background.
// -----------------------------------------------------------------------------
class ESI : public QWidget
{
    Q_OBJECT
public:
    ESI(QWidget *parent, QString name, QString MIPSname, int x, int y);
    void Show(void);
    void Update(void);
    QString Report(void);
    QString ProcessCommand(QString cmd);
    bool SetValues(QString strVals);
    void Shutdown(void);
    void Restore(void);

    QWidget *p;
    QString  Title;
    int      X, Y;
    QString  MIPSnm;
    int      Channel;
    Comms   *comms;
    bool     isShutdown;
    QFrame  *frmESI;

private:
    QLineEdit *ESIsp;
    QLineEdit *ESIrb;
    QCheckBox *ESIena;
    QLabel    *labels[2];
    bool       activeEnableState;

private slots:
    void ESIChange(void);
    void ESIenaChange(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // ESI_H
