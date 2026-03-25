// =============================================================================
// faims.h
//
// FAIMS (Field Asymmetric Ion Mobility Spectrometry) module for the MIPS
// host application.
//
// Manages the FAIMS tab including RF/DC parameter updates, CV parking,
// linear scan, step scan, auto-tune, curtain supply control, and
// lock-mode switching. The PollLoop() method is called from the main
// update timer to service trigger detection and time-based CV parking.
//
// Depends on:  ui_mips.h, mips.h, comms.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#ifndef FAIMS_H
#define FAIMS_H

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
#include <QFileDialog>

// CV parking state machine states
enum FAIMSState { WAITING = 1, SCANNING = 2 };

class FAIMS : public QDialog
{
    Q_OBJECT

public:
    FAIMS(Ui::MIPS *w, Comms *c);
    void Update(void);
    void PollLoop(void);
    bool GetNextTarget(float et);
    void Save(QString Filename);
    void Load(QString Filename);
    void SetVersionOptions(void);
    int  getHeaderIndex(QString Name);
    QString getCSVtoken(QString Name, int index);
    void Log(QString Message);
    QString LogFileName;

    Ui::MIPS *fui;
    Comms    *comms;
    bool      CVparkingTriggered;
    bool      WaitingForLinearScanTrig;
    bool      WaitingForStepScanTrig;
    QStringList Parks;
    int       CurrentPoint;
    QString   TargetCompound;
    int       State;
    float     TargetRT;
    float     TargetWindow;
    float     TargetCV;
    float     TargetBias;
    // Target file variables
    QString     Header;
    QStringList Records;

private slots:
    void FAIMSUpdated(void);
    void FAIMSenable(void);
    void FAIMSscan(void);
    void FAIMSloadCSV(void);
    void FAIMSstartLinearScan(void);
    void FAIMSabortLinearScan(void);
    void FAIMSstartStepScan(void);
    void FAIMSabortStepScan(void);
    void FAIMSlockOff(void);
    void FAIMSlockOn(void);
    void FAIMSselectLogFile(void);
    void slotLinearTrigOut(void);
    void slotStepTrigOut(void);
    void slotFAIMSautoTune(void);
    void slotFAIMSautoTuneAbort(void);
    void slotFAIMSCurtianEna(void);
    void slotFAIMSCurtianInd(void);
    void slotFAIMSnegTune(void);
};

#endif // FAIMS_H
