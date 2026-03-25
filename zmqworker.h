// =============================================================================
// zmqworker.h
//
// Declares ZmqReqRep and ZmqWorker.
// See zmqworker.cpp for full documentation and build instructions.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef ZMQWORKER_H
#define ZMQWORKER_H
#define UseZmQ
#ifdef UseZmQ

#include <QObject>
#include <QThread>
#include <QDebug>
#include <zmq.hpp>
#include <string>

class ZmqWorker;

// -----------------------------------------------------------------------------
// ZmqReqRep — thread-safe ZMQ REQ-REP client. Owns a ZmqWorker on a dedicated
// QThread. Commands are dispatched via command() from the scripting system.
// Replies and errors are stored in messageReceived / messageError and retrieved
// with the REPLY / ERROR commands.
// -----------------------------------------------------------------------------
class ZmqReqRep : public QObject
{
    Q_OBJECT

signals:
    void requestTriggered(QString cmd, int mStimeout);

public:
    explicit ZmqReqRep(QObject *parent = nullptr);
    ~ZmqReqRep();
    bool    begin(QString address);
    void    stop(void);
    void    sendMessage(QString msg);
    QString command(QString cmd);

public slots:
    void messageReceiver(QString msg);
    void errorMessage(QString msg);

private:
    QThread   *workerThread;
    ZmqWorker *worker;
    QString    messageReceived;  // last successful reply
    QString    messageError;     // last error string
    QString    serverAddress;
    int        mStimeout;        // send/receive timeout in ms (default 1000)
};

// -----------------------------------------------------------------------------
// ZmqWorker — runs on a dedicated QThread. Maintains a persistent ZMQ REQ
// socket for the lifetime of the thread. On send/receive failure the socket
// is recreated to reset the REQ-REP state machine.
// -----------------------------------------------------------------------------
class ZmqWorker : public QObject
{
    Q_OBJECT

public:
    explicit ZmqWorker(QObject *parent = nullptr)
        : QObject(parent),
          context(1),
          socket(context, ZMQ_REQ),
          isConnected(false) {}

    QString serverAddress;

public slots:
    // sendRequest — sends message to the server and emits replyReceived on
    // success or errorOccurred on timeout / failure.
    void sendRequest(const QString &message, int timeoutMs = 5000)
    {
        try
        {
            if(!isConnected)
            {
                if(serverAddress.isEmpty())
                {
                    emit errorOccurred("Server address is not set.");
                    return;
                }
                socket.set(zmq::sockopt::rcvtimeo, timeoutMs);
                socket.set(zmq::sockopt::sndtimeo, timeoutMs);
                socket.set(zmq::sockopt::linger, 0);
                socket.connect(serverAddress.toStdString().c_str());
                isConnected = true;
            }

            std::string msgStr = message.toStdString();
            zmq::message_t request(msgStr.size());
            memcpy(request.data(), msgStr.c_str(), msgStr.size());

            auto send_result = socket.send(request, zmq::send_flags::none);
            if(!send_result.has_value())
            {
                emit errorOccurred("Failed to send request (Send Timeout).");
                handleSocketError();
                return;
            }

            zmq::message_t reply;
            auto recv_result = socket.recv(reply, zmq::recv_flags::none);
            if(!recv_result.has_value())
            {
                // REQ-REP state machine is stuck after a timeout; must recreate socket
                emit errorOccurred("Server request timed out. No response received.");
                handleSocketError();
                return;
            }

            emit replyReceived(QString::fromStdString(
                std::string(static_cast<char*>(reply.data()), reply.size())));
        }
        catch(const zmq::error_t &e)
        {
            emit errorOccurred(QString("ZMQ Exception: %1").arg(e.what()));
            handleSocketError();
        }
    }

signals:
    void replyReceived(const QString &reply);
    void errorOccurred(const QString &err);

private:
    // handleSocketError — recreates the socket to reset the REQ-REP state
    // machine after a failed or timed-out exchange.
    void handleSocketError()
    {
        socket      = zmq::socket_t(context, ZMQ_REQ);
        isConnected = false;
    }

    zmq::context_t context;
    zmq::socket_t  socket;
    bool           isConnected;
};

#endif // UseZmQ
#endif // ZMQWORKER_H
