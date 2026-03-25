// =============================================================================
// dcbias.h
//
// Class declarations for DCbias, DCBchannel, DCBoffset, and DCBenable.
// See dcbias.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef DCBIAS
#define DCBIAS

#include "ui_mips.h"
#include "mips.h"
#include "comms.h"

#include <QtCore/QtGlobal>
#include <QObject>
#include <QApplication>

// -----------------------------------------------------------------------------
// DCbias — control panel for the built-in 8/16/24-channel DC bias board.
// Handles grouped voltage adjustment, power enable, and settings Save/Load.
// -----------------------------------------------------------------------------
class DCbias : public QDialog
{
    Q_OBJECT

public:
    DCbias(Ui::MIPS *w, Comms *c);
    void SetNumberOfChannels(int num);
    void Update(void);
    void Save(QString Filename);
    void Load(QString Filename);
    void ApplyDelta(QString GrpName, float change);

    Ui::MIPS  *dui;
    Comms     *comms;
    int        NumChannels;
    QLineEdit *selectedLineEdit;

private:
    bool Updating;   // true while an update sweep is in progress — blocks re-entrant edits
    bool UpdateOff;  // true while the user is editing a value — suppresses background updates

private slots:
    void DCbiasUpdated(void);
    void DCbiasPower(void);
    void UpdateDCbias(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

// -----------------------------------------------------------------------------
// DCBchannel — draggable widget showing setpoint (Vsp) and readback (Vrb) for
// one DC bias channel. Readback background: green = in tolerance, red = out.
// Supports linked-channel grouping via the DCBs list and LinkEnable flag.
// Supports Shutdown/Restore for safe power sequencing.
// -----------------------------------------------------------------------------
class DCBchannel : public QWidget
{
    Q_OBJECT
public:
    DCBchannel(QWidget *parent, QString name, QString MIPSname, int x, int y);
    void    Show(void);
    void    Update(QString sVals = "");
    QString Report(void);
    bool    SetValues(QString strVals);
    void    Shutdown(void);
    void    Restore(void);
    QString ProcessCommand(QString cmd);

    QWidget            *p;
    QString             Title;
    int                 X, Y;
    QString             MIPSnm;
    int                 Channel;      // MIPS channel number (1-based)
    Comms              *comms;
    QLineEdit          *Vsp;          // setpoint line edit (public for group linking)
    bool                LinkEnable;   // true when this channel is part of a linked group
    QList<DCBchannel*>  DCBs;         // linked channels updated together
    float               CurrentVsp;   // last applied setpoint — used to compute group delta
    bool                isShutdown;   // true when driven to 0 V by Shutdown()
    QFrame             *frmDCB;

private:
    QLineEdit *Vrb;           // readback line edit (read-only)
    QLabel    *labels[2];
    QString    activeVoltage; // setpoint saved by Shutdown(), restored by Restore()
    bool       Updating;
    bool       UpdateOff;
    bool       VspEdited;     // set true when user edits Vsp; prevents overwrite on next poll

private slots:
    void VspChange(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

// -----------------------------------------------------------------------------
// DCBoffset — draggable widget for the per-board voltage offset/range control.
// Sends SDCBOF/GDCBOF commands to the MIPS firmware.
// -----------------------------------------------------------------------------
class DCBoffset : public QWidget
{
    Q_OBJECT
public:
    DCBoffset(QWidget *parent, QString name, QString MIPSname, int x, int y);
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
    QFrame  *frmDCBO;

private:
    QLineEdit *Voff;
    QLabel    *labels[2];

private slots:
    void VoffChange(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

// -----------------------------------------------------------------------------
// DCBenable — draggable checkbox that sends SDCPWR ON/OFF to the MIPS firmware.
// Supports Shutdown/Restore for power sequencing.
// -----------------------------------------------------------------------------
class DCBenable : public QWidget
{
    Q_OBJECT
public:
    DCBenable(QWidget *parent, QString name, QString MIPSname, int x, int y);
    void    Show(void);
    void    Update(void);
    QString Report(void);
    QString ProcessCommand(QString cmd);
    bool    SetValues(QString strVals);
    void    Shutdown(void);
    void    Restore(void);

    QWidget *p;
    QString  Title;
    int      X, Y;
    QString  MIPSnm;
    Comms   *comms;
    bool     isShutdown;
    QFrame  *frmDCBena;

private:
    QCheckBox *DCBena;
    bool       activeEnableState; // saved enable state for Shutdown/Restore

private slots:
    void DCBenaChange(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // DCBIAS
