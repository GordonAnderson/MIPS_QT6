// =============================================================================
// ccontrol.h
//
// Ccontrol — generic configurable control widget for the MIPS custom control
// panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Supports control types: LineEdit, CheckBox, Button, ComboBox.
// Each type has configurable Get, Set, and Readback MIPS commands.
//
// Depends on:  comms.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef CCONTROL_H
#define CCONTROL_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QRandomGenerator>

#include "comms.h"

// -----------------------------------------------------------------------------
// Ccontrol
//
// Implements a generic control that maps a named MIPS parameter to one of four
// UI types (LineEdit, CheckBox, Button, ComboBox). Get/Set/Readback MIPS
// commands are configured at construction time. Optional linear scaling and
// scripting hooks are supported.
//
// Syntax example (from control panel file):
//   Ccontrol,name,mips name,type,get command,set command,readback command,units,X,Y
// -----------------------------------------------------------------------------
class Ccontrol : public QWidget
{
    Q_OBJECT
signals:
    void change(QString scriptName);
public:
    Ccontrol(QWidget *parent, QString name, QString MIPSname, QString Type,
             QString Gcmd, QString Scmd, QString RBcmd, QString Units, int x, int y);
    void    Show(void);
    void    Update(void);
    QString Report(void);
    bool    SetValues(QString strVals);
    void    SetList(QString strOptions);
    void    Shutdown(void);
    void    Restore(void);
    QString ProcessCommand(QString cmd);

    QWidget     *p;
    QString      Title;
    int          X, Y;
    int          skipCount = 1;
    QString      MIPSnm;
    QString      Ctype;
    QString      Dtype;
    QString      GetCmd;
    QString      SetCmd;
    QString      ReadbackCmd;
    QString      UnitsText;
    QString      ActiveValue;
    QString      ShutdownValue;
    Comms       *comms;
    bool         isShutdown;
    float        Step;
    QFrame      *frmCc    = nullptr;
    QPushButton *pbButton  = nullptr;
    QString      scriptName = "";
    QString      scriptCall = "";

private:
    QLineEdit        *Vsp      = nullptr;
    QLineEdit        *Vrb;
    QCheckBox        *chkBox;
    QComboBox        *comboBox;
    QLabel           *labels[2] = {nullptr, nullptr};
    bool              Updating;
    bool              UpdateOff;
    bool              firstUpdate = true;
    bool              forceUpdate = false;
    QRandomGenerator  generator;
    int               updateCount;

private slots:
    void VspChange(void);
    void pbButtonPressed(void);
    void chkBoxStateChanged(Qt::CheckState);
    void comboBoxChanged(int);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // CCONTROL_H
