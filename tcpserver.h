// =============================================================================
// tcpserver.h
//
// Class declaration for TCPserver.
// See tcpserver.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QStatusBar>
#include "ringbuffer.h"

// -----------------------------------------------------------------------------
// TCPserver — single-client TCP server. Buffers incoming data in a RingBuffer
// and emits dataReady() / lineReady() signals. Sends outgoing data via
// sendMessage(). Port defaults to 9999; set before calling listen().
// -----------------------------------------------------------------------------
class TCPserver : public QObject
{
    Q_OBJECT
public:
    explicit TCPserver(QObject *parent = 0);
    ~TCPserver();
    void    listen(void);
    void    sendMessage(QString mess);
    QString readLine(void);
    int     bytesAvailable(void);

    int         port;        // TCP port to listen on (default 9999)
    QStatusBar *statusbar;   // optional status bar for connection messages

signals:
    void dataReady(void);    // emitted when any bytes arrive
    void lineReady(void);    // emitted when a newline-terminated line is available

public slots:
    void newConnection(void);
    void readData(void);
    void disconnected(void);

private:
    RingBuffer  rb;
    QTcpServer *server;
    QTcpSocket *socket;      // current connected client (NULL if none)
};

#endif // TCPSERVER_H
