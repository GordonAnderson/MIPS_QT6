// =============================================================================
// devicescriptdialog.h
//
// DeviceScriptDialog -- Tools -> Device Scripts. Write a GAACE_Script program,
// check it, and load it into a script slot on a connected device; also shows
// the device's script limits and syscall table, and starts/stops/queries slots.
//
// This is not the JavaScript console (scriptingconsole.*): that runs scripts on
// the PC, these run on the device and keep running with no PC attached.
//
// How a Send works: the dialog reads the device's syscall table (GSYSCALLS) and
// command list (GCMDS), hands them and the source to the compiler (GscRunner,
// which runs gsc.py), and uploads the result stamped with the table's hash so
// the device refuses a script built for different syscalls. Compiling therefore
// needs a connected device; editing and saving source does not. A precompiled
// .hex file can still be sent from the "Precompiled hex" section.
//
// Only serial-connected systems from MIPS::Systems are offered. The dialog
// keeps system *names*, not Comms pointers, and looks the Comms up again for
// every operation, because MIPS::FindAllMIPSsystems() deletes and recreates
// them. The source is what is saved; bytecode is rebuilt on every Check/Send.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     November 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef DEVICESCRIPTDIALOG_H
#define DEVICESCRIPTDIALOG_H

#include <QDialog>
#include <QList>

#include "devicescriptproto.h"

class CodeEditor;
class Comms;
class Properties;
class QCloseEvent;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPlainTextEdit;
class QProgressBar;
class QPushButton;
class QSpinBox;
class QWidget;

class DeviceScriptDialog : public QDialog
{
    Q_OBJECT

public:
    DeviceScriptDialog(QWidget *parent, const QList<Comms *> *systems, Properties *prop);

public slots:
    void reject() override;

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void refreshSystems();
    void onSystemChanged();

    void newScript();
    void openScript();
    bool saveScript();
    bool saveScriptAs();
    void onSourceChanged();
    void onDiagnosticClicked(QListWidgetItem *item);

    void check();
    void send();
    void browseHex();
    void useDeviceHash();
    void sendHex();

    void refreshStatus();
    void startSlot();
    void stopSlot();

private:
    Comms *current() const;
    void   log(const QString &text);
    void   setScriptingAvailable(bool available);
    void   setBusy(bool busy);
    void   updateTitle();
    bool   maybeSave();
    bool   writeFile(const QString &path);
    QString startDirectory() const;

    // Compiles the editor's source against the selected device's tables and
    // shows the errors and warnings. True if it compiled.
    bool   compileSource(DeviceScriptProto::CompileResult &result);
    void   showDiagnostics(const DeviceScriptProto::CompileResult &result);
    // Uploads `payload` to the selected slot (asks before replacing a script).
    bool   uploadPayload(Comms *comms, const QByteArray &payload, quint32 hash);

    const QList<Comms *> *systems_;
    Properties           *properties_;

    bool                                  scripting_ = false;
    DeviceScriptProto::Limits             limits_;
    DeviceScriptProto::SyscallTable       table_;
    bool                                  haveTable_ = false;
    QList<DeviceScriptProto::CommandEntry> commands_;
    bool                                  haveCommands_ = false;

    QString scriptPath_;        //!< the .gs file being edited; empty if never saved
    QString lastDirectory_;

    QComboBox      *systemCombo_;
    QPushButton    *refreshButton_;
    QSpinBox       *slotSpin_;
    QLabel         *limitsLabel_;
    QPlainTextEdit *syscallView_;
    QLabel         *fileLabel_;
    CodeEditor     *editor_;
    QListWidget    *diagnostics_;
    QPushButton    *newButton_;
    QPushButton    *openButton_;
    QPushButton    *saveButton_;
    QPushButton    *saveAsButton_;
    QPushButton    *checkButton_;
    QPushButton    *sendButton_;
    QPushButton    *statusButton_;
    QPushButton    *startButton_;
    QPushButton    *stopButton_;
    QGroupBox      *hexGroup_;
    QWidget        *hexPanel_;
    QLineEdit      *hexFileEdit_;
    QPushButton    *hexBrowseButton_;
    QLineEdit      *hashEdit_;
    QPushButton    *useHashButton_;
    QPushButton    *sendHexButton_;
    QProgressBar   *progress_;
    QPlainTextEdit *logView_;
};

#endif // DEVICESCRIPTDIALOG_H
