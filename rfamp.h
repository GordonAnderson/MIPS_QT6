// =============================================================================
// rfamp.h
//
// RF amplifier control panel widget for the MIPS host application.
//
// Implements a dockable control panel widget for an RF amplifier module.
// Supports RF settings (frequency, drive, setpoint), mass filter parameters
// (Ro, k, resolution, m/z), DC power supply control, and scripting access
// via ProcessCommand().
//
// Hardware:    MIPS RF amplifier module (commands: SRFAFREQ, SRFADRV,
//              SRFAENA, RFAQUPDATE, SDCPWR, etc.)
// Depends on:  comms.h, Utilities.h, ui_rfamp.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#ifndef RFAMP_H
#define RFAMP_H

#include <QDialog>
#include <QtCore/QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QStatusBar>
#include <QMessageBox>
#include <QObject>
#include <QApplication>
#include <QFileInfo>
#include <QKeyEvent>

#include "comms.h"

namespace Ui {
class RFamp;
}

class RFamp : public QDialog
{
    Q_OBJECT

public:
    explicit RFamp(QWidget *parent, QString name, QString MIPSname, int Module);
    ~RFamp();
    void Update(void);
    QString ProcessCommand(QString cmd);
    QString Report(void);
    bool SetValues(QString strVals);
    void Shutdown(void);
    void Restore(void);

    int     Channel;
    Comms   *comms;
    QString Title;
    QString MIPSnm;
    bool    isShutdown;
    bool    activeEnableState;

private:
    Ui::RFamp *ui;
    bool Updating;
    bool UpdateOff;
    QWidget *p;

private slots:
    void Updated(void);
    void slotUpdate(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // RFAMP_H
