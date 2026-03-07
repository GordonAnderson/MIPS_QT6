// =============================================================================
// zmqworker.cpp
//
// Implements ZmqReqRep — a Qt wrapper around a ZMQ REQ-REP socket pair that
// runs the blocking send/receive cycle on a dedicated QThread (ZmqWorker).
//
// Usage from the scripting system:
//   mips.ZMQ("begin,tcp://localhost:5555")
//   mips.ZMQ("Request,Hello server")
//   mips.msDelay(1)
//   mips.ZMQ("Reply")         // returns last received reply
//   mips.ZMQ("Error")         // returns last error string
//   mips.ZMQ("Stop")          // shuts down the worker thread
//
// ZMQ installation (required before building with UseZmQ defined):
//   macOS:   brew install zmq cppzmq
//   Windows: vcpkg install cppzmq:x64-windows
//            (vcpkg installed in Users/gaa)
//
// .pro additions:
//   macx   { INCLUDEPATH += /usr/local/include
//             LIBS       += -L/usr/local/lib -lzmq }
//   win32  { INCLUDEPATH += C:/vcpkg/installed/x64-windows/include
//             LIBS       += -LC:/vcpkg/installed/x64-windows/lib -llibzmq-mt-4_3_4 }
//
// Depends on:  zmqworker.h, zmq.hpp
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "zmqworker.h"

ZmqReqRep::ZmqReqRep(QObject *parent) :
    QObject(parent)
{
    mStimeout    = 1000;
    workerThread = nullptr;
    worker       = nullptr;
    messageReceived.clear();
    messageError.clear();
    serverAddress.clear();
}

ZmqReqRep::~ZmqReqRep()
{
    stop();
    workerThread = nullptr;
    worker       = nullptr;
}

// begin — creates and starts the worker thread connected to address.
// Returns false if a thread is already running.
bool ZmqReqRep::begin(QString address)
{
    if(workerThread && workerThread->isRunning()) return false;

    workerThread = new QThread(this);
    worker = new ZmqWorker();
    worker->serverAddress = address;
    worker->moveToThread(workerThread);

    connect(workerThread, &QThread::finished,       worker,       &QObject::deleteLater);
    connect(this,         &ZmqReqRep::requestTriggered, worker,   &ZmqWorker::sendRequest);
    connect(worker,       &ZmqWorker::replyReceived, this,        &ZmqReqRep::messageReceiver);
    connect(worker,       &ZmqWorker::errorOccurred, this,        &ZmqReqRep::errorMessage);

    workerThread->start();
    return true;
}

// stop — quits the worker thread and waits up to 3 seconds for it to finish.
// Terminates forcibly if it does not stop in time.
void ZmqReqRep::stop(void)
{
    if(workerThread && workerThread->isRunning())
    {
        workerThread->quit();
        if(!workerThread->wait(3000))
        {
            workerThread->terminate();
            workerThread->wait();
        }
    }
}

// command — parses a ZMQ command string and dispatches accordingly.
// Supported commands: BEGIN,<addr> | REQUEST,<msg> | REPLY | ERROR | STOP
QString ZmqReqRep::command(QString cmd)
{
    QStringList cmdList = cmd.trimmed().split(",");

    if(cmdList[0].toUpper() == "REQUEST")
    {
        messageReceived.clear();
        messageError.clear();
        int commaIndex = cmd.indexOf(',');
        if(commaIndex != -1)
        {
            emit requestTriggered(cmd.mid(commaIndex + 1), mStimeout);
            return "";
        }
        return "?";
    }
    if(cmdList.count() == 1)
    {
        if(cmd.trimmed().toUpper() == "STOP")  { stop(); return ""; }
        if(cmd.trimmed().toUpper() == "REPLY") { return messageReceived; }
        if(cmd.trimmed().toUpper() == "ERROR") { return messageError; }
    }
    if(cmdList.count() == 2)
    {
        if(cmdList[0].toUpper() == "BEGIN")
        {
            serverAddress = cmdList[1].trimmed();
            return begin(serverAddress) ? "" : "?";
        }
    }
    return "?";
}

void ZmqReqRep::sendMessage(QString msg)
{
    emit requestTriggered(msg, mStimeout);
}

// messageReceiver — saves the most recent reply from the worker.
void ZmqReqRep::messageReceiver(QString msg)
{
    messageReceived = msg;
}

// errorMessage — saves the most recent error string from the worker.
void ZmqReqRep::errorMessage(QString msg)
{
    messageError = msg;
}
