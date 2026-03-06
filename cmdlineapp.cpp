/*

 This file contains the implementation of two classes:
 - cmdlineapp: A dialog for interacting with an external command-line application.
 - extProcess: A wrapper for QProcess to manage external processes. This class uses the
   cmdlineapp dialog for user interaction.

 cmdlineapp class: a Qt dialog designed to wrap and interact with an external command-line application
 (a data acquisition program).

 The implementation relies heavily on QProcess for asynchronous communication and QTimer for sequencing
 commands and handling external program prompts.

 When process.start(appPath) is called in the Execute function, the operating system launches the
 target application as an entirely new and independent process. It runs outside the memory space of
 the Qt application.

 The QProcess object itself is not blocking the main thread of the MIPS Qt application. It monitors the
 standard input/output (stdio) of the external process and emits signals (readyReadStandardOutput,
 finished, etc.) when new data is available or the process status changes.

 When this class is instantiated the parent can define the following strings that will be used to
 control the interaction with the external application:
 - ReadyMessage: When this string is detected in the external application's output,
   the Ready() signal is emitted indicating that the application is ready for user input
   or a trigger event.
 - InputRequest: When this string is detected, it indicates that the external application
   is prompting for user input. A timer (responseTimer) is started to automatically send
   a default response ('N') if the user does not respond within a set time.
 - ContinueMessage: When this string is detected, it indicates that the external application
   has completed its data acquisition cycle. The AcquireFinishing() method is called to handle
   post-acquisition tasks, and the AppCompleted() signal is emitted.
 - fileName: If defined, this specifies a file path where the entire terminal output
   will be saved when the acquisition cycle completes.
 This class will generate several events/signals that the parent can connect to for handling various stages
 of the acquisition process:
 - Ready(): Emitted when the external application signals it is ready for input.
 - AppCompleted(): Emitted when the external application has completed its acquisition cycle.
 - DialogClosed(): Emitted when the dialog is closed by the user, allowing the parent to clean up references.

 */

#include "cmdlineapp.h"
#include "ui_cmdlineapp.h"

// Constructor: Initializes the dialog, sets up UI elements, initializes internal state variables,
// and establishes connections between internal timers, the external process, and user input elements.
cmdlineapp::cmdlineapp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cmdlineapp)
{
    ui->setupUi(this);
    // Hide the plot area initially, as it's only shown when data plotting is requested.
    ui->plot->hide();

    // Initialize state variables related to the external application's messaging protocol.
    ReadyMessage = "";
    InputRequest = "";
    ContinueMessage = "";
    fileName = "";
    title = "Acquire";

    // Initialize QTimers for handling responses and command sequencing.
    responseTimer = new QTimer;
    messageTimer = new QTimer;

    // Clear internal message queue and ensure the QProcess is closed.
    messages.clear();
    process.close();

    // Set the initial window title.
    cmdlineapp::setWindowTitle(title);

    // Connect the message timer timeout to process the next command in the queue.
    connect(messageTimer, SIGNAL(timeout()), this, SLOT(messageProcessor()));

    // Connect the response timer timeout to send a default 'N' (No) response
    // if the external application prompts for input and the timer expires.
    connect(responseTimer, SIGNAL(timeout()), this, SLOT(sendNO()));

    // Connect QProcess signals to read output/error streams and detect termination.
    connect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(readProcessOutput()));
    connect(&process,SIGNAL(readyReadStandardError()),this,SLOT(readProcessOutput()));
    connect(&process,SIGNAL(finished(int)),this,SLOT(AppFinished()));

    // Connect the command input line's 'Enter' key press (editingFinished) to trigger
    // sending the message to the external process.
    connect(ui->leCommand,SIGNAL(editingFinished()),this,SLOT(readMessage()));
}

// Destructor: Cleans up the dynamically allocated UI object.
cmdlineapp::~cmdlineapp()
{
    delete ui;
}

// Public slot/method that handles closing the dialog, typically called when the user hits the
// window's close button. It ensures the external process is terminated and emits a signal.
void cmdlineapp::reject()
{
    process.close();
    emit DialogClosed();
    delete this;
}

// Public slot/method to dismiss the dialog, functionally identical to reject() in this implementation.
void cmdlineapp::Dismiss()
{
    process.close();
    emit DialogClosed();
    delete this;
}

// Public method to append a new line of text to the terminal display widget.
void cmdlineapp::AppendText(QString message)
{
    ui->txtTerm->appendPlainText(message);
}

// Public method to clear the terminal display and reset the window title.
void cmdlineapp::Clear(void)
{
    cmdlineapp::setWindowTitle(title);
    ui->txtTerm->clear();
}

// Public method to start the external command-line application or restart it if already running.
// It uses platform-specific commands (e.g., /bin/bash on Mac) to execute the target application.
void cmdlineapp::Execute(void)
{
    QStringList arguments;

    // Update the window title to include the path of the running application.
    cmdlineapp::setWindowTitle(title + ", " + appPath);

    if(process.isOpen())
    {
        // If the process is open, it means it's waiting for a command to restart the loop (e.g., 'Y').
        ui->txtTerm->appendPlainText(title + " loop is restarting...\n");
        // Enqueue commands ('Y' and the filename path) to send to the running process.
        messages.append("Y");
        messages.append(appPathNoOptions);
        // Start the timer to sequentially send the queued messages.
        messageTimer->start(2000);
        return;
    }

    // Standard application start procedure:

    // For Mac, run the app through bash with the -c flag.
#if defined(Q_OS_MAC)
    arguments << "-c";
    arguments << appPath;
    process.start("/bin/bash",arguments);
// For other platforms (like Windows/Linux), just start the application directly.
#else
    process.start(appPath);
#endif

    ui->txtTerm->appendPlainText("Application should start soon...\n");
    // Wait indefinitely for the process to start (or until an error occurs).
    process.waitForStarted(-1);
}

// Slot/method to configure the plotting widget based on a comma-separated message received
// from the external application (Plot, Title, Y-Axis Label, X-Axis Label, X-Range Max).
void cmdlineapp::setupPlot(QString mess)
{
    QStringList reslist = mess.split(",");
    // Ensure the message has the expected number of plot setup parameters.
    if(reslist.count() != 5) return;

    // Clear existing graphs and add a new one.
    ui->plot->clearGraphs();
    ui->plot->addGraph();

    // Clean up previous title element if it exists in row 0.
    if( ui->plot->plotLayout()->rowCount() > 1)
    {
        ui->plot->plotLayout()->removeAt(ui->plot->plotLayout()->rowColToIndex(0, 0));
        ui->plot->plotLayout()->simplify();
    }

    // Insert a new row and add the plot title (reslist[1]).
    ui->plot->plotLayout()->insertRow(0);
    ui->plot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->plot, reslist[1], QFont("sans", 12, QFont::Bold)));

    // Set axis labels (Y-axis: reslist[2], X-axis: reslist[3]).
    ui->plot->xAxis->setLabel(reslist[3]);
    ui->plot->yAxis->setLabel(reslist[2]);

    // Set X-axis range (starting at 0 to maximum specified in reslist[4]).
    ui->plot->xAxis->setRange(0, reslist[4].toInt());
    // Set a default Y-axis range (0 to 100).
    ui->plot->yAxis->setRange(0, 100);

    ui->plot->replot();
    // Make the plot visible.
    ui->plot->show();
}

// Slot/method to receive and plot a single data point (X, Y coordinates) on the currently active graph.
// The message is expected to be a comma-separated string "X_Value,Y_Value".
void cmdlineapp::plotDataPoint(QString mess)
{
    QStringList reslist = mess.split(",");
    // Skip if the plot widget is hidden.
    if(ui->plot->isHidden()) return;

    // Check for the expected "X,Y" format.
    if(reslist.count()==2)
    {
        // Add the data point to the first graph (index 0).
        ui->plot->graph(0)->addData(reslist[0].toDouble(), reslist[1].toDouble());
        // Rescale the Y-axis to fit the new data point.
        ui->plot->yAxis->rescale(true);
        ui->plot->replot();
    }
}

// Private method to send a given string command to the external application via its standard input (stdin),
// then append that string to the local terminal display.
void cmdlineapp::sendString(QString mess)
{
    // Write the string data to the QProcess's standard input.
    process.write((char*)mess.data());

    // Update the local terminal display.
    ui->txtTerm->moveCursor (QTextCursor::End);
    ui->txtTerm->insertPlainText (mess);
    ui->txtTerm->moveCursor (QTextCursor::End);
    // Clear the command line input box.
    ui->leCommand->setText("");
}

// Slot/method executed by the responseTimer timeout. It sends a "N\n" (No followed by a newline)
// command to the external process and stops the timer, typically used to answer a timed prompt negatively.
void cmdlineapp::sendNO(void)
{
    ui->leCommand->setText("N");
    emit ui->leCommand->editingFinished();
    responseTimer->stop();
}

// Slot/method connected to the QProcess's readyReadStandardOutput and readyReadStandardError signals.
// It reads all available data from the process, checks for predefined control messages (Ready, Continue, InputRequest),
// and processes incoming lines for potential plot data before appending everything to the terminal view.
void cmdlineapp::readProcessOutput(void)
{
    QString mess;
    static QString messline="";
    static bool ploting = false;

    // Read standard output data.
    mess = process.readAllStandardOutput();

    // Check for predefined control messages from the application:
    if(ReadyMessage != "") if(mess.contains(ReadyMessage)) emit Ready();
    if(ContinueMessage != "")
    {
        if(mess.contains(ContinueMessage))
        {
            // If the "ContinueMessage" is found, the acquire process finished successfully.
            AcquireFinishing();
            emit AppCompleted();
        }
    }
    if(InputRequest != "")
    {
        if(mess.contains(InputRequest))
        {
            // If an "InputRequest" prompt is detected, start the reply timer (to send 'N' if no user input).
            responseTimer->start(5000);
        }
    }

    // Buffer incomplete lines and process complete lines (delimited by '\n').
    messline += mess;
    if(messline.contains("\n"))
    {
        QStringList messlinelist;
        messlinelist = messline.split("\n");
        // The last part might be an incomplete line, so it's stored back into messline.
        messline = "";
        for(int i=1;i<messlinelist.count();i++) messline += messlinelist[i];

        // Process each full line received.
        for(int i=0; i<messlinelist.count();i++)
        {
            // If we are currently in plotting mode, treat the line as a data point.
            if(ploting) plotDataPoint(messlinelist[i]);

            // Check if the line is a plot setup command ("Plot,...").
            if(messlinelist[i].contains("Plot,"))
            {
                setupPlot(messlinelist[i]);
                ploting = true; // Enter plotting mode.
            }
        }
    }

    // Append the entire raw output (including incomplete lines) to the terminal window.
    ui->txtTerm->moveCursor (QTextCursor::End);
    ui->txtTerm->insertPlainText (mess);
    ui->txtTerm->moveCursor (QTextCursor::End);

    // Read and append standard error data immediately.
    ui->txtTerm->moveCursor (QTextCursor::End);
    ui->txtTerm->insertPlainText (process.readAllStandardError());
    ui->txtTerm->moveCursor (QTextCursor::End);
}

// Slot/method connected to the QLineEdit's editingFinished signal (i.e., when the user hits Enter).
// It stops the automatic reply timer, sends the typed command to the external process, and clears the input box.
void cmdlineapp::readMessage(void)
{
    // Stop the auto-reply timer (user input takes precedence).
    responseTimer->stop();

    // Write the command string followed by a newline character to the process's standard input.
    process.write(ui->leCommand->text().toStdString().c_str());
    process.write("\n");

    // Echo the user input to the terminal window.
    ui->txtTerm->moveCursor (QTextCursor::End);
    ui->txtTerm->insertPlainText (ui->leCommand->text() + "\n");
    ui->txtTerm->moveCursor (QTextCursor::End);

    // Clear the command line input box.
    ui->leCommand->setText("");
}

// Private method called when the external application signals that the data acquisition cycle is complete
// (via ContinueMessage or AppFinished). It saves the entire terminal contents to a file if a filename was defined.
void cmdlineapp::AcquireFinishing(void)
{
    // Check if a file name has been set for logging/data saving.
    if(fileName != "")
    {
        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            // Write the entire contents of the terminal window to the file.
            QTextStream stream(&file);
            // QDateTime variable is declared but not used in stream output.
            //QDateTime dateTime = QDateTime::currentDateTime();
            stream << ui->txtTerm->toPlainText();
            file.close();
        }
    }

    // Hide the plot display.
    ui->plot->hide();
    ui->txtTerm->appendPlainText("App signaled its finished!\n");
}

// Slot/method connected to the QProcess's finished signal. It finalizes the acquisition process,
// ensures the process handle is closed, and emits a signal indicating completion.
void cmdlineapp::AppFinished(void)
{
    AcquireFinishing();
    process.close();
    emit AppCompleted();
}

// Slot/method executed by the messageTimer to process sequential commands stored in the 'messages' queue.
// It takes the first message, sends it as a command, and removes it from the queue, repeating until the queue is empty.
void cmdlineapp::messageProcessor(void)
{
    if(messages.count() != 0)
    {
        // Set the command line text field with the message from the queue.
        ui->leCommand->setText(messages[0]);
        // Call readMessage to simulate the user hitting Enter (sending the command).
        readMessage();
        // Remove the processed message.
        messages.removeAt(0);
    }
    else
    {
        // Stop the timer when the command queue is empty.
        messageTimer->stop();
    }
}

// extProcess class: A wrapper for managing an external process using cmdlineapp.
extProcess::extProcess(QWidget *parent, QString name, QString program)
{
    this->name    = name;
    this->program = program;
    cla = new cmdlineapp(parent);
    cla->title = name;
    cla->setWindowTitle(name);
    cla->appPath = program;
    status = "Idle";
    readyScript = "";
    completeScript = "";
    // Connect signals from cmdlineapp to handle events as needed.
    connect(cla, SIGNAL(Ready()), this, SLOT(slotExternalProcessReady()));
    connect(cla, SIGNAL(AppCompleted()), this, SLOT(slotExternalProcessCompleted()));
    connect(cla, SIGNAL(DialogClosed()), this, SLOT(slotExternalProcessDialogClosed()));
}

extProcess::~extProcess()
{
//    disconnect(cla, SIGNAL(Ready()), this, SLOT(slotExternalProcessReady()));
//    disconnect(cla, SIGNAL(AppCompleted()), this, SLOT(slotExternalProcessCompleted()));
//    disconnect(cla, SIGNAL(DialogClosed()), this, SLOT(slotExternalProcessDialogClosed()));
//    delete cla;
}

void extProcess::slotExternalProcessReady(void)
{
    // Handle the external process ready event.
    status = "Ready";
    if(readyScript != "") emit change(readyScript);
}

void extProcess::slotExternalProcessCompleted(void)
{
    // Handle the external process completed event.
    status = "Completed";
    if(completeScript != "") emit change(completeScript);
}

void extProcess::slotExternalProcessDialogClosed(void)
{
    // Handle the external process dialog closed event.
    emit DialogClosed(name);
}

QString extProcess::ProcessCommand(QString cmd)
{
    if(cmd.startsWith(name))
    {
        if(cmd.toUpper() == name.toUpper() + ".SHOW") {cla->show(); return "";}
        if(cmd.toUpper() == name.toUpper() + ".RUN") {cla->Execute(); status="Running"; return "";}
        if(cmd.toUpper() == name.toUpper() + ".STATUS") return(status);
        if(cmd.toUpper() == name.toUpper() + ".DELETE") {cla->Dismiss(); return "";}
        QStringList reslist = cmd.split("=");
        if(reslist.count()==2)
        {
            if(reslist[0].toUpper() == name.toUpper() + ".READYMESSAGE")
            {
                cla->ReadyMessage = reslist[1];
                return "";
            }
            if(reslist[0].toUpper() == name.toUpper() + ".INPUTREQUEST")
            {
                cla->InputRequest = reslist[1];
                return "";
            }
            if(reslist[0].toUpper() == name.toUpper() + ".CONTINUEMESSAGE")
            {
                cla->ContinueMessage = reslist[1];
                return "";
            }
            if(reslist[0].toUpper() == name.toUpper() + ".FILENAME")
            {
                cla->fileName = reslist[1];
                return "";
            }
            if(reslist[0].toUpper() == name.toUpper() + ".READYSCRIPT")
            {
                readyScript = reslist[1];
                return "";
            }
            if(reslist[0].toUpper() == name.toUpper() + ".COMPLETESCRIPT")
            {
                completeScript = reslist[1];
                return "";
            }
        }
    }
    return "?";
}

