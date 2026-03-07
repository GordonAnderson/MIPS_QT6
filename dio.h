// =============================================================================
// dio.h
//
// Class declarations for DIO and DIOchannel.
// See dio.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef DIO_H
#define DIO_H

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
// DIO — manages the Digital IO tab. Handles digital output checkboxes,
// trigger output, frequency/burst generator, and remote navigation of the
// MIPS controller front-panel display via serial control codes.
// -----------------------------------------------------------------------------
class DIO: public QDialog
{
    Q_OBJECT

public:
    DIO(Ui::MIPS *w, Comms *c);
    void Update(void);
    void Save(QString Filename);
    void Load(QString Filename);

    Ui::MIPS *dui;
    Comms    *comms;

private slots:
    void UpdateDIO(void);
    void DOUpdated(void);
    void TrigHigh(void);
    void TrigLow(void);
    void TrigPulse(void);
    void RFgenerate(void);
    void SetFreq(void);
    void SetWidth(void);
    // Remote navigation of MIPS controller front-panel display
    void RemoteNavigation(void);
    void RemoteNavSmallUP(void);
    void RemoteNavLargeUP(void);
    void RemoteNavSmallDown(void);
    void RemoteNavLargeDown(void);
    void RemoteNavSelect(void);
};

// -----------------------------------------------------------------------------
// DIOchannel — control panel widget for a single DIO channel.
// Renders as a checkbox. Channels A–P are read/write; channels above P
// are read-only inputs and will revert any user interaction.
// -----------------------------------------------------------------------------
class DIOchannel : public QWidget
{
    Q_OBJECT
public:
    DIOchannel(QWidget *parent, QString name, QString MIPSname, int x, int y);
    void    Show(void);
    void    Update(void);
    QString Report(void);
    QString ProcessCommand(QString cmd);
    bool    SetValues(QString strVals);

    QWidget *p;
    QString  Title;
    int      X, Y;
    QString  MIPSnm;
    QString  Channel;   // DIO channel letter, e.g. "A"–"P" (output), above "P" = input
    Comms   *comms;
    bool     ReadOnly;  // true for input-only channels (above "P")
    QFrame  *frmDIO;

private:
    QCheckBox *DIO;

private slots:
    void DIOChange(bool);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // DIO_H
