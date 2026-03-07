// =============================================================================
// psg.h
//
// Pulse sequence generator (PSG) tab controller for the MIPS host application.
//
// Manages the PSG tab UI: building and downloading MIPS table commands,
// loading/saving pulse sequence files (.psg), and launching the pulse sequence
// editor (pseDialog) and viewer (psviewer).
//
// Hardware:    MIPS timing/table module (commands: STBLDAT, STBLCLK, STBLTRG,
//              SMOD, TBLSTRT, TBLABORT, GTBLADV, GTBLNUM, STBLVLT, etc.)
// Depends on:  comms.h, pse.h, psviewer.h, ui_mips.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#ifndef PSG_H
#define PSG_H

#include "ui_mips.h"
#include "mips.h"
#include "comms.h"
#include "pse.h"
#include "psviewer.h"

#include <QtCore/QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QStatusBar>
#include <QMessageBox>
#include <QObject>
#include <QApplication>
#include <QFileInfo>
#include <QFileDialog>

class PSG : public QDialog
{
    Q_OBJECT

public:
    PSG(Ui::MIPS *w, Comms *c);
    void Update(void);
    void Load(void);
    void Save(void);
    int Referenced(QList<psgPoint*> P, int i);
    QString BuildTableCommand(QList<psgPoint*> P);

    Ui::MIPS *pui;
    Comms    *comms;
    pseDialog *pse;
    psviewer  *psv;
    QList<psgPoint*> psg;

private slots:
    void on_pbDownload_pressed(void);
    void on_pbViewTable_pressed();
    void on_pbLoadFromFile_pressed();
    void on_pbCreateNew_pressed();
    void on_pbSaveToFile_pressed();
    void on_pbEditCurrent_pressed();
    void on_leSequenceNumber_textEdited();
    void on_chkAutoAdvance_clicked();
    void on_pbTrigger_pressed();
    void on_pbRead_pressed();
    void on_pbWrite_pressed();
    void on_pbVisPulseSequence_pressed();
    void on_pbExitTableMode_pressed();
};

#endif // PSG_H
