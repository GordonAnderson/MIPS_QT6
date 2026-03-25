// =============================================================================
// properties.h
//
// Class declaration for Properties.
// See properties.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>
#include <QApplication>

namespace Ui { class Properties; }

void setSystemFontSize(int fs);

// -----------------------------------------------------------------------------
// Properties — persistent application settings dialog. Reads/writes a
// CSV-format Properties.mips file from the application or home directory.
// Call UpdateVars() after any UI change to sync the public member variables.
// -----------------------------------------------------------------------------
class Properties : public QDialog
{
    Q_OBJECT

public:
    explicit Properties(QWidget *parent = 0);
    ~Properties();
    void    UpdateVars(void);
    bool    Save(QString fileName);
    bool    Load(QString fileName);
    void    Log(QString Message);

    // Paths and file settings
    QString ApplicationPath;
    QString HomePath;
    QString DataFilePath;
    QString MethodesPath;
    QString ScriptPath;
    QString LoadControlPanel;
    QString FileName;
    QString LogFile;
    QString AMPSbaud;
    QString sysFontSize;

    // Behaviour flags
    bool SearchAMPS;
    bool AutoFileName;
    bool AutoConnect;
    bool AutoRestore;

    // Numeric settings
    int   MinMIPS;
    float UpdateSecs;

    // TCP/IP connection list
    QStringList MIPS_TCPIP;

private:
    Ui::Properties *ui;

private slots:
    void slotDataFilePath(void);
    void slotMethodesPath(void);
    void slotScriptPath(void);
    void slotLoadControlPanel(void);
    void slotClear(void);
    void slotOK(void);
    void slotLogFile(void);
};

#endif // PROPERTIES_H
