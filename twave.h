// =============================================================================
// twave.h
//
// Twave tab controller for the MIPS host application.
//
// Controls the travelling wave (Twave) ion guide tab. Supports single and
// dual channel operation, optional ARB compressor mode, frequency sweep,
// and external trigger configuration. Uses object name conventions (leSTW*,
// rbSTW*) for dynamic widget-to-command mapping.
//
// Hardware:    MIPS Twave module (commands: STWF, STWPV, STWDIR, STWCMODE,
//              STWCSW, STWSGO, STWSHLT, TWCTRG, SDTRIG*, etc.)
// Depends on:  comms.h, ui_mips.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#ifndef TWAVE_H
#define TWAVE_H

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

class Twave : public QDialog
{
    Q_OBJECT

public:
    Twave(Ui::MIPS *w, Comms *c);
    void Update(void);
    void Save(QString Filename);
    void Load(QString Filename);
    bool myEventFilter(QObject *obj, QEvent *event);

    Ui::MIPS *tui;
    Comms    *comms;
    int      NumChannels;
    bool     Compressor;

private:
    bool Updating;
    bool UpdateOff;

private slots:
    void Changed(void);
    void rbModeCompress(void);
    void rbModeNormal(void);
    void rbSwitchClose(void);
    void rbSwitchOpen(void);
    void pbUpdate(void);
    void pbForceTrigger(void);
    void rbTW1fwd(void);
    void rbTW1rev(void);
    void rbTW2fwd(void);
    void rbTW2rev(void);
    void pbTWsweepStart(void);
    void pbTWsweepStop(void);
    void SweepExtTrigger(void);
};

#endif // TWAVE_H
