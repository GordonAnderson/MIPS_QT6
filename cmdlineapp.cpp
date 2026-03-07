// =============================================================================
// cmdlineapp.cpp
//
// External command-line application dialog classes for the MIPS host app.
//
// Depends on:  cmdlineapp.h, ui_cmdlineapp.h, qcustomplot.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "cmdlineapp.h"
#include "ui_cmdlineapp.h"

// ---------------------------------------------------------------------------
// cmdlineapp
// ---------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Constructor — initialises state, timers, and connects process/UI signals.
// -----------------------------------------------------------------------------
cmdlineapp::cmdlineapp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cmdlineapp)
{
    ui->setupUi(this);
    ui->plot->hide();

    ReadyMessage    = "";
    InputRequest    = "";
    ContinueMessage = "";
    fileName        = "";
    title           = "Acquire";

    responseTimer = new QTimer;
    messageTimer  = new QTimer;

    messages.clear();
    process.close();
    cmdlineapp::setWindowTitle(title);

    connect(messageTimer,  &QTimer::timeout, this, &cmdlineapp::messageProcessor);
    connect(responseTimer, &QTimer::timeout, this, &cmdlineapp::sendNO);

    connect(&process, &QProcess::readyReadStandardOutput, this, &cmdlineapp::readProcessOutput);
    connect(&process, &QProcess::readyReadStandardError,  this, &cmdlineapp::readProcessOutput);
    connect(&process, &QProcess::finished, this, &cmdlineapp::AppFinished);

    connect(ui->leCommand, &QLineEdit::returnPressed, this, &cmdlineapp::readMessage);
}

// ~cmdlineapp — destructor. Releases the UI form.
cmdlineapp::~cmdlineapp()
{
    delete ui;
}

// -----------------------------------------------------------------------------
// reject / Dismiss — terminate the process and signal the parent to clean up.
// -----------------------------------------------------------------------------
void cmdlineapp::reject()
{
    process.close();
    emit DialogClosed();
    delete this;
}

void cmdlineapp::Dismiss()
{
    process.close();
    emit DialogClosed();
    delete this;
}

// AppendText — appends a plain-text message to the terminal display.
void cmdlineapp::AppendText(QString message)
{
    ui->txtTerm->appendPlainText(message);
}

// Clear — resets the window title and clears the terminal display.
void cmdlineapp::Clear(void)
{
    cmdlineapp::setWindowTitle(title);
    ui->txtTerm->clear();
}

// -----------------------------------------------------------------------------
// Execute — starts the external application. If the process is already open
// (waiting to loop), sends 'Y' and the app path to restart the acquisition.
// On Mac the app is launched via /bin/bash -c; on other platforms directly.
// -----------------------------------------------------------------------------
void cmdlineapp::Execute(void)
{
    QStringList arguments;

    cmdlineapp::setWindowTitle(title + ", " + appPath);

    if(process.isOpen())
    {
        ui->txtTerm->appendPlainText(title + " loop is restarting...\n");
        messages.append("Y");
        messages.append(appPathNoOptions);
        messageTimer->start(2000);
        return;
    }

#if defined(Q_OS_MAC)
    arguments << "-c" << appPath;
    process.start("/bin/bash", arguments);
#else
    process.start(appPath);
#endif

    ui->txtTerm->appendPlainText("Application should start soon...\n");
    process.waitForStarted(-1);
}

// -----------------------------------------------------------------------------
// setupPlot — configures the QCustomPlot widget from a comma-separated
// setup message: "Plot,Title,Y-Label,X-Label,X-Max"
// -----------------------------------------------------------------------------
void cmdlineapp::setupPlot(QString mess)
{
    QStringList reslist = mess.split(",");
    if(reslist.count() != 5) return;

    ui->plot->clearGraphs();
    ui->plot->addGraph();

    if(ui->plot->plotLayout()->rowCount() > 1)
    {
        ui->plot->plotLayout()->removeAt(ui->plot->plotLayout()->rowColToIndex(0, 0));
        ui->plot->plotLayout()->simplify();
    }

    ui->plot->plotLayout()->insertRow(0);
    ui->plot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->plot, reslist[1], QFont("sans", 12, QFont::Bold)));

    ui->plot->xAxis->setLabel(reslist[3]);
    ui->plot->yAxis->setLabel(reslist[2]);
    ui->plot->xAxis->setRange(0, reslist[4].toInt());
    ui->plot->yAxis->setRange(0, 100);
    ui->plot->replot();
    ui->plot->show();
}

// -----------------------------------------------------------------------------
// plotDataPoint — adds a single "X,Y" data point to the active graph.
// -----------------------------------------------------------------------------
void cmdlineapp::plotDataPoint(QString mess)
{
    QStringList reslist = mess.split(",");
    if(ui->plot->isHidden()) return;
    if(reslist.count() == 2)
    {
        ui->plot->graph(0)->addData(reslist[0].toDouble(), reslist[1].toDouble());
        ui->plot->yAxis->rescale(true);
        ui->plot->replot();
    }
}

// -----------------------------------------------------------------------------
// sendString — writes a string to the process stdin and echoes it to the terminal.
// -----------------------------------------------------------------------------
void cmdlineapp::sendString(QString mess)
{
    process.write(mess.toUtf8());
    ui->txtTerm->moveCursor(QTextCursor::End);
    ui->txtTerm->insertPlainText(mess);
    ui->txtTerm->moveCursor(QTextCursor::End);
    ui->leCommand->setText("");
}

// -----------------------------------------------------------------------------
// sendNO — auto-reply timer callback: sends 'N' if no user input arrived in time.
// -----------------------------------------------------------------------------
void cmdlineapp::sendNO(void)
{
    ui->leCommand->setText("N");
    emit ui->leCommand->returnPressed();
    responseTimer->stop();
}

// -----------------------------------------------------------------------------
// readProcessOutput — reads stdout/stderr from the process, checks for control
// messages (Ready, Continue, InputRequest), processes plot data lines, and
// appends all output to the terminal display.
// -----------------------------------------------------------------------------
void cmdlineapp::readProcessOutput(void)
{
    QString mess;
    static QString messline = "";
    static bool ploting = false;

    mess = process.readAllStandardOutput();

    if(ReadyMessage != "" && mess.contains(ReadyMessage)) emit Ready();
    if(ContinueMessage != "" && mess.contains(ContinueMessage))
    {
        AcquireFinishing();
        emit AppCompleted();
    }
    if(InputRequest != "" && mess.contains(InputRequest))
        responseTimer->start(5000);

    // Buffer and process complete lines delimited by '\n'
    messline += mess;
    if(messline.contains("\n"))
    {
        QStringList messlinelist = messline.split("\n");
        messline = "";
        for(int i = 1; i < messlinelist.count(); i++) messline += messlinelist[i];

        for(int i = 0; i < messlinelist.count(); i++)
        {
            if(ploting) plotDataPoint(messlinelist[i]);
            if(messlinelist[i].contains("Plot,"))
            {
                setupPlot(messlinelist[i]);
                ploting = true;
            }
        }
    }

    ui->txtTerm->moveCursor(QTextCursor::End);
    ui->txtTerm->insertPlainText(mess);
    ui->txtTerm->moveCursor(QTextCursor::End);
    ui->txtTerm->moveCursor(QTextCursor::End);
    ui->txtTerm->insertPlainText(process.readAllStandardError());
    ui->txtTerm->moveCursor(QTextCursor::End);
}

// -----------------------------------------------------------------------------
// readMessage — sends the current leCommand text to the process and echoes it.
// -----------------------------------------------------------------------------
void cmdlineapp::readMessage(void)
{
    responseTimer->stop();
    process.write(ui->leCommand->text().toStdString().c_str());
    process.write("\n");
    ui->txtTerm->moveCursor(QTextCursor::End);
    ui->txtTerm->insertPlainText(ui->leCommand->text() + "\n");
    ui->txtTerm->moveCursor(QTextCursor::End);
    ui->leCommand->setText("");
}

// -----------------------------------------------------------------------------
// AcquireFinishing — saves terminal contents to fileName (if set) and hides plot.
// -----------------------------------------------------------------------------
void cmdlineapp::AcquireFinishing(void)
{
    if(fileName != "")
    {
        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream << ui->txtTerm->toPlainText();
            file.close();
        }
    }
    ui->plot->hide();
    ui->txtTerm->appendPlainText("App signaled its finished!\n");
}

// AppFinished — slot called when the process exits normally. Runs
// AcquireFinishing(), closes the process, and emits AppCompleted.
void cmdlineapp::AppFinished(void)
{
    AcquireFinishing();
    process.close();
    emit AppCompleted();
}

// -----------------------------------------------------------------------------
// messageProcessor — dequeues and sends one command per timer tick.
// Stops the timer when the queue is empty.
// -----------------------------------------------------------------------------
void cmdlineapp::messageProcessor(void)
{
    if(messages.count() != 0)
    {
        ui->leCommand->setText(messages[0]);
        readMessage();
        messages.removeAt(0);
    }
    else
    {
        messageTimer->stop();
    }
}

// ---------------------------------------------------------------------------
// extProcess
// ---------------------------------------------------------------------------

// extProcess — constructor. Creates a cmdlineapp dialog for the given program,
// connects its Ready/AppCompleted/DialogClosed signals, and initialises status.
extProcess::extProcess(QWidget *parent, QString name, QString program)
{
    this->name    = name;
    this->program = program;
    cla           = new cmdlineapp(parent);
    cla->title    = name;
    cla->setWindowTitle(name);
    cla->appPath  = program;
    status        = "Idle";
    readyScript   = "";
    completeScript = "";

    connect(cla, &cmdlineapp::Ready,        this, &extProcess::slotExternalProcessReady);
    connect(cla, &cmdlineapp::AppCompleted, this, &extProcess::slotExternalProcessCompleted);
    connect(cla, &cmdlineapp::DialogClosed, this, &extProcess::slotExternalProcessDialogClosed);
}

// ~extProcess — destructor. No explicit cleanup needed; cmdlineapp is parented
// to the QWidget and is deleted by Qt's object tree.
extProcess::~extProcess()
{
}

// slotExternalProcessReady — called when the external app emits its ready
// message. Sets status to "Ready" and triggers readyScript if configured.
void extProcess::slotExternalProcessReady(void)
{
    status = "Ready";
    if(readyScript != "") emit change(readyScript);
}

// slotExternalProcessCompleted — called when the external app signals
// completion. Sets status to "Completed" and triggers completeScript if set.
void extProcess::slotExternalProcessCompleted(void)
{
    status = "Completed";
    if(completeScript != "") emit change(completeScript);
}

// slotExternalProcessDialogClosed — called when the cmdlineapp dialog is
// dismissed. Emits DialogClosed with this process's name for cleanup by the caller.
void extProcess::slotExternalProcessDialogClosed(void)
{
    emit DialogClosed(name);
}

// -----------------------------------------------------------------------------
// ProcessCommand — scripting interface for controlling the external process.
// Supports SHOW, RUN, STATUS, DELETE, and property assignment via name=value.
// -----------------------------------------------------------------------------
QString extProcess::ProcessCommand(QString cmd)
{
    if(!cmd.startsWith(name)) return "?";

    if(cmd.toUpper() == name.toUpper() + ".SHOW")   { cla->show();    return ""; }
    if(cmd.toUpper() == name.toUpper() + ".RUN")    { cla->Execute(); status = "Running"; return ""; }
    if(cmd.toUpper() == name.toUpper() + ".STATUS") return status;
    if(cmd.toUpper() == name.toUpper() + ".DELETE") { cla->Dismiss(); return ""; }

    QStringList reslist = cmd.split("=");
    if(reslist.count() == 2)
    {
        QString key = reslist[0].toUpper();
        if(key == name.toUpper() + ".READYMESSAGE")    { cla->ReadyMessage    = reslist[1]; return ""; }
        if(key == name.toUpper() + ".INPUTREQUEST")    { cla->InputRequest    = reslist[1]; return ""; }
        if(key == name.toUpper() + ".CONTINUEMESSAGE") { cla->ContinueMessage = reslist[1]; return ""; }
        if(key == name.toUpper() + ".FILENAME")        { cla->fileName        = reslist[1]; return ""; }
        if(key == name.toUpper() + ".READYSCRIPT")     { readyScript          = reslist[1]; return ""; }
        if(key == name.toUpper() + ".COMPLETESCRIPT")  { completeScript       = reslist[1]; return ""; }
    }
    return "?";
}
