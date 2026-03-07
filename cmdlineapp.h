// =============================================================================
// cmdlineapp.h
//
// External command-line application dialog classes for the MIPS host app.
//
// Provides two classes:
//
//   cmdlineapp — a Qt dialog that wraps and interacts with an external
//                command-line application (typically a data acquisition
//                program). Uses QProcess for asynchronous I/O and QTimer
//                for command sequencing and timed prompt responses.
//
//                The parent sets the following strings to control interaction:
//                  ReadyMessage    — emits Ready() when detected in output
//                  InputRequest    — starts a 5 s auto-reply timer (sends 'N')
//                  ContinueMessage — emits AppCompleted() when detected
//                  fileName        — if set, saves terminal output on completion
//
//   extProcess  — lightweight wrapper that owns a cmdlineapp instance and
//                 exposes it via a scripting ProcessCommand() interface.
//
// Depends on:  ui_cmdlineapp.h, qcustomplot.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#ifndef CMDLINEAPP_H
#define CMDLINEAPP_H

#include <QDialog>
#include <QtCore/QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QStatusBar>
#include <QMessageBox>
#include <QObject>
#include <QApplication>
#include <QFileInfo>
#include <QProcess>
#include <QThread>
#include <QFileDialog>

namespace Ui {
class cmdlineapp;
}

class cmdlineapp : public QDialog
{
    Q_OBJECT

signals:
    void DialogClosed(void);
    void AppCompleted(void);
    void Ready(void);

public:
    explicit cmdlineapp(QWidget *parent = 0);
    ~cmdlineapp();
    void reject();
    QProcess    process;
    QString     title;
    QString     appPath;
    QString     appPathNoOptions;
    QString     fileName;
    QString     ReadyMessage;
    QString     ContinueMessage;
    QString     InputRequest;
    QStringList messages;
    void Execute(void);
    void Clear(void);
    void Dismiss(void);
    void AppendText(QString message);

private:
    Ui::cmdlineapp *ui;
    void setupPlot(QString mess);
    void plotDataPoint(QString mess);
    QTimer *responseTimer;
    QTimer *messageTimer;
    void AcquireFinishing(void);
    void sendString(QString mess);

private slots:
    void readProcessOutput(void);
    void readMessage(void);
    void AppFinished(void);
    void sendNO(void);
    void messageProcessor(void);
};

class extProcess : public QWidget
{
    Q_OBJECT

signals:
    void DialogClosed(QString name);
    void change(QString script);

public:
    extProcess(QWidget *parent, QString name, QString program);
    ~extProcess();
    QString ProcessCommand(QString cmd);
    QString name;

private:
    cmdlineapp *cla;
    QString    program;
    QString    status;
    QString    readyScript;
    QString    completeScript;

private slots:
    void slotExternalProcessReady(void);
    void slotExternalProcessCompleted(void);
    void slotExternalProcessDialogClosed(void);
};

#endif // CMDLINEAPP_H
