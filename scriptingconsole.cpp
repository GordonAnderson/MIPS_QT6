// =============================================================================
// scriptingconsole.cpp
//
// JavaScript scripting engine classes for the MIPS host application.
//
// The JSengine class encapsulates the QJSEngine and runs in a separate thread.
// All invokable functions block using Qt::BlockingQueuedConnection signals to
// marshal calls safely back to the main thread (ControlPanel slots).
//
// Note: QString assignment to JSengine::script is not thread safe. Callers
// must set engine->script before emitting startEngine() while the engine is
// idle (isRunning == false).
//
// To do:
//   - Look into ways to better integrate scripts and the main loop. Consider
//     running the update loop in its own thread to avoid blocking the main UI.
//
// Depends on:  scriptingconsole.h, ui_scriptingconsole.h, controlpanel.h,
//              properties.h, Utilities.h, help.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "scriptingconsole.h"
#include "ui_scriptingconsole.h"
#include "help.h"
#include "Utilities.h"
#include <QJSEngine>

#include <QtCore/QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QMessageBox>
#include <QObject>
#include <QApplication>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <QString>

#include "controlpanel.h"

// ---------------------------------------------------------------------------
// JSengine
// ---------------------------------------------------------------------------

/*! \brief JSengine constructor
 *
 * Connects each JSengine signal to the corresponding ControlPanel slot using
 * Qt::BlockingQueuedConnection so that invokable script functions block until
 * the main thread has completed the requested operation and returned a value.
 */
JSengine::JSengine(QWidget *parent)
{
    p = parent;
    ControlPanel *cp = (ControlPanel *)p;

    connect(this, SIGNAL(SaveSig(QString)),                          cp, SLOT(Save(QString)),                          Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(LoadSig(QString)),                          cp, SLOT(Load(QString)),                          Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(GetLineSig(QString)),                       cp, SLOT(GetLine(QString)),                       Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(SendStringSig(QString,QString)),            cp, SLOT(SendString(QString,QString)),            Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(SendCommandSig(QString,QString)),           cp, SLOT(SendCommand(QString,QString)),           Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(SendMessSig(QString,QString)),              cp, SLOT(SendMess(QString,QString)),              Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(SystemEnableSig()),                         cp, SLOT(SystemEnable()),                         Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(SystemShutdownSig()),                       cp, SLOT(SystemShutdown()),                       Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(isShutDownSig()),                           cp, SLOT(isShutDown()),                           Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(AcquireSig(QString)),                       cp, SLOT(Acquire(QString)),                       Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(isAcquiringSig()),                          cp, SLOT(isAcquiring()),                          Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(DismissAcquireSig()),                       cp, SLOT(DismissAcquire()),                       Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(statusMessageSig(QString)),                 cp, SLOT(statusMessage(QString)),                 Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(popupMessageSig(QString)),                  cp, SLOT(popupMessage(QString)),                  Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(popupYesNoMessageSig(QString)),             cp, SLOT(popupYesNoMessage(QString)),             Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(popupUserInputSig(QString,QString)),        cp, SLOT(popupUserInput(QString,QString)),        Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(sysUpdatingSig()),                          cp, SLOT(sysUpdating()),                          Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(UpdateHaltedSig(bool)),                     cp, SLOT(UpdateHalted(bool)),                     Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(UpdateHaltedSigNB(bool)),                   cp, SLOT(UpdateHalted(bool)));
    connect(this, SIGNAL(SelectFileSig(QString,QString,QString)),    cp, SLOT(SelectFile(QString,QString,QString)),    Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(ReadCSVfileSig(QString,QString)),           cp, SLOT(ReadCSVfile(QString,QString)),           Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(ReadCSVentrySig(int,int)),                  cp, SLOT(ReadCSVentry(int,int)),                  Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(CommandSig(QString)),                       cp, SLOT(Command(QString)),                       Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(CreatePlotSig(QString,QString,QString,int)),cp, SLOT(CreatePlot(QString,QString,QString,int)),Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(PlotCommandSig(QString)),                   cp, SLOT(PlotCommand(QString)),                   Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(isCommsSig()),                              cp, SLOT(isComms()),                              Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(MakePathNameUniqueSig(QString)),            cp, SLOT(MakePathNameUnique(QString)),            Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(GenerateUniqueFileNameSig()),               cp, SLOT(GenerateUniqueFileName()),               Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(MakeFileNameUniqueSig(QString)),            cp, SLOT(MakeFileNameUnique(QString)),            Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(AppendToFileSig(QString,QString)),          cp, SLOT(AppendToFile(QString,QString)),          Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(ReadFileLineSig(QString,int)),              cp, SLOT(ReadFileLine(QString,int)),              Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(ReadFileSig(QString)),                      cp, SLOT(ReadFile(QString)),                      Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(elapsedTimeSig(bool)),                      cp, SLOT(elapsedTime(bool)),                      Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(tcpSocketSig(QString,QString,QString)),     cp, SLOT(tcpSocket(QString,QString,QString)),     Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(setValueSig(const QString &,const QVariant &)), cp, SLOT(setValue(const QString &,const QVariant &)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(getValueSig(const QString &)),              cp, SLOT(getValue(const QString &)),              Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(CreateProcessSig(QString,QString)),         cp, SLOT(CreateProcess(QString,QString)),         Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(ZMQsig(QString)),                           cp, SLOT(ZMQ(QString)),                           Qt::BlockingQueuedConnection);

    //QJSValue mips = engine.newQObject(this);
    //engine.globalObject().setProperty("mips", mips);
}

// doMsDelay — sleeps the engine thread for ms milliseconds. Called from
// running scripts via the mips.msDelay() JavaScript binding.
void JSengine::doMsDelay(int ms)
{
    QThread::msleep(ms);
}

// doWaitForUpdate — blocks until one complete update cycle has finished.
// Polls sysUpdating() to detect a false→true→false transition.
void JSengine::doWaitForUpdate(void)
{
    while(sysUpdating() == true)  doMsDelay(100);
    while(sysUpdating() == false) doMsDelay(100);
    while(sysUpdating() == true)  doMsDelay(100);
}

// setInterrupted — propagates an interrupt request to the QJSEngine, causing
// the running script to throw a JS exception on the next statement boundary.
void JSengine::setInterrupted(bool state)
{
    if (engine) engine->setInterrupted(state);
}

/*! \brief runEngine
 *
 * Runs the script or a named function call. If scriptCall is empty the full
 * script text is evaluated. If scriptCall is non-empty it is parsed as a
 * function call with arguments: "functionName(arg1,arg2)".
 */
QVariant JSengine::runEngine(void)
{
    // CRITICAL FIX: Ensure engine exists in the CURRENT thread
    if (!engine) {
        engine = new QJSEngine();

        // Re-bind the "mips" object to the new local engine
        QJSValue mips = engine->newQObject(this);
        engine->globalObject().setProperty("mips", mips);
    }

    if(scriptCall.isEmpty())
    {
        isRunning = true;
        result    = engine->evaluate(script);
        emit resultReady(result);
        isRunning = false;
        return result.toVariant();
    }
    // Parse call string and invoke the named function with arguments
    static QRegularExpression re("[()]");
    QStringList callList = scriptCall.split(re);
    if(callList.length() == 3)
    {
        isRunning = true;
        QStringList argsList = callList[1].split(",");
        QJSValueList args;
        for(int i = 0; i < argsList.count(); i++) args.append(argsList[i]);
        QJSValue fun = engine->evaluate(script);
        result = fun.call(args);
        emit resultReady(result);
        isRunning = false;
        return result.toVariant();
    }
    result = "Invalid call!";
    emit resultReady(result);
    return result.toVariant();
}

// ---------------------------------------------------------------------------
// Script
// ---------------------------------------------------------------------------

// Script — constructor. Creates a dedicated QThread and JSengine, wires
// BlockingQueuedConnection signals for cross-thread safety, and starts
// the thread ready for script execution.
Script::Script(QWidget *parent, QString scriptName, QString fileName, Properties *prop, QStatusBar *statusbar)
{
    p                = parent;
    properties       = prop;
    sb               = statusbar;
    this->scriptName = scriptName;
    this->fileName   = fileName;
    scriptText.clear();
    scriptCall.clear();

    thread = new QThread(this);
    engine = new JSengine(p);
    connect(engine, &JSengine::resultReady,    this, &Script::scriptFinished);
    engine->moveToThread(thread);
    connect(thread, SIGNAL(finished()),            this, SLOT(engineDone()));
    connect(this,   SIGNAL(startEngine()),         engine, SLOT(runEngine()));
    connect(this,   SIGNAL(setInterrupted(bool)),  engine, SLOT(setInterrupted(bool)));
    thread->start();
}

// ~Script — interrupts any running script, disconnects all signals, and
// cleanly shuts down the engine thread before releasing resources.
Script::~Script()
{
    engine->setInterrupted(true);
    disconnect(engine, &JSengine::resultReady,   this, &Script::scriptFinished);
    disconnect(thread, SIGNAL(finished()),           this, SLOT(engineDone()));
    disconnect(this,   SIGNAL(startEngine()),        engine, SLOT(runEngine()));
    disconnect(this,   SIGNAL(setInterrupted(bool)), engine, SLOT(setInterrupted(bool)));
    thread->quit();
    thread->wait();
    engine->deleteLater();
    thread->deleteLater();
}

// run — loads the script file if not already cached, then emits startEngine()
// to run it on the worker thread. Returns false if the engine is already
// running or the file cannot be opened.
bool Script::run(void)
{
    if(engine == nullptr) engine = new JSengine(p);
    if(engine->isRunning) return false;

    if(scriptText.isEmpty())
    {
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            if(properties != NULL) properties->Log("Script loaded: " + fileName);
            QTextStream stream(&file);
            scriptText = stream.readAll();
            file.close();
        }
        else
        {
            if(properties != NULL) properties->Log("Can't open script file: " + fileName);
            if(sb != NULL) sb->showMessage("Can't open script file: " + fileName);
            return false;
        }
    }
    if(!scriptText.isEmpty())
    {
        // Note: QString assignment is not thread safe — engine must be idle (isRunning == false)
        engine->script     = scriptText;
        engine->scriptCall = scriptCall;
        emit setInterrupted(false);
        emit startEngine();
        return true;
    }
    return false;
}

// scriptFinished — slot called by the engine when a script completes.
// Logs the result or error message to the Properties log and status bar.
void Script::scriptFinished(QJSValue result)
{
    if(result.isError())
    {
        if(properties != NULL) properties->Log("Script error: " + result.toString());
        if(sb != NULL) sb->showMessage("Script error: " + result.toString());
    }
    else
    {
        if(properties != NULL) properties->Log("Script finished!");
        if(sb != NULL) sb->showMessage("Script finished!");
    }
}

// engineDone — slot called when the worker thread finishes; reserved for
// future cleanup.
void Script::engineDone()
{
}

// ProcessCommand — scripting API for a named Script object. Supports:
//   name.run / name.abort / name.isRunning — control and status queries
//   name.scriptText / name.scriptCall      — read/write cached text and call
// Returns "?" for unrecognised commands.
QString Script::ProcessCommand(QString cmd)
{
    if((scriptName + ".run") == cmd.trimmed())
    {
        run();
        return "";
    }
    if((scriptName + ".abort") == cmd.trimmed())
    {
        if(engine->isRunning == false) return "";
        engine->setInterrupted(true);
        if(properties != NULL) properties->Log("Script aborted");
        return "";
    }
    if((scriptName + ".isRunning") == cmd.trimmed())
        return engine->isRunning ? "TRUE" : "FALSE";
    if((scriptName + ".scriptText") == cmd.trimmed())
        return scriptText;
    if((scriptName + ".scriptCall") == cmd.trimmed())
        return scriptCall;

    QStringList resList = cmd.split("=");
    if((resList.count() == 2) && (resList[0].trimmed() == (scriptName + ".scriptText").trimmed()))
        scriptText = resList[1];
    if((resList.count() == 2) && (resList[0].trimmed() == (scriptName + ".scriptCall").trimmed()))
        scriptCall = resList[1];
    return "?";
}

// ---------------------------------------------------------------------------
// ScriptingConsole
// ---------------------------------------------------------------------------

// ScriptingConsole — constructor. Creates the dialog UI, a dedicated thread,
// and a JSengine. Connects engine result and lifecycle signals and starts
// the thread ready for interactive script execution.
ScriptingConsole::ScriptingConsole(QWidget *parent, Properties *prop) :
    QDialog(parent),
    ui(new Ui::ScriptingConsole)
{
    p          = parent;
    ui->setupUi(this);
    properties = prop;

    this->setFixedSize(501, 366);
    thread = new QThread(this);
    engine = new JSengine(p);
    connect(engine, &JSengine::resultReady,   this, &ScriptingConsole::scriptFinished);
    engine->moveToThread(thread);
    connect(thread, SIGNAL(finished()),           this, SLOT(engineDone()));
    connect(this,   SIGNAL(startEngine()),        engine, SLOT(runEngine()));
    connect(this,   SIGNAL(setInterrupted(bool)), engine, SLOT(setInterrupted(bool)));
    thread->start();
}

// ~ScriptingConsole — interrupts any running script, disconnects signals,
// shuts down the engine thread, and releases the UI.
ScriptingConsole::~ScriptingConsole()
{
    engine->setInterrupted(true);
    disconnect(engine, &JSengine::resultReady,   this, &ScriptingConsole::scriptFinished);
    disconnect(thread, SIGNAL(finished()),           this, SLOT(engineDone()));
    disconnect(this,   SIGNAL(startEngine()),        engine, SLOT(runEngine()));
    disconnect(this,   SIGNAL(setInterrupted(bool)), engine, SLOT(setInterrupted(bool)));
    thread->quit();
    thread->wait();
    engine->deleteLater();
    thread->deleteLater();
    delete ui;
}

// paintEvent — refreshes the Execute button label on each repaint to reflect
// whether a script is currently running.
void ScriptingConsole::paintEvent(QPaintEvent *)
{
    UpdateStatus();
}

// scriptFinished — slot called when the engine completes. Displays the result
// in red for errors, plain text otherwise.
void ScriptingConsole::scriptFinished(QJSValue result)
{
    QString markup;
    if(result.isError()) markup.append("<font color=\"red\">");
    markup.append(result.toString());
    if(result.isError()) markup.append("</font>");
    ui->lblStatus->setText(markup);
}

// engineDone — slot called when the worker thread finishes; reserved for
// future cleanup.
void ScriptingConsole::engineDone()
{
}

// RunScript — copies the editor text into the engine and emits startEngine()
// to run it on the worker thread. No-op if the engine is already running.
void ScriptingConsole::RunScript(void)
{
    if(engine->isRunning == true) return;
    if(properties != NULL) properties->Log("Script executed");
    ui->lblStatus->setText("");
    // Note: QString assignment is not thread safe — engine must be idle before setting script
    engine->script = ui->txtScript->toPlainText();
    emit setInterrupted(false);
    emit startEngine();
}

// on_pbEvaluate_clicked — Execute button handler; clears the button pressed
// state and delegates to RunScript().
void ScriptingConsole::on_pbEvaluate_clicked()
{
    ui->pbEvaluate->setDown(false);
    RunScript();
}

// UpdateStatus — sets the Execute button label to "Script is running!" while
// the engine is active, or "Execute" when idle.
void ScriptingConsole::UpdateStatus(void)
{
    ui->pbEvaluate->setText(engine->isRunning ? "Script is running!" : "Execute");
}

// on_pbSave_clicked — opens a save-file dialog and writes the editor text to
// a .scrpt file.
void ScriptingConsole::on_pbSave_clicked()
{
    QFileDialog fileDialog;

    if(properties != NULL) fileDialog.setDirectory(properties->ScriptPath);
    QString fileName = fileDialog.getSaveFileName(this, tr("Save script to file"), "", tr("scrpt (*.scrpt);;All files (*.*)"));
    if(fileName == "") return;
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << ui->txtScript->toPlainText();
        file.close();
        if(properties != NULL) properties->Log("Script saved: " + fileName);
    }
}

// on_pbLoad_clicked — opens a file-open dialog and loads a .scrpt file into
// the editor.
void ScriptingConsole::on_pbLoad_clicked()
{
    QFileDialog fileDialog;

    if(properties != NULL) fileDialog.setDirectory(properties->ScriptPath);
    QString fileName = fileDialog.getOpenFileName(this, tr("Load script from File"), "", tr("scrpt (*.scrpt);;All files (*.*)"));
    if(fileName == "") return;
    QFile file(fileName);
    ui->txtScript->clear();
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        ui->txtScript->append(stream.readAll());
        file.close();
        if(properties != NULL) properties->Log("Script loaded: " + fileName);
    }
}

// -----------------------------------------------------------------------------
// on_pbAbort_clicked — interrupts a running script.
// Note: cannot use emit setInterrupted(true) here because the engine blocks
// while running. engine->setInterrupted(true) is thread-safe.
// -----------------------------------------------------------------------------
void ScriptingConsole::on_pbAbort_clicked()
{
    if(engine->isRunning == false) return;
    engine->setInterrupted(true);
    if(properties != NULL) properties->Log("Script aborted");
}

// on_pbHelp_clicked — opens the scripting help viewer loaded from the
// embedded :/scripthelp.txt resource.
void ScriptingConsole::on_pbHelp_clicked()
{
    Help *help = new Help();
    help->SetTitle("Script Help");
    help->LoadHelpText(":/scripthelp.txt");
    help->show();
}

// ---------------------------------------------------------------------------
// ScriptButton
// ---------------------------------------------------------------------------

// ScriptButton — constructor. Creates a QPushButton at (x,y) backed by a
// dedicated JSengine thread. Traverses to the top-level window to find the
// ControlPanel for signal connections, then starts the engine thread.
ScriptButton::ScriptButton(QWidget *parent, QString name, QString ScriptFile, int x, int y, Properties *prop, QStatusBar *statusbar)
{
    p               = parent;
    X               = x;
    Y               = y;
    Title           = name;
    FileName        = ScriptFile;
    properties      = prop;
    sb              = statusbar;
    engine          = NULL;
    CallOnUpdate    = false;
    CallOnStart     = false;
    ScriptTextFixed = false;
    reportResults   = true;
    ScriptText.clear();
    scriptAbort.clear();

    pcp = p->parentWidget();
    while(pcp->parentWidget() != NULL) pcp = pcp->parentWidget();

    buttonTimer = new QTimer(this);
    connect(buttonTimer, &QTimer::timeout, this, &ScriptButton::onTimerTimeout);

    thread = new QThread(this);
    engine = new JSengine(pcp);
    connect(engine, &JSengine::resultReady,   this, &ScriptButton::scriptFinished);
    engine->moveToThread(thread);
    connect(thread, SIGNAL(finished()),           this, SLOT(engineDone()));
    connect(this,   SIGNAL(startEngine()),        engine, SLOT(runEngine()));
    connect(this,   SIGNAL(setInterrupted(bool)), engine, SLOT(setInterrupted(bool)));
    thread->start();
}

// ~ScriptButton — interrupts any running script, disconnects signals, and
// cleanly shuts down the engine thread.
ScriptButton::~ScriptButton()
{
    engine->setInterrupted(true);
    disconnect(engine, &JSengine::resultReady,   this, &ScriptButton::scriptFinished);
    disconnect(thread, SIGNAL(finished()),           this, SLOT(engineDone()));
    disconnect(this,   SIGNAL(startEngine()),        engine, SLOT(runEngine()));
    disconnect(this,   SIGNAL(setInterrupted(bool)), engine, SLOT(setInterrupted(bool)));
    thread->quit();
    thread->wait();
    engine->deleteLater();
    thread->deleteLater();
}

// Show — creates the push-button widget at the stored (X, Y) position and
// installs drag-to-move support.
void ScriptButton::Show(void)
{
    pbButton = new QPushButton(Title, p);
    pbButton->setGeometry(X, Y, 150, 32);
    pbButton->setAutoDefault(false);
    connect(pbButton, &QPushButton::pressed, this, &ScriptButton::pbButtonPressed, Qt::QueuedConnection);
    pbButton->installEventFilter(this);
    pbButton->setMouseTracking(true);
}

// eventFilter — delegates drag-to-move to moveWidget().
bool ScriptButton::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, pbButton, pbButton, event)) return true;
    return false;
}

// onTimerTimeout — re-enables the push button 250 ms after it was pressed to
// debounce rapid repeated clicks.
void ScriptButton::onTimerTimeout(void)
{
    pbButton->setEnabled(true);
    buttonTimer->stop();
}

// scriptFinished — slot called when the engine completes. If reportResults is
// set, logs the outcome and restores the default cursor.
void ScriptButton::scriptFinished(QJSValue result)
{
    if(reportResults)
    {
        if(result.isError())
        {
            if(properties != NULL) properties->Log("Script error: " + result.toString());
            if(sb != NULL) sb->showMessage("Script error: " + result.toString());
        }
        else
        {
            if(properties != NULL) properties->Log("Script finished!");
            if(sb != NULL) sb->showMessage("Script finished!");
        }
        QApplication::restoreOverrideCursor();
    }
}

// engineDone — slot called when the worker thread finishes. Restores the
// default cursor in case the thread exited abnormally.
void ScriptButton::engineDone()
{
    QApplication::restoreOverrideCursor();
}

// pbButtonPressed — Qt pressed() slot. Sets keyboard focus then delegates to
// ButtonPressed(true) for the full load-and-run sequence.
void ScriptButton::pbButtonPressed(void)
{
    pbButton->setFocus();
    ButtonPressed(true);
}

/*! \brief ButtonPressed
 *
 * If a script is already running, asks the user whether to abort it.
 * If not running, loads the script from file (if AlwaysLoad or not yet loaded)
 * and executes it. If scriptAbort is defined it is run instead of the main
 * script when the user confirms an abort.
 *
 * \param AlwaysLoad If true, reloads the script file even if already cached.
 */
void ScriptButton::ButtonPressed(bool AlwaysLoad)
{
    pbButton->setDown(false);
    pbButton->setEnabled(false);
    buttonTimer->start(250);

    if(engine->isRunning)
    {
        QMessageBox msgBox;
        msgBox.setText("A script is currently running, do you want to abort?");
        msgBox.setInformativeText("Select yes to abort");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        if(msgBox.exec() == QMessageBox::No) return;
        emit engine->UpdateHaltedSigNB(false);
        engine->setInterrupted(true);
        if(properties != NULL) properties->Log("Script aborted by button");
        if(!scriptAbort.isEmpty())
        {
            if(properties != NULL) properties->Log("Abort script executed");
            engine->script = scriptAbort;
            emit setInterrupted(false);
            emit startEngine();
        }
        return;
    }

    if(((AlwaysLoad) || (ScriptText.isEmpty())) && !ScriptTextFixed)
    {
        QFile file(FileName);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            if(properties != NULL) properties->Log("Script loaded: " + FileName);
            QTextStream stream(&file);
            ScriptText = stream.readAll();
            file.close();
        }
        else
        {
            if(properties != NULL) properties->Log("Can't open script file: " + FileName);
            if(sb != NULL) sb->showMessage("Can't open script file: " + FileName);
            return;
        }
    }
    if(!ScriptText.isEmpty())
    {
        reportResults      = AlwaysLoad;
        if(AlwaysLoad) QApplication::setOverrideCursor(Qt::WaitCursor);
        engine->script     = ScriptText;
        engine->scriptCall = scriptCall;
        emit setInterrupted(false);
        emit startEngine();
    }
}

/*! \brief ProcessCommand
 *
 * Scripting interface for the script button. Supports:
 *   title           — run the script (same as pressing the button)
 *   title.scriptText / title.scriptCall / title.scriptAbort — read properties
 *   title=ABORT     — abort a running script
 *   title.prop=val  — write scriptText, scriptCall, or scriptAbort
 */
QString ScriptButton::ProcessCommand(QString cmd)
{
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;

    if(title == cmd.trimmed())                            { ButtonPressed(false); return ""; }
    if((title + ".scriptText")  == cmd.trimmed())         return ScriptText;
    if((title + ".scriptCall")  == cmd.trimmed())         return scriptCall;
    if((title + ".scriptAbort") == cmd.trimmed())         return scriptAbort;

    QStringList resList = cmd.split("=");
    if((resList.count() == 2) && (resList[0].trimmed() == title.trimmed()))
    {
        if(resList[1].trimmed().toUpper() == "ABORT")
        {
            emit engine->UpdateHaltedSigNB(false);
            if(engine->isRunning == false) return "";
            engine->setInterrupted(true);
            if(properties != NULL) properties->Log("Script aborted by another script");
            return "";
        }
    }
    if((resList.count() == 2) && (resList[0].trimmed() == (title + ".scriptText").trimmed()))
        { ScriptText = resList[1]; return ""; }
    if((resList.count() == 2) && (resList[0].trimmed() == (title + ".scriptCall").trimmed()))
        { scriptCall = resList[1]; return ""; }
    if((resList.count() == 2) && (resList[0].trimmed() == (title + ".scriptAbort").trimmed()))
        { scriptAbort = resList[1]; return ""; }
    return "?";
}

/*! \brief Update
 *
 * Called on each control panel update cycle. Runs the script if CallOnUpdate
 * is set. On the first call with CallOnStart set, shows a standby message,
 * runs the script once, then clears the flag.
 */
void ScriptButton::Update(void)
{
    if(CallOnUpdate) ButtonPressed(false);
    if(CallOnStart)
    {
        QMessageBox *msg = new QMessageBox();
        msg->setText("Configuring system, standby...");
        msg->setStandardButtons(QMessageBox::NoButton);
        msg->setWindowModality(Qt::NonModal);
        msg->show();
        CallOnStart = false;
        ButtonPressed(false);
        msg->hide();
    }
}
