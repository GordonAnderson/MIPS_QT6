//
// scriptconsole.cpp
//
// This file contains the classes that support the script console as well
// as the script buttons. The JSengine class encapulates the script engine,
// the JSengine blocks so it is run in a seperate thread. The mips object and
// its functions, that are invokable from the script, are interfaced through
// the JSengine class. All the interface functions block and use signals to
// interface to the main thread.
//
// The script class allows scripts to be executed without using the script
// console or the script button.
//
// If a script communicates with a MIPS system the user needs to make sure
// the main loop is not in the update loop and actively communicating with
// the MIPS hardware. Functions are provided to halt updating and to see if
// the system is in the update loop.
//
// To do list
//  - look into ways to better integrate scripts and the main loop.
//    consider the update loop running in its own tread and thus not
//    blocking the main UI.
//
// Gordon Anderson
// GAA Custom Electronics, LLC
// gaa@gaa-ce.com
//

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

/*! \brief JSengine class implementation
 *
 * The JSengine class is a wrapper around the QJSEngine class that allows
 * scripts to be run in a worker thread. The JSengine class provides a
 * number of signals that can be used to communicate with the main thread.
 * The JSengine class also provides a number of functions that can be called
 * from the script.
 */
JSengine::JSengine(QWidget *parent)
{
    p = parent;
    ControlPanel *cp;
    cp = (ControlPanel *)p;

    connect(this,SIGNAL(SaveSig(QString)),cp,SLOT(Save(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(LoadSig(QString)),cp,SLOT(Load(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(GetLineSig(QString)),cp,SLOT(GetLine(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(SendStringSig(QString,QString)),cp,SLOT(SendString(QString,QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(SendCommandSig(QString,QString)),cp,SLOT(SendCommand(QString,QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(SendMessSig(QString,QString)),cp,SLOT(SendMess(QString,QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(SystemEnableSig()),cp,SLOT(SystemEnable()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(SystemShutdownSig()),cp,SLOT(SystemShutdown()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(isShutDownSig()),cp,SLOT(isShutDown()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(AcquireSig(QString)),cp,SLOT(Acquire(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(isAcquiringSig()),cp,SLOT(isAcquiring()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(DismissAcquireSig()),cp,SLOT(DismissAcquire()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(statusMessageSig(QString)),cp,SLOT(statusMessage(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(popupMessageSig(QString)),cp,SLOT(popupMessage(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(popupYesNoMessageSig(QString)),cp,SLOT(popupYesNoMessage(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(popupUserInputSig(QString,QString)),cp,SLOT(popupUserInput(QString,QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(sysUpdatingSig()),cp,SLOT(sysUpdating()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(UpdateHaltedSig(bool)),cp,SLOT(UpdateHalted(bool)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(UpdateHaltedSigNB(bool)),cp,SLOT(UpdateHalted(bool)));
    connect(this,SIGNAL(SelectFileSig(QString,QString,QString)),cp,SLOT(SelectFile(QString,QString,QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(ReadCSVfileSig(QString,QString)),cp,SLOT(ReadCSVfile(QString,QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(ReadCSVentrySig(int,int)),cp,SLOT(ReadCSVentry(int,int)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(CommandSig(QString)),cp,SLOT(Command(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(CreatePlotSig(QString,QString,QString,int)),cp,SLOT(CreatePlot(QString,QString,QString,int)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(PlotCommandSig(QString)),cp,SLOT(PlotCommand(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(isCommsSig()),cp,SLOT(isComms()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(MakePathNameUniqueSig(QString)),cp,SLOT(MakePathNameUnique(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(GenerateUniqueFileNameSig()),cp,SLOT(GenerateUniqueFileName()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(MakeFileNameUniqueSig(QString)),cp,SLOT(MakeFileNameUnique(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(AppendToFileSig(QString,QString)),cp,SLOT(AppendToFile(QString,QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(ReadFileLineSig(QString,int)),cp,SLOT(ReadFileLine(QString,int)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(ReadFileSig(QString)),cp,SLOT(ReadFile(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(elapsedTimeSig(bool)),cp,SLOT(elapsedTime(bool)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(tcpSocketSig(QString,QString,QString)),cp,SLOT(tcpSocket(QString,QString,QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(setValueSig(const QString &,const QVariant &)),cp,SLOT(setValue(const QString &,const QVariant &)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(getValueSig(const QString &)),cp,SLOT(getValue(const QString &)),Qt::BlockingQueuedConnection);

    QJSValue mips = engine.newQObject(this);
    //engine.installExtensions(QJSEngine::AllExtensions);
    engine.globalObject().setProperty("mips",mips);
}

void JSengine::doMsDelay(int ms)
{
    QThread::msleep(ms);
}

void JSengine::doWaitForUpdate(void)
{
    // Wait for updating flag to clear
    while(sysUpdating()==true) doMsDelay(100);
    // Wait for updating flag to set
    while(sysUpdating()==false) doMsDelay(100);
    // Wait for updating flag to clear
    while(sysUpdating()==true) doMsDelay(100);
}

void JSengine::setInterrupted(bool state)
{
    engine.setInterrupted(state);
}

/*! \brief runEngine
 * This function runs the script engine, it can either run a script or a
 * function call. If the scriptCall is empty then the script is evaluated
 * and the result is returned. If the scriptCall is not empty then it is
 * assumed to be a function call with arguments, the function is evaluated
 * and called with the arguments.
 *
 * \return QJSValue The result of the script or function call.
 */
QJSValue JSengine::runEngine(void)
{
    if(scriptCall.isEmpty())
    {
       isRunning = true;
       result = engine.evaluate(script);
       emit resultReady(result);
       isRunning = false;
       return result;
    }
    // Parse call string and get arguments
    static QRegularExpression re("[()]");
    QStringList callList = scriptCall.split(re);
    if(callList.length() == 3)
    {
        isRunning = true;
        QStringList argsList = callList[1].split(",");
        QJSValueList args;
        for(int i=0;i<argsList.count();i++) args.append(argsList[i]);
        QJSValue fun = engine.evaluate(script);
        result = fun.call(args);
        emit resultReady(result);
        isRunning = false;
        return result;
    }
    result = "Invalid call!";
    emit resultReady(result);
    return result;
}

/*! \brief Script class implementation
 *
 * The script class allows scripts to be executed without using the script
 * console or the script button. The script is run in a worker thread to
 * prevent blocking the main UI thread.
 */
Script::Script(QWidget *parent, QString scriptName, QString fileName, Properties *prop, QStatusBar *statusbar)
{
    p = parent;
    properties = prop;
    sb = statusbar;
    this->scriptName = scriptName;
    this->fileName = fileName;
    scriptText.clear();
    scriptCall.clear();

    thread = new QThread(this);
    engine = new JSengine(p);
    connect(engine,&JSengine::resultReady,this,&Script::scriptFinished);
    engine->moveToThread(thread);
    connect(thread, SIGNAL(finished()), this, SLOT(engineDone()));
    connect(this, SIGNAL(startEngine()), engine, SLOT(runEngine()));
    connect(this, SIGNAL(setInterrupted(bool)), engine, SLOT(setInterrupted(bool)));
    thread->start();
}

Script::~Script()
{
    engine->setInterrupted(true);
    disconnect(engine,&JSengine::resultReady,this,&Script::scriptFinished);
    disconnect(thread, SIGNAL(finished()), this, SLOT(engineDone()));
    disconnect(this, SIGNAL(startEngine()), engine, SLOT(runEngine()));
    disconnect(this, SIGNAL(setInterrupted(bool)), engine, SLOT(setInterrupted(bool)));
    thread->quit();
    thread->wait();
    engine->deleteLater();
    thread->deleteLater();
}

bool Script::run(void)
{
    if(engine == nullptr) engine = new JSengine(p);
    if(engine->isRunning) return false;
    // Load Script if needed
    if(scriptText.isEmpty())
    {
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly|QIODevice::Text))
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
        // Load script to the script engine, this should likely be done with
        // a slot and not a direct call.
        // QString is not thread safe!
        engine->script = scriptText;
        engine->scriptCall = scriptCall;
        emit setInterrupted(false); // Make sure the interrupted flag is clear
        emit startEngine();
        return true;
    }
    return false;
}

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

void Script::engineDone()
{
}

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
    {
        if(engine->isRunning) return "TRUE";
        else return "FALSE";
    }
    if((scriptName + ".scriptText") == cmd.trimmed())
    {
        return scriptText;
    }
    if((scriptName + ".scriptCall") == cmd.trimmed())
    {
        return scriptCall;
    }
    QStringList resList = cmd.split("=");
    if((resList.count()==2) && (resList[0].trimmed() == (scriptName + ".scriptText").trimmed()))
    {
        scriptText = resList[1];
    }
    if((resList.count()==2) && (resList[0].trimmed() == (scriptName + ".scriptCall").trimmed()))
    {
        scriptCall = resList[1];
    }
    return "?";
}

//
// ScriptingConsole class implementation
//
// The script engine is run in a worker thread to prevent blocking. The
// worker thread and engine are created in the constructor and only deleted
// when this object is deleted.
//
ScriptingConsole::ScriptingConsole(QWidget *parent, Properties *prop) :
    QDialog(parent),
    ui(new Ui::ScriptingConsole)
{
   p = parent;
   ui->setupUi(this);
   properties = prop;

   this->setFixedSize(501,366);
   // Create thread, if parent is "this" then this class must clean up
   // and delete thread when this object is deleted
   thread = new QThread(this);
   engine = new JSengine(p);
   connect(engine,&JSengine::resultReady,this,&ScriptingConsole::scriptFinished);
   engine->moveToThread(thread);
   connect(thread, SIGNAL(finished()), this, SLOT(engineDone()));
   connect(this, SIGNAL(startEngine()), engine, SLOT(runEngine()));
   connect(this, SIGNAL(setInterrupted(bool)), engine, SLOT(setInterrupted(bool)));
   thread->start();
}

ScriptingConsole::~ScriptingConsole()
{
    engine->setInterrupted(true);
    disconnect(engine,&JSengine::resultReady,this,&ScriptingConsole::scriptFinished);
    disconnect(thread, SIGNAL(finished()), this, SLOT(engineDone()));
    disconnect(this, SIGNAL(startEngine()), engine, SLOT(runEngine()));
    disconnect(this, SIGNAL(setInterrupted(bool)), engine, SLOT(setInterrupted(bool)));
    thread->quit();
    thread->wait();
    engine->deleteLater();
    thread->deleteLater();
    delete ui;
}

void ScriptingConsole::paintEvent(QPaintEvent *)
{
    UpdateStatus();
}

void ScriptingConsole::scriptFinished(QJSValue result)
{
    // Script finished with result data, display the result
    QString markup;
    if(result.isError())
        markup.append("<font color=\"red\">");
    markup.append(result.toString());
    if(result.isError())
        markup.append("</font>");
    ui->lblStatus->setText(markup);
}

void ScriptingConsole::engineDone()
{
    // The script engine is done, do any necessary work
}

void ScriptingConsole::RunScript(void)
{
    if(engine->isRunning == true) return;
    if(properties != NULL) properties->Log("Script executed");
    ui->lblStatus->setText("");
    QString script = ui->txtScript->toPlainText();
    // Load script to the script engine, this should likely be done with
    // a slot and not a direct call.
    // QString is not thread safe!
    engine->script = script;
    emit setInterrupted(false); // Make sure the interrupted flag is clear
    emit startEngine();
}

void ScriptingConsole::on_pbEvaluate_clicked()
{
    ui->pbEvaluate->setDown(false);
    RunScript();
}

void ScriptingConsole::UpdateStatus(void)
{
    if(engine->isRunning == true)
    {
        ui->pbEvaluate->setText("Script is running!");
    }
    else
    {
        ui->pbEvaluate->setText("Execute");
    }
}

void ScriptingConsole::on_pbSave_clicked()
{
    QFileDialog fileDialog;

    if(properties != NULL) fileDialog.setDirectory(properties->ScriptPath);
    QString fileName = fileDialog.getSaveFileName(this, tr("Save script to file"),"",tr("scrpt (*.scrpt);;All files (*.*)"));
    if(fileName == "") return;
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // We're going to streaming text to the file
        QTextStream stream(&file);
        stream << ui->txtScript->toPlainText();
        file.close();
        if(properties != NULL) properties->Log("Script saved: " + fileName);
    }
}

void ScriptingConsole::on_pbLoad_clicked()
{
    QFileDialog fileDialog;

    if(properties != NULL) fileDialog.setDirectory(properties->ScriptPath);
    QString fileName = fileDialog.getOpenFileName(this, tr("Load script from File"),"",tr("scrpt (*.scrpt);;All files (*.*)"));
    if(fileName == "") return;
    QFile file(fileName);
    ui->txtScript->clear();
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        QString text = stream.readAll();
        ui->txtScript->append(text);
        file.close();
        if(properties != NULL) properties->Log("Script loaded: " + fileName);
    }
}

// This function will abort a running script
void ScriptingConsole::on_pbAbort_clicked()
{
    if(engine->isRunning == false) return;
    // Can not use emit setInterrupted(true) here because the
    // scripting engine blocks when it runs a script.
    // engine->setInterrupted(true); is thread safe.
    engine->setInterrupted(true);
    if(properties != NULL) properties->Log("Script aborted");
}

void ScriptingConsole::on_pbHelp_clicked()
{
    Help *help = new Help();

    help->SetTitle("Script Help");
    help->LoadHelpText(":/scripthelp.txt");
    help->show();
}

// **********************************************************************************************
// ScriptButton   *******************************************************************************
// **********************************************************************************************
ScriptButton::ScriptButton(QWidget *parent, QString name, QString ScriptFile, int x, int y, Properties *prop, QStatusBar *statusbar)
{
    p      = parent;
    X      = x;
    Y      = y;
    Title = name;
    FileName = ScriptFile;
    properties = prop;
    sb = statusbar;
    engine = NULL;
    CallOnUpdate = false;
    CallOnStart = false;
    ScriptText.clear();
    scriptAbort.clear();
    ScriptTextFixed = false;
    reportResults = true;

    pcp = p->parentWidget();
    while(pcp->parentWidget() != NULL) pcp = pcp->parentWidget();

    buttonTimer = new QTimer(this);
    connect(buttonTimer, &QTimer::timeout, this, &ScriptButton::onTimerTimeout);

    // Create thread, if parent is "this" then this class must clean up
    // and delete thread when this object is deleted
    thread = new QThread(this);
    engine = new JSengine(pcp);
    connect(engine,&JSengine::resultReady,this,&ScriptButton::scriptFinished);
    engine->moveToThread(thread);
    connect(thread, SIGNAL(finished()), this, SLOT(engineDone()));
    connect(this, SIGNAL(startEngine()), engine, SLOT(runEngine()));
    connect(this, SIGNAL(setInterrupted(bool)), engine, SLOT(setInterrupted(bool)));
    thread->start();
}

ScriptButton::~ScriptButton()
{
    engine->setInterrupted(true);
    disconnect(engine,&JSengine::resultReady,this,&ScriptButton::scriptFinished);
    disconnect(thread, SIGNAL(finished()), this, SLOT(engineDone()));
    disconnect(this, SIGNAL(startEngine()), engine, SLOT(runEngine()));
    disconnect(this, SIGNAL(setInterrupted(bool)), engine, SLOT(setInterrupted(bool)));
    thread->quit();
    thread->wait();
    engine->deleteLater();
    thread->deleteLater();
}

void ScriptButton::Show(void)
{
    pbButton = new QPushButton(Title,p);
    pbButton->setGeometry(X,Y,150,32);
    pbButton->setAutoDefault(false);
    connect(pbButton,SIGNAL(pressed()),this,SLOT(pbButtonPressed()),Qt::QueuedConnection);
    pbButton->installEventFilter(this);
    pbButton->setMouseTracking(true);
}

bool ScriptButton::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, pbButton, pbButton , event)) return true;
    return false;
}

void ScriptButton::onTimerTimeout(void)
{
    // Re-enable the button
    pbButton->setEnabled(true);

    // Stop the timer
    buttonTimer->stop();
}

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
    }
    if(reportResults) QApplication::restoreOverrideCursor();
}

void ScriptButton::engineDone()
{
    QApplication::restoreOverrideCursor();
}

// This event fires when the button is pressed, start script and turn
// button on when pressed. If pressed when script is running ask user
// if he would like to abort script.
void ScriptButton::pbButtonPressed(void)
{
    pbButton->setFocus();
    ButtonPressed(true);
}

/*! \brief ButtonPressed
 * This function is called when the button is pressed. It will check if a
 * script is running and if so, it will ask the user if they want to abort
 * the script. If the script is not running, it will load the script from
 * the file and run it.
 *
 * \param AlwaysLoad If true, the script will be loaded from the file even
 *                   if it is already loaded.
 */
void ScriptButton::ButtonPressed(bool AlwaysLoad)
{
    QMessageBox msgBox;

    pbButton->setDown(false);
    // Disable the button
    pbButton->setEnabled(false);

    // Start the timer to re-enable the button after a delay
    buttonTimer->start(250);

    if(engine->isRunning)
    {
        msgBox.setText("A script is currently running, do you want to abort?");
        msgBox.setInformativeText("Select yes to abort");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        if (msgBox.exec() == QMessageBox::No) return;
        // Stop the script
        if(engine)
        {
            emit engine->UpdateHaltedSigNB(false);
            engine->setInterrupted(true);
        }
        if(properties != NULL) properties->Log("Script aborted by button");
        // If abortScript is defined then run it
        if(!scriptAbort.isEmpty())
        {
            if(properties != NULL) properties->Log("Abort script executed");
            engine->script = scriptAbort;
            emit setInterrupted(false); // Make sure the interrupted flag is clear
            emit startEngine();
        }
        return;
    }
    // Load Script if needed
    if(((AlwaysLoad) || (ScriptText.isEmpty())) && !ScriptTextFixed)
    {
        QFile file(FileName);
        if(file.open(QIODevice::ReadOnly|QIODevice::Text))
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
        reportResults = AlwaysLoad;
        if(AlwaysLoad) QApplication::setOverrideCursor(Qt::WaitCursor);
        engine->script = ScriptText;
        engine->scriptCall = scriptCall;
        emit setInterrupted(false); // Make sure the interrupted flag is clear
        emit startEngine();
    }
}

/*! \brief ProcessCommand
 * This function processes a command that is sent to the script button.
 * The command can be to run the script, abort the script, or get the
 * script text or call.
 *
 * \param cmd The command to process.
 * \return QString The result of the command processing.
 */
QString ScriptButton::ProcessCommand(QString cmd)
{
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(title == cmd.trimmed())
    {
        ButtonPressed(false);
        return("");
    }
    if((title + ".scriptText") == cmd.trimmed())
    {
        return ScriptText;
    }
    if((title + ".scriptCall") == cmd.trimmed())
    {
        return scriptCall;
    }
    if((title + ".scriptAbort") == cmd.trimmed())
    {
        return scriptAbort;
    }
    QStringList resList = cmd.split("=");
    if((resList.count()==2) && (resList[0].trimmed() == title.trimmed()))
    {
        if(resList[1].trimmed().toUpper() == "ABORT")
        {
            // Here to abort the script if its running
            emit engine->UpdateHaltedSigNB(false);
            // Stop the script
            if(engine->isRunning == false) return "";
            engine->setInterrupted(true);
            if(properties != NULL) properties->Log("Script aborted by another script");
            return "";
        }
    }
    if((resList.count()==2) && (resList[0].trimmed() == (title + ".scriptText").trimmed()))
    {
        ScriptText = resList[1];
        return "";
    }
    if((resList.count()==2) && (resList[0].trimmed() == (title + ".scriptCall").trimmed()))
    {
        scriptCall = resList[1];
        return "";
    }
    if((resList.count()==2) && (resList[0].trimmed() == (title + ".scriptAbort").trimmed()))
    {
        scriptAbort = resList[1];
        return "";
    }
    return("?");
}

/*! \brief Update
 * This function is called to update the script button. It will call the
 * ButtonPressed function if CallOnUpdate is true, and it will call the
 * ButtonPressed function if CallOnStart is true.
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

