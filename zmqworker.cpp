#include "zmqworker.h"

/*

mips.ZMQ("begin,tcp://localhost:5555")
mips.ZMQ("Request,Hello server, this is GAA")
mips.msDelay(1)
mips.ZMQ("Reply")

1. Install zmq

macOS (Homebrew)
  brew install zmq cppzmq

Windows (vcpkg)

  The easiest way to get stable binaries on Windows is via vcpkg:

  git clone [https://github.com/Microsoft/vcpkg.git](https://github.com/Microsoft/vcpkg.git)
  .\vcpkg\bootstrap-vcpkg.bat
  //.\vcpkg\vcpkg install zmq:x64-windows
  .\vcpkg\vcpkg install cppzmq:x64-windows

  vcpkg was install in Users/gaa

2. Project File Configuration (.pro)

  You must tell Qt where to find the headers and libraries. Adjust the paths based on
  where you installed ZMQ.

  # macOS Homebrew paths
  macx
  {
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib -lzmq
  }

  # Windows vcpkg paths (example)
  win32
  {
    INCLUDEPATH += C:/vcpkg/installed/x64-windows/include
    LIBS += -LC:/vcpkg/installed/x64-windows/lib -llibzmq-mt-4_3_4
  }

*/

ZmqReqRep::ZmqReqRep(QObject *parent):
QObject(parent)
{
    mStimeout = 1000;
    workerThread = nullptr;
    worker = nullptr;
    messageReceived.clear();
    messageError.clear();
    serverAddress.clear();
}

ZmqReqRep::~ZmqReqRep()
{
    stop();
    workerThread = nullptr;
    worker = nullptr;
    qDebug() << "ZmqReqRep destroyed and worker thread stopped.";
}

bool ZmqReqRep::begin(QString address)
{
    if (workerThread && workerThread->isRunning()) return false;

    workerThread = new QThread(this);
    worker = new ZmqWorker();
    worker->serverAddress = address;
    worker->moveToThread(workerThread);

    // Clean up thread when worker is deleted
    connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);

    // Connect trigger to Worker
    connect(this, &ZmqReqRep::requestTriggered, worker, &ZmqWorker::sendRequest);

    // Connect Worker results back to class
    connect(worker, &ZmqWorker::replyReceived, this, &ZmqReqRep::messageReceiver);
    connect(worker, &ZmqWorker::errorOccurred, this, &ZmqReqRep::errorMessage);

    workerThread->start();

    qDebug() << "ZmqReqRep initialized and worker thread started.";
    return true;
}

void ZmqReqRep::stop(void)
{
    if (workerThread && workerThread->isRunning()) {
        // Step 1: Tell the thread's exec() loop to stop processing
        workerThread->quit();

        // Step 2: Block current thread until workerThread has finished.
        // This is crucial to prevent memory corruption or "thread still running" crashes.
        if (!workerThread->wait(3000)) { // Wait up to 3 seconds
            // If it still hasn't stopped, you might need to force it
            workerThread->terminate();
            workerThread->wait();
        }
    }
}

QString ZmqReqRep::command(QString cmd)
{
    QStringList cmdList = cmd.trimmed().split(",");
    if(cmdList[0].toUpper() == "REQUEST")
    {
        messageReceived.clear();
        messageError.clear();
        int commaIndex = cmd.indexOf(',');
        if(commaIndex !=-1)
        {
            emit requestTriggered(cmd.mid(commaIndex + 1),mStimeout);
            return "";
        }
        else return "?";
    }
    if(cmdList.count() == 1)
    {
        if(cmd.trimmed().toUpper() == "STOP")
        {
            stop();
            return "";
        }
        else if(cmd.trimmed().toUpper() == "REPLY")
        {
            return messageReceived;
        }
        else if(cmd.trimmed().toUpper() == "ERROR")
        {
            return messageError;
        }
    }
    if(cmdList.count() == 2)
    {
        if(cmdList[0].toUpper() == "BEGIN")
        {
           serverAddress = cmdList[1].trimmed();
           if(begin(serverAddress)) return "";
           else return "?";
        }
    }
    return "?";
}

void ZmqReqRep::sendMessage(QString msg)
{
    emit requestTriggered(msg,mStimeout);
}

void ZmqReqRep::messageReceiver(QString msg)
{
    // Save the message
    qDebug() << "Received reply: " << msg;
    messageReceived = msg;
}

void ZmqReqRep::errorMessage(QString msg)
{
    // Save the message
    qDebug() << "Error reply: " << msg;
    messageError = msg;
}


