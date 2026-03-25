// =============================================================================
// arbwaveformedit.h
//
// ARB waveform editor dialog for the MIPS host application.
//
// Provides a dialog for editing, generating, and previewing ARB waveforms.
// Supports up/down square wave generation, sine wave generation, manual text
// entry, inversion, and live QCustomPlot preview. Emits WaveformReady() when
// the user accepts the dialog.
//
// Waveform values are in the range [-100, 100] representing percent amplitude.
// Points-per-period (PPP) is clamped to [ARB_PPP_MIN, ARB_PPP_MAX].
//
// Depends on:  qcustomplot.h, ui_arbwaveformedit.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#ifndef ARBWAVEFORMEDIT_H
#define ARBWAVEFORMEDIT_H

#include <QDialog>

namespace Ui {
class ARBwaveformEdit;
}

class ARBwaveformEdit : public QDialog
{
    Q_OBJECT

signals:
    void WaveformReady(void);

public:
    explicit ARBwaveformEdit(QWidget *parent, int ppp);
    ~ARBwaveformEdit();
    void SetWaveform(int *wf);
    void GetWaveform(int *wf);

private:
    Ui::ARBwaveformEdit *ui;
    int Waveform[32];
    int PPP;

private slots:
    void GenerateUpDown(void);
    void GenerateSine(void);
    void PlotData(void);
    void InvertData(void);
    void WaveformAccepted(void);
};

#endif // ARBWAVEFORMEDIT_H
