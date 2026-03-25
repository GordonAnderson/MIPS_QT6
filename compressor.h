// =============================================================================
// compressor.h
//
// Compressor dialog class for the MIPS host application.
//
// Implements the ARB compressor control dialog. Supports dynamic widget
// binding via object name conventions (leS*, chk*, rb*, lel*) to map UI
// controls directly to MIPS firmware commands without explicit per-widget
// signal connections.
//
// Hardware:    MIPS ARB compressor module (commands: TARBTRG, SARBCORDER, etc.)
// Depends on:  comms.h, ui_compressor.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <QDialog>
#include <QtCore/QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QStatusBar>
#include <QMessageBox>
#include <QObject>
#include <QApplication>
#include <QFileInfo>
#include <QKeyEvent>
#include <QCheckBox>
#include <QComboBox>

#include "comms.h"

namespace Ui {
class Compressor;
}

class Compressor : public QDialog
{
    Q_OBJECT

public:
    explicit Compressor(QWidget *parent, QString name, QString MIPSname);
    ~Compressor();
    void Update(void);
    QString Report(void);
    bool SetValues(QString strVals);
    QString ProcessCommand(QString cmd);

    Comms   *comms;
    QString Title;
    QString MIPSnm;

private:
    Ui::Compressor *ui;
    bool Updating;
    bool UpdateOff;

private slots:
    void Updated(void);
    void pbARBforceTriggerSlot(void);
};

#endif // COMPRESSOR_H
