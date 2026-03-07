// =============================================================================
// arb.h
//
// ARB (Arbitrary Waveform Generator) module classes for the MIPS host app.
//
// Provides two classes:
//
//   ARB        — manages the ARB tab on the main MIPS UI. Handles Twave and
//                ARB mode switching, channel/range setting, compressor controls,
//                waveform editing, and save/load of all ARB parameters.
//
//   ARBchannel — a dynamically created widget group representing one ARB
//                output channel. Supports frequency, amplitude, aux, offset,
//                waveform type, and direction control.
//
// Depends on:  ui_mips.h, mips.h, comms.h, arbwaveformedit.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#ifndef ARB_H
#define ARB_H

#include "ui_mips.h"
#include "mips.h"
#include "comms.h"
#include "arbwaveformedit.h"

#include <QtCore/QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QStatusBar>
#include <QMessageBox>
#include <QObject>
#include <QApplication>
#include <QFileInfo>

class Help;
class ARBwaveformEdit;

class ARB : public QDialog
{
    Q_OBJECT

public:
    ARB(Ui::MIPS *w, Comms *c);
    void SetNumberOfChannels(int num);
    void Update(void);
    void Save(QString Filename);
    void Load(QString Filename);
    Comms *comms;

private:
    Ui::MIPS        *aui;
    int              NumChannels;
    int              PPP;
    QString          LogString;
    Help            *LogedData;
    ARBwaveformEdit *ARBwfEdit;
    bool             Compressor;

private slots:
    void ARBUpdated(void);
    void ARBUpdatedParms(void);
    void SetARBchannel(void);
    void SetARBchannelRange(void);
    void ARBtrigger(void);
    void SetARBchannel_2(void);
    void SetARBchannelRange_2(void);
    void ARBtrigger_2(void);
    void ARBviewLog(void);
    void ARBclearLog(void);
    void ARBupdate(void);
    void ARBtabSelected(void);
    void ARBtypeSelected(void);
    void rbTWfwd(void);
    void rbTWrev(void);
    void ARBmoduleSelected(void);
    void EditARBwaveform(void);
    void ReadWaveform(void);
    // Compressor
    void rbModeCompress(void);
    void rbModeNormal(void);
    void rbSwitchClose(void);
    void rbSwitchOpen(void);
    void pbForceTrigger(void);
};

class ARBchannel : public QWidget
{
    Q_OBJECT

public:
    ARBchannel(QWidget *parent, QString name, QString MIPSname, int x, int y);
    void    Show(void);
    void    Update(void);
    QString Report(void);
    bool    SetValues(QString strVals);
    void    Shutdown(void);
    void    Restore(void);
    QString ProcessCommand(QString cmd);
    QWidget    *p;
    QString     Title;
    int         X, Y;
    QString     MIPSnm;
    int         Channel;
    int         PPP;
    Comms      *comms;
    QStatusBar *statusBar;
    bool        isShutdown;
    QGroupBox  *gbARB;

private:
    QLineEdit    *leSWFREQ;
    QLineEdit    *leSWFVRNG;
    QLineEdit    *leSWFVAUX;
    QLineEdit    *leSWFVOFF;
    QRadioButton *SWFDIR_FWD;
    QRadioButton *SWFDIR_REV;
    QComboBox    *Waveform;
    QPushButton  *EditWaveform;
    QLabel       *labels[10];
    ARBwaveformEdit *ARBwfEdit;
    QString      activeVRNG;
    QString      activeVAUX;
    QString      activeVOFF;
    bool         Updating;
    bool         UpdateOff;

private slots:
    void leChange(void);
    void rbChange(void);
    void wfChange(void);
    void wfEdit(void);
    void ReadWaveform(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // ARB_H
