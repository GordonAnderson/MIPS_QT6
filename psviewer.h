// =============================================================================
// psviewer.h
//
// Pulse sequence viewer dialog for the MIPS host application.
//
// Displays a QCustomPlot visualization of a pulse sequence generator (PSG)
// point list. Supports per-channel DC bias plotting with loop bracket
// annotations and interactive zoom/pan controls.
//
// Depends on:  pse.h, qcustomplot.h, ui_psviewer.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#ifndef PSVIEWER_H
#define PSVIEWER_H

#include "pse.h"

#include <QDialog>

namespace Ui {
class psviewer;
}

class psviewer : public QDialog
{
    Q_OBJECT

public:
    explicit psviewer(QList<psgPoint*> *P, QWidget *parent = 0);
    ~psviewer();
    void PlotDCBchannel(int channel);

private:
    Ui::psviewer *ui;
    QList<psgPoint*> *psg;
    float minY;
    float maxY;
    int   minCycles;
    int   maxCycles;
    int   plotnum;

private slots:
    void SetZoom(void);
    void PlotSelectedItem(void);
};

#endif // PSVIEWER_H
