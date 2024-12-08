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

JSengine::JSengine(QWidget *parent)
{
    p = parent;
    ControlPanel *cp;
    cp = (ControlPanel *)p;

    connect(this,SIGNAL(SaveSig(QString)),cp,SLOT(Save(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(LoadSig(QString)),cp,SLOT(Load(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(GetLineSig(QString)),cp,SLOT(GetLine(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(SendStringSig(QString, QString)),cp,SLOT(SendString(QString, QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(SendCommandSig(QString, QString)),cp,SLOT(SendCommand(QString, QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(SendMessSig(QString, QString)),cp,SLOT(SendMess(QString, QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(SystemEnableSig()),cp,SLOT(SystemEnable()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(SystemShutdownSig()),cp,SLOT(SystemShutdown()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(isShutDownSig()),cp,SLOT(isShutDown()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(AcquireSig(QString)),cp,SLOT(Acquire(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(isAcquiringSig()),cp,SLOT(isAcquiring()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(DismissAcquireSig()),cp,SLOT(DismissAcquire()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(msDelaySig(int)),cp,SLOT(msDelay(int)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(statusMessageSig(QString)),cp,SLOT(statusMessage(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(popupMessageSig(QString)),cp,SLOT(popupMessage(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(popupYesNoMessageSig(QString)),cp,SLOT(popupYesNoMessage(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(popupUserInputSig(QString, QString)),cp,SLOT(popupUserInput(QString, QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(UpdateHaltedSig(bool)),cp,SLOT(UpdateHalted(bool)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(UpdateHaltedSigNB(bool)),cp,SLOT(UpdateHalted(bool)));
    connect(this,SIGNAL(WaitForUpdateSig()),cp,SLOT(WaitForUpdate()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(SelectFileSig(QString, QString, QString)),cp,SLOT(SelectFile(QString, QString, QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(ReadCSVfileSig(QString, QString)),cp,SLOT(ReadCSVfile(QString, QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(ReadCSVentrySig(int, int)),cp,SLOT(ReadCSVentry(int, int)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(CommandSig(QString)),cp,SLOT(Command(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(CreatePlotSig(QString, QString, QString, int)),cp,SLOT(CreatePlot(QString, QString, QString, int)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(PlotCommandSig(QString)),cp,SLOT(PlotCommand(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(isCommsSig()),cp,SLOT(isComms()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(MakePathNameUniqueSig(QString)),cp,SLOT(MakePathNameUnique(QString)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(GenerateUniqueFileNameSig()),cp,SLOT(GenerateUniqueFileName()),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(elapsedTimeSig(bool)),cp,SLOT(elapsedTime(bool)),Qt::BlockingQueuedConnection);
    connect(this,SIGNAL(tcpSocketSig(QString, QString, QString)),cp,SLOT(tcpSocket(QString, QString, QString)),Qt::BlockingQueuedConnection);

    QJSValue mips = engine.newQObject(this);
    engine.globalObject().setProperty("mips",mips);
}

QJSValue JSengine::runEngine(void)
{
    isRunning = true;
    result = engine.evaluate(script);
    emit resultReady(result);
    isRunning = false;
    return result;
}

ScriptingConsole::ScriptingConsole(QWidget *parent, Properties *prop) :
    QDialog(parent),
    ui(new Ui::ScriptingConsole)
{
   p = parent;
   ui->setupUi(this);
   properties = prop;

   this->setFixedSize(501,366);
   engine = new JSengine(p);
}

ScriptingConsole::~ScriptingConsole()
{
    delete ui;
}

void ScriptingConsole::paintEvent(QPaintEvent *)
{
    UpdateStatus();
}

void ScriptingConsole::scriptFinished(QJSValue result)
{
    QString markup;
    if(result.isError())
        markup.append("<font color=\"red\">");
    markup.append(result.toString());
    if(result.isError())
        markup.append("</font>");
    ui->lblStatus->setText(markup);
    thread->terminate();
}

void ScriptingConsole::engineDone()
{
    thread->deleteLater();
    engine = new JSengine(p);
}

void ScriptingConsole::RunScript(void)
{
    if(engine->isRunning == true) return;
    if(properties != NULL) properties->Log("Script executed");
    QString script = ui->txtScript->toPlainText();
    engine->script = script;
    connect(engine,&JSengine::resultReady,this,&ScriptingConsole::scriptFinished);
    thread = new QThread(this);
    engine->moveToThread(thread);
    connect(thread, SIGNAL(finished()), this, SLOT(engineDone()));
    connect(thread, SIGNAL(started()), engine, SLOT(runEngine()));
    thread->start();
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

void ScriptingConsole::on_pbAbort_clicked()
{
    if(engine->isRunning == false) return;
    engine->engine.setInterrupted(true);
    thread->terminate();
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
    ScriptTextFixed = false;
    reportResults = true;
}

ScriptButton::~ScriptButton()
{
    if(engine != NULL)
    {
        engine->engine.setInterrupted(true);
    }
}

void ScriptButton::Show(void)
{
    QWidget          *parent;

    pbButton = new QPushButton(Title,p);
    pbButton->setGeometry(X,Y,150,32);
    pbButton->setAutoDefault(false);
    connect(pbButton,SIGNAL(pressed()),this,SLOT(pbButtonPressed()),Qt::QueuedConnection);
    parent = p->parentWidget();
    while(parent->parentWidget() != NULL) parent = parent->parentWidget();
    engine = new JSengine(parent);
    pbButton->installEventFilter(this);
    pbButton->setMouseTracking(true);
}

bool ScriptButton::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, pbButton, pbButton , event)) return true;
    return false;
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
    thread->terminate();
    if(reportResults) QApplication::restoreOverrideCursor();
}

void ScriptButton::engineDone()
{
    thread->deleteLater();
    QWidget *parent = p->parentWidget();
    while(parent->parentWidget() != NULL) parent = parent->parentWidget();
    engine = new JSengine(parent);
    //engine = new JSengine(p);
}

// This event fires when button is pressed, start script and turn
// button on when pressed. If pressed when script is running ask user
// if he would like to abort script.
void ScriptButton::pbButtonPressed(void)
{
    pbButton->setFocus();
    ButtonPressed(true);
}

void ScriptButton::ButtonPressed(bool AlwaysLoad)
{
    QMessageBox msgBox;

    pbButton->setDown(false);
    if(engine->isRunning)
    {
        msgBox.setText("A script is currently running, do you want to abort?");
        msgBox.setInformativeText("Select yes to abort");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if(ret == QMessageBox::No) return;
        //QJSValueList arg;
        //QJSValue v = false;
        //arg.append(v);
        //mips.property("UpdateHalted").call(arg);
        emit engine->UpdateHaltedSigNB(false);
        // Stop the script
        if(engine->isRunning == false) return;
        engine->engine.setInterrupted(true);
        thread->terminate();
        if(properties != NULL) properties->Log("Script aborted by button");
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
        // Start the script in a new thread
        engine->script = ScriptText;
        connect(engine,&JSengine::resultReady,this,&ScriptButton::scriptFinished);
        thread = new QThread( );
        engine->moveToThread(thread);
        connect(thread, SIGNAL(finished()), this, SLOT(engineDone()));
        connect(thread, SIGNAL(started()), engine, SLOT(runEngine()));
        thread->start();
    }
}

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
    QStringList resList = cmd.split("=");
    if((resList.count()==2) && (resList[0].trimmed() == title.trimmed()))
    {
        if(resList[1].trimmed().toUpper() == "ABORT")
        {
            // Here to abort the script if its running
            emit engine->UpdateHaltedSigNB(false);
            // Stop the script
            if(engine->isRunning == false) return "";
            engine->engine.setInterrupted(true);
            thread->terminate();
            if(properties != NULL) properties->Log("Script aborted by another script");
            return "";
        }
    }
    return("?");
}

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

