// =============================================================================
// pse.h
//
// Pulse sequence editor (PSE) classes for the MIPS host application.
//
// Provides two classes:
//   psgPoint  — data model for a single time point in a pulse sequence.
//               Holds digital output states, DC bias voltages, and loop info.
//   pseDialog — dialog for editing a QList of psgPoint entries. Supports
//               insert, delete, ramp generation, and loop configuration.
//
// Depends on:  ui_mips.h, mips.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#ifndef PSE_H
#define PSE_H

#include <QDialog>
#include "ui_mips.h"
#include "mips.h"

static const int PSG_CHANNELS = 16; // Number of digital output and DC bias channels per PSG point

// psgPoint — a single time point in a pulse sequence. Stores the digital
// output state and DC bias voltage for each channel at this time point,
// plus optional loop control fields.
class psgPoint
{
public:
    QString Name;
    int     TimePoint;
    bool    DigitalO[PSG_CHANNELS];
    float   DCbias[PSG_CHANNELS];
    bool    Loop;
    QString LoopName;
    int     LoopCount;

    explicit psgPoint();
    void operator=(const psgPoint &P)
    {
        Name      = P.Name;
        TimePoint = P.TimePoint;
        Loop      = P.Loop;
        LoopName  = P.LoopName;
        LoopCount = P.LoopCount;
        for(int i = 0; i < PSG_CHANNELS; i++) { DigitalO[i] = P.DigitalO[i]; DCbias[i] = P.DCbias[i]; }
    }
};

QDataStream &operator<<(QDataStream &out, const psgPoint &point);
QDataStream &operator>>(QDataStream &in, psgPoint &point);

namespace Ui {
class pseDialog;
}

class pseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit pseDialog(QList<psgPoint*> *psg, QWidget *parent = 0);
    void UpdateDialog(psgPoint *point);
    ~pseDialog();

    Ui::MIPS *pui;

private slots:
    void on_DIO_checked();
    void on_pbNext_pressed();
    void on_pbPrevious_pressed();
    void on_DCBIAS_edited();
    void on_pbInsert_pressed();
    void on_pbDelete_pressed();
    void on_leName_textChanged(const QString &arg1);
    void on_leClocks_textChanged(const QString &arg1);
    void on_leCycles_textChanged(const QString &arg1);
    void on_chkLoop_clicked(bool checked);
    void on_comboLoop_currentIndexChanged(const QString &arg1);
    void InsertPulse(void);
    void MakeRamp(void);
    void RampCancel(void);

private:
    Ui::pseDialog *ui;
    int CurrentIndex;
    psgPoint *activePoint;
    QList<psgPoint*> *p;
};

#endif // PSE_H
