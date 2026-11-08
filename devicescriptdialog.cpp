// =============================================================================
// devicescriptdialog.cpp
//
// DeviceScriptDialog. See devicescriptdialog.h.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     November 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "devicescriptdialog.h"
#include "codeeditor.h"
#include "comms.h"
#include "devicescript.h"
#include "gscrunner.h"
#include "properties.h"

#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

using namespace DeviceScriptProto;

static QString hex8(quint32 v)
{
    return QString("%1").arg(v, 8, 16, QChar('0')).toUpper();
}

DeviceScriptDialog::DeviceScriptDialog(QWidget *parent, const QList<Comms *> *systems, Properties *prop)
    : QDialog(parent), systems_(systems), properties_(prop)
{
    resize(780, 820);

    systemCombo_   = new QComboBox;
    systemCombo_->setObjectName("systemCombo");
    refreshButton_ = new QPushButton("Refresh");
    slotSpin_      = new QSpinBox;
    slotSpin_->setRange(0, 0);
    limitsLabel_   = new QLabel;
    limitsLabel_->setWordWrap(true);
    syscallView_   = new QPlainTextEdit;
    syscallView_->setObjectName("syscallView");
    syscallView_->setReadOnly(true);
    syscallView_->setMaximumHeight(90);

    fileLabel_    = new QLabel;
    newButton_    = new QPushButton("New");
    openButton_   = new QPushButton("Open...");
    saveButton_   = new QPushButton("Save");
    saveAsButton_ = new QPushButton("Save As...");
    editor_       = new CodeEditor;
    editor_->setObjectName("sourceEditor");
    editor_->setPlaceholderText("Write a GAACE_Script program here, or open a .gs file.\n\n"
                                "    cmd(\"SDRV\", 23.5);        // run a command on this device\n"
                                "    set_volts(read_adc());    // call a syscall the device offers (listed above)");
    diagnostics_  = new QListWidget;
    diagnostics_->setObjectName("diagnostics");
    diagnostics_->setMaximumHeight(110);

    checkButton_  = new QPushButton("Check");
    sendButton_   = new QPushButton("Send to device");
    statusButton_ = new QPushButton("Slot status");
    startButton_  = new QPushButton("Start slot");
    stopButton_   = new QPushButton("Stop slot");

    hexGroup_ = new QGroupBox("Send a precompiled .hex file instead");
    hexGroup_->setCheckable(true);
    hexGroup_->setChecked(false);
    hexPanel_        = new QWidget;
    hexFileEdit_     = new QLineEdit;
    hexFileEdit_->setPlaceholderText("The .hex file gsc.py --scriptload --format hex writes");
    hexBrowseButton_ = new QPushButton("Browse...");
    hashEdit_        = new QLineEdit;
    hashEdit_->setPlaceholderText("optional, 8 hex digits");
    hashEdit_->setMaximumWidth(170);
    useHashButton_   = new QPushButton("Use device table hash");
    sendHexButton_   = new QPushButton("Send hex");

    progress_ = new QProgressBar;
    progress_->setVisible(false);
    logView_  = new QPlainTextEdit;
    logView_->setObjectName("logView");
    logView_->setReadOnly(true);
    logView_->setMaximumBlockCount(2000);

    QGridLayout *top = new QGridLayout;
    top->addWidget(new QLabel("System:"), 0, 0);
    top->addWidget(systemCombo_, 0, 1);
    top->addWidget(refreshButton_, 0, 2);
    top->addWidget(new QLabel("Slot:"), 1, 0);
    top->addWidget(slotSpin_, 1, 1, Qt::AlignLeft);
    top->setColumnStretch(1, 1);

    QHBoxLayout *fileRow = new QHBoxLayout;
    fileRow->addWidget(newButton_);
    fileRow->addWidget(openButton_);
    fileRow->addWidget(saveButton_);
    fileRow->addWidget(saveAsButton_);
    fileRow->addSpacing(12);
    fileRow->addWidget(fileLabel_, 1);

    QHBoxLayout *actions = new QHBoxLayout;
    actions->addWidget(checkButton_);
    actions->addWidget(sendButton_);
    actions->addSpacing(12);
    actions->addWidget(statusButton_);
    actions->addWidget(startButton_);
    actions->addWidget(stopButton_);
    actions->addStretch(1);

    QGridLayout *hexLayout = new QGridLayout(hexPanel_);
    hexLayout->setContentsMargins(0, 0, 0, 0);
    hexLayout->addWidget(new QLabel("File:"), 0, 0);
    hexLayout->addWidget(hexFileEdit_, 0, 1);
    hexLayout->addWidget(hexBrowseButton_, 0, 2);
    hexLayout->addWidget(new QLabel("Syscall table hash:"), 1, 0);
    QHBoxLayout *hashRow = new QHBoxLayout;
    hashRow->addWidget(hashEdit_);
    hashRow->addWidget(useHashButton_);
    hashRow->addStretch(1);
    hexLayout->addLayout(hashRow, 1, 1, 1, 2);
    hexLayout->addWidget(sendHexButton_, 2, 1, Qt::AlignLeft);
    hexLayout->setColumnStretch(1, 1);
    QVBoxLayout *hexOuter = new QVBoxLayout(hexGroup_);
    hexOuter->addWidget(hexPanel_);
    hexPanel_->setVisible(false);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(top);
    layout->addWidget(limitsLabel_);
    layout->addWidget(new QLabel("Syscalls this device offers:"));
    layout->addWidget(syscallView_);
    layout->addLayout(fileRow);
    layout->addWidget(editor_, 3);
    layout->addLayout(actions);
    layout->addWidget(diagnostics_);
    layout->addWidget(hexGroup_);
    layout->addWidget(progress_);
    layout->addWidget(logView_, 1);

    connect(refreshButton_,  &QPushButton::clicked, this, &DeviceScriptDialog::refreshSystems);
    connect(systemCombo_,    &QComboBox::currentIndexChanged, this, [this](int) { onSystemChanged(); });
    connect(newButton_,      &QPushButton::clicked, this, &DeviceScriptDialog::newScript);
    connect(openButton_,     &QPushButton::clicked, this, &DeviceScriptDialog::openScript);
    connect(saveButton_,     &QPushButton::clicked, this, &DeviceScriptDialog::saveScript);
    connect(saveAsButton_,   &QPushButton::clicked, this, &DeviceScriptDialog::saveScriptAs);
    connect(editor_,         &QPlainTextEdit::textChanged, this, &DeviceScriptDialog::onSourceChanged);
    connect(editor_->document(), &QTextDocument::modificationChanged, this, [this](bool) { updateTitle(); });
    connect(diagnostics_,    &QListWidget::itemClicked, this, &DeviceScriptDialog::onDiagnosticClicked);
    connect(checkButton_,    &QPushButton::clicked, this, &DeviceScriptDialog::check);
    connect(sendButton_,     &QPushButton::clicked, this, &DeviceScriptDialog::send);
    connect(hexGroup_,       &QGroupBox::toggled, hexPanel_, &QWidget::setVisible);
    connect(hexBrowseButton_, &QPushButton::clicked, this, &DeviceScriptDialog::browseHex);
    connect(useHashButton_,  &QPushButton::clicked, this, &DeviceScriptDialog::useDeviceHash);
    connect(sendHexButton_,  &QPushButton::clicked, this, &DeviceScriptDialog::sendHex);
    connect(statusButton_,   &QPushButton::clicked, this, &DeviceScriptDialog::refreshStatus);
    connect(startButton_,    &QPushButton::clicked, this, &DeviceScriptDialog::startSlot);
    connect(stopButton_,     &QPushButton::clicked, this, &DeviceScriptDialog::stopSlot);

    if(properties_ != nullptr) lastDirectory_ = properties_->ScriptPath;
    updateTitle();
    setScriptingAvailable(false);
    log(GscRunner::describeSetup());
}

void DeviceScriptDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    refreshSystems();
}

// Closing the dialog (X, Esc) must not lose unsaved source.
void DeviceScriptDialog::closeEvent(QCloseEvent *event)
{
    if(maybeSave()) event->accept();
    else            event->ignore();
}

void DeviceScriptDialog::reject()
{
    if(maybeSave()) QDialog::reject();
}

// The Comms for the selected system, or null if it is gone or disconnected.
// Looked up by name every time -- never cached (see the header).
Comms *DeviceScriptDialog::current() const
{
    if(systems_ == nullptr) return nullptr;
    const QString name = systemCombo_->currentText();
    for(Comms *c : *systems_)
        if(c != nullptr && c->MIPSname == name && c->isConnected()) return c;
    return nullptr;
}

void DeviceScriptDialog::log(const QString &text)
{
    logView_->appendPlainText(QDateTime::currentDateTime().toString("hh:mm:ss  ") + text);
}

void DeviceScriptDialog::setScriptingAvailable(bool available)
{
    scripting_ = available;
    for(QWidget *w : QList<QWidget *>{ slotSpin_, checkButton_, sendButton_, statusButton_,
                                        startButton_, stopButton_, useHashButton_, sendHexButton_ })
        w->setEnabled(available);
}

void DeviceScriptDialog::setBusy(bool busy)
{
    const QList<QWidget *> alwaysUsable = { systemCombo_, refreshButton_, newButton_, openButton_,
                                            saveButton_, saveAsButton_, hexBrowseButton_ };
    const QList<QWidget *> needsDevice  = { slotSpin_, checkButton_, sendButton_, statusButton_,
                                            startButton_, stopButton_, useHashButton_, sendHexButton_ };
    for(QWidget *w : alwaysUsable) w->setEnabled(!busy);
    for(QWidget *w : needsDevice)  w->setEnabled(!busy && scripting_);
    if(busy) QApplication::setOverrideCursor(Qt::WaitCursor);
    else     QApplication::restoreOverrideCursor();
}

void DeviceScriptDialog::updateTitle()
{
    const QString name = scriptPath_.isEmpty() ? QString("untitled") : QFileInfo(scriptPath_).fileName();
    const QString mark = editor_->document()->isModified() ? "*" : "";
    setWindowTitle("Device Scripts - " + name + mark);
    fileLabel_->setText(scriptPath_.isEmpty() ? QString("(not saved)") : scriptPath_);
}

// ---------------------------------------------------------------------------
// The device
// ---------------------------------------------------------------------------

// Repopulate the system list from MIPS::Systems, keeping the selection if the
// same system is still there.
void DeviceScriptDialog::refreshSystems()
{
    const QString previous = systemCombo_->currentText();
    systemCombo_->blockSignals(true);
    systemCombo_->clear();
    if(systems_ != nullptr)
        for(Comms *c : *systems_)
            if(c != nullptr && c->isConnected()) systemCombo_->addItem(c->MIPSname);
    const int idx = systemCombo_->findText(previous);
    if(idx >= 0) systemCombo_->setCurrentIndex(idx);
    systemCombo_->blockSignals(false);
    onSystemChanged();
}

// Ask the selected system what it supports and show it. The syscall table and
// command list are kept in memory for this selection only; nothing is stored.
void DeviceScriptDialog::onSystemChanged()
{
    haveTable_ = false;
    haveCommands_ = false;
    commands_.clear();
    syscallView_->clear();
    Comms *comms = current();
    if(comms == nullptr)
    {
        limitsLabel_->setText(systemCombo_->count() == 0 ? "No system is connected."
                                                          : "That system is not connected.");
        setScriptingAvailable(false);
        return;
    }

    setBusy(true);
    DeviceScriptClient::Result r = DeviceScriptClient::queryLimits(comms, limits_);
    if(!r.ok)
    {
        limitsLabel_->setText(r.message);
        setScriptingAvailable(false);
        setBusy(false);
        return;
    }
    slotSpin_->setRange(0, qMax(0, limits_.slotCount - 1));
    limitsLabel_->setText(QString("%1 script slots, %2 bytes per slot (length prefix + code + string pool), "
                                  "stack %3, %4 variables")
                          .arg(limits_.slotCount).arg(limits_.maxCodeLen)
                          .arg(limits_.stackSize).arg(limits_.varSlots));
    setScriptingAvailable(true);

    r = DeviceScriptClient::querySyscalls(comms, table_);
    if(r.ok)
    {
        haveTable_ = true;
        QStringList text;
        text << QString("%1 syscalls, table hash %2").arg(table_.entries.count()).arg(hex8(table_.hash));
        for(int i = 0; i < table_.entries.count(); i++)
        {
            const SyscallEntry &e = table_.entries.at(i);
            QStringList args;
            for(int a = 0; a < e.argc; a++) args << QString("a%1").arg(a);
            text << QString("  %1: %2 %3(%4)").arg(i).arg(e.returnsFloat ? "float" : "int")
                        .arg(e.name).arg(args.join(", "));
        }
        syscallView_->setPlainText(text.join("\n"));
    }
    else
    {
        syscallView_->setPlainText(r.message + "\nScripts must then declare the syscalls they use.");
    }

    r = DeviceScriptClient::queryCommands(comms, commands_);
    haveCommands_ = r.ok;
    if(!r.ok) log("No command list from the device, so command names will not be checked: " + r.message);
    setBusy(false);
}

void DeviceScriptDialog::refreshStatus()
{
    Comms *comms = current();
    if(comms == nullptr) { log("Not connected."); return; }
    setBusy(true);
    SlotStatus st;
    const DeviceScriptClient::Result r = DeviceScriptClient::slotStatus(comms, slotSpin_->value(), st);
    setBusy(false);
    if(!r.ok) { log(r.message); return; }
    log(QString("Slot %1: %2, last VM status %3").arg(slotSpin_->value())
            .arg(st.loaded ? "script loaded" : "empty").arg(st.lastStatus));
}

void DeviceScriptDialog::startSlot()
{
    Comms *comms = current();
    if(comms == nullptr) { log("Not connected."); return; }
    setBusy(true);
    const DeviceScriptClient::Result r = DeviceScriptClient::setSlotRunning(comms, slotSpin_->value(), true);
    setBusy(false);
    log(r.ok ? QString("Slot %1 started.").arg(slotSpin_->value()) : r.message);
}

void DeviceScriptDialog::stopSlot()
{
    Comms *comms = current();
    if(comms == nullptr) { log("Not connected."); return; }
    setBusy(true);
    const DeviceScriptClient::Result r = DeviceScriptClient::setSlotRunning(comms, slotSpin_->value(), false);
    setBusy(false);
    log(r.ok ? QString("Slot %1 stopped.").arg(slotSpin_->value()) : r.message);
}

// Uploads to the selected slot, asking first if it already holds a script.
bool DeviceScriptDialog::uploadPayload(Comms *comms, const QByteArray &payload, quint32 hash)
{
    const int slot = slotSpin_->value();
    if(payload.size() > limits_.maxCodeLen)
    {
        log(QString("The script is %1 bytes but a slot holds %2 (length prefix + code + string pool). "
                    "Shorten the script, or build the firmware with a larger GAACE_SCRIPT_MAX_CODE_LEN.")
                .arg(payload.size()).arg(limits_.maxCodeLen));
        return false;
    }

    SlotStatus before;
    const DeviceScriptClient::Result beforeResult = DeviceScriptClient::slotStatus(comms, slot, before);
    if(!beforeResult.ok)
        log("Could not tell whether slot " + QString::number(slot) + " already has a script (" +
            beforeResult.message + "); sending without asking.");
    else if(before.loaded)
    {
        if(QMessageBox::question(this, "Replace script?",
                                 QString("Slot %1 on %2 already has a script. Replace it?")
                                     .arg(slot).arg(systemCombo_->currentText())) != QMessageBox::Yes)
        {
            log("Cancelled.");
            return false;
        }
    }

    setBusy(true);
    progress_->setRange(0, 1);
    progress_->setValue(0);
    progress_->setVisible(true);
    const DeviceScriptClient::Result r = DeviceScriptClient::upload(
        comms, slot, payload, hash,
        [this](int done, int total)
        {
            progress_->setRange(0, total);
            progress_->setValue(done);
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        });
    progress_->setVisible(false);
    setBusy(false);

    if(!r.ok) { log("Send failed: " + r.message); return false; }
    log(QString("Sent %1 bytes to slot %2 on %3%4. The slot is stopped; use \"Start slot\" to run it.")
            .arg(payload.size()).arg(slot).arg(systemCombo_->currentText())
            .arg(hash ? QString(" (checked against table hash %1)").arg(hex8(hash)) : QString()));
    refreshStatus();
    return true;
}

// ---------------------------------------------------------------------------
// The source file
// ---------------------------------------------------------------------------

QString DeviceScriptDialog::startDirectory() const
{
    if(!scriptPath_.isEmpty()) return QFileInfo(scriptPath_).absolutePath();
    return lastDirectory_;
}

bool DeviceScriptDialog::maybeSave()
{
    if(!editor_->document()->isModified()) return true;
    const QMessageBox::StandardButton b = QMessageBox::question(
        this, "Unsaved script",
        QString("Save changes to %1?").arg(scriptPath_.isEmpty() ? QString("the script")
                                                                 : QFileInfo(scriptPath_).fileName()),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
    if(b == QMessageBox::Cancel) return false;
    if(b == QMessageBox::Save)   return saveScript();
    return true;
}

bool DeviceScriptDialog::writeFile(const QString &path)
{
    QFile f(path);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Save failed", "Cannot write " + path + ": " + f.errorString());
        return false;
    }
    f.write(editor_->toPlainText().toUtf8());
    f.close();
    scriptPath_ = path;
    lastDirectory_ = QFileInfo(path).absolutePath();
    editor_->document()->setModified(false);
    updateTitle();
    log("Saved " + path);
    return true;
}

void DeviceScriptDialog::newScript()
{
    if(!maybeSave()) return;
    editor_->clear();
    editor_->document()->setModified(false);
    scriptPath_.clear();
    diagnostics_->clear();
    updateTitle();
}

void DeviceScriptDialog::openScript()
{
    if(!maybeSave()) return;
    const QString path = QFileDialog::getOpenFileName(this, "Open a GAACE_Script program",
                                                      startDirectory(),
                                                      "GAACE_Script (*.gs);;All files (*)");
    if(path.isEmpty()) return;
    QFile f(path);
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Open failed", "Cannot read " + path + ": " + f.errorString());
        return;
    }
    editor_->setPlainText(QString::fromUtf8(f.readAll()));
    editor_->document()->setModified(false);
    scriptPath_ = path;
    lastDirectory_ = QFileInfo(path).absolutePath();
    diagnostics_->clear();
    updateTitle();
    log("Opened " + path);
}

bool DeviceScriptDialog::saveScript()
{
    if(scriptPath_.isEmpty()) return saveScriptAs();
    return writeFile(scriptPath_);
}

bool DeviceScriptDialog::saveScriptAs()
{
    QString path = QFileDialog::getSaveFileName(this, "Save the script",
                                                scriptPath_.isEmpty() ? startDirectory() : scriptPath_,
                                                "GAACE_Script (*.gs);;All files (*)");
    if(path.isEmpty()) return false;
    if(QFileInfo(path).suffix().isEmpty()) path += ".gs";
    return writeFile(path);
}

// Editing invalidates the last compile's messages: their line numbers no
// longer point at the right text.
void DeviceScriptDialog::onSourceChanged()
{
    editor_->clearProblem();
    diagnostics_->clear();
}

// ---------------------------------------------------------------------------
// Compile and send
// ---------------------------------------------------------------------------

void DeviceScriptDialog::showDiagnostics(const CompileResult &result)
{
    diagnostics_->clear();
    editor_->clearProblem();

    auto add = [this](const QString &kind, const Diagnostic &d, const QColor &color)
    {
        const QString where = d.line > 0 ? QString("line %1, column %2: ").arg(d.line).arg(d.column) : QString();
        QListWidgetItem *item = new QListWidgetItem(kind + where + d.message);
        item->setForeground(color);
        item->setData(Qt::UserRole, d.line);
        item->setData(Qt::UserRole + 1, d.column);
        diagnostics_->addItem(item);
    };

    if(!result.failure.isEmpty())
    {
        Diagnostic d;
        d.message = result.failure;
        add("Compiler problem: ", d, QColor(Qt::red));
    }
    else if(!result.ok)
    {
        add("Error: ", result.error, QColor(Qt::red));
        if(result.error.line > 0) editor_->showProblem(result.error.line, result.error.column);
    }
    for(const Diagnostic &w : result.warnings) add("Warning: ", w, QColor(200, 120, 0));
}

void DeviceScriptDialog::onDiagnosticClicked(QListWidgetItem *item)
{
    const int line = item->data(Qt::UserRole).toInt();
    if(line > 0)
    {
        editor_->showProblem(line, item->data(Qt::UserRole + 1).toInt());
        editor_->setFocus();
    }
}

bool DeviceScriptDialog::compileSource(CompileResult &result)
{
    const QString source = editor_->toPlainText();
    if(source.trimmed().isEmpty())
    {
        log("There is no script to compile.");
        return false;
    }
    SyscallTable table = table_;
    const QByteArray syscalls = haveTable_ ? syscallTableJson(table) : QByteArray();
    const QByteArray commands = haveCommands_ ? commandListJson(commands_) : QByteArray();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    result = GscRunner::compile(source, syscalls, commands);
    QApplication::restoreOverrideCursor();

    showDiagnostics(result);
    if(!result.failure.isEmpty()) { log(result.failure); return false; }
    if(!result.ok)
    {
        log("Not compiled: " + (result.error.line > 0 ? QString("line %1, column %2: ")
                                    .arg(result.error.line).arg(result.error.column) : QString())
            + result.error.message);
        return false;
    }
    return true;
}

void DeviceScriptDialog::check()
{
    CompileResult result;
    if(!compileSource(result)) return;
    log(QString("Compiled: %1 bytes (code %2, strings %3)%4.")
            .arg(result.payload.size()).arg(result.codeLen).arg(result.poolLen)
            .arg(result.warnings.isEmpty() ? QString()
                                           : QString(", %1 warning(s)").arg(result.warnings.count())));
}

void DeviceScriptDialog::send()
{
    Comms *comms = current();
    if(comms == nullptr) { log("Not connected."); return; }
    CompileResult result;
    if(!compileSource(result)) return;
    if(!result.warnings.isEmpty())
        log(QString("%1 warning(s); sending anyway (see the list).").arg(result.warnings.count()));
    // Only a script that calls syscalls is stamped with the table hash; the
    // compiler decides (result.syscallHash is 0 otherwise).
    uploadPayload(comms, result.payload, result.syscallHash);
}

// ---------------------------------------------------------------------------
// Precompiled hex
// ---------------------------------------------------------------------------

void DeviceScriptDialog::browseHex()
{
    const QString path = QFileDialog::getOpenFileName(this, "Select a compiled script (hex)",
                                                      hexFileEdit_->text().isEmpty() ? startDirectory()
                                                          : QFileInfo(hexFileEdit_->text()).absolutePath(),
                                                      "Hex payload (*.hex *.txt);;All files (*)");
    if(!path.isEmpty()) hexFileEdit_->setText(path);
}

void DeviceScriptDialog::useDeviceHash()
{
    if(haveTable_) hashEdit_->setText(hex8(table_.hash));
}

void DeviceScriptDialog::sendHex()
{
    Comms *comms = current();
    if(comms == nullptr) { log("Not connected."); return; }

    const QString path = hexFileEdit_->text().trimmed();
    if(path.isEmpty()) { log("Choose the compiled script (.hex) to send first."); return; }
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        log("Cannot read " + path + ": " + file.errorString());
        return;
    }
    QByteArray payload;
    QString err;
    if(!parseHexPayload(QString::fromLatin1(file.readAll()), payload, &err))
    {
        log(path + ": " + err);
        return;
    }
    quint32 hash = 0;
    if(!hashEdit_->text().trimmed().isEmpty() && !parseHashText(hashEdit_->text(), hash))
    {
        log("The syscall table hash must be 1 to 8 hex digits, not zero.");
        return;
    }
    uploadPayload(comms, payload, hash);
}
