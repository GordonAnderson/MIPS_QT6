// =============================================================================
// program.h
//
// Class declaration for Program.
// See program.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef PROGRAM_H
#define PROGRAM_H

#include "ui_mips.h"
#include "comms.h"
#include "console.h"

#include <QDialog>
#include <QObject>
#include <QApplication>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QThread>
#include <QtSerialPort/QSerialPort>

// -----------------------------------------------------------------------------
// Program — firmware programmer for MIPS and RFmega boards. Uses the bundled
// bossac tool to erase, write, or read the SAM microcontroller flash via the
// Arduino 1200-baud bootloader activation sequence.
// -----------------------------------------------------------------------------
class Program : public QDialog
{
    Q_OBJECT

public:
    Program(Ui::MIPS *w, Comms *c, Console *con);

    Ui::MIPS *pui;
    Comms    *comms;
    Console  *console;
    QProcess  process;
    QString   appPath;   // directory containing bossac

private slots:
    void programMIPS(void);
    void programRFmega(void);
    void executeProgrammerCommand(QString cmd);
    void setBootloaderBootBit(void);
    void saveMIPSfirmware(void);
    void readProcessOutput(void);
};

#endif // PROGRAM_H
