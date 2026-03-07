// =============================================================================
// tcpserver.cpp
//
// Implements TCPserver — a simple single-client TCP server on a configurable
// port (default 9999). Incoming data is buffered in a RingBuffer and signalled
// to the caller via dataReady() and lineReady(). Outgoing data is sent via
// sendMessage(). Only one client socket is kept at a time; a new connection
// closes the previous one.
//
// Depends on:  ringbuffer.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "tcpserver.h"

TCPserver::TCPserver(QObject *parent) :
    QObject(parent)
{
    socket    = NULL;
    statusbar = NULL;
    port      = 9999;
    server    = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

TCPserver::~TCPserver()
{
    if(socket != NULL) socket->close();
    server->close();
}

// listen — starts accepting connections on the configured port. Reports
// success or failure via the status bar if one is set.
void TCPserver::listen(void)
{
    if(!server->listen(QHostAddress::Any, port))
    {
        if(statusbar != NULL) statusbar->showMessage("Server could not start");
    }
    else
    {
        if(statusbar != NULL) statusbar->showMessage("Server started!");
    }
}

// sendMessage — writes a UTF-8 encoded message to the connected client.
// Silently discards bare newlines and calls when no client is connected.
void TCPserver::sendMessage(QString mess)
{
    if(mess == "\n") return;
    if(socket == NULL) return;
    socket->write(mess.toUtf8().constData());
    socket->flush();
}

int TCPserver::bytesAvailable(void)
{
    return(rb.size());
}

// newConnection — accepts the next pending connection. If a client is already
// connected it is closed first. Wires readyRead and disconnected signals.
void TCPserver::newConnection(void)
{
    QTcpSocket *sck = server->nextPendingConnection();
    if(socket != NULL) socket->close();
    socket = sck;
    if(socket == NULL) return;
    connect(socket, SIGNAL(readyRead()),     this, SLOT(readData()));
    connect(socket, SIGNAL(disconnected()),  this, SLOT(disconnected()));
}

void TCPserver::disconnected(void)
{
    disconnect(socket, SIGNAL(readyRead()),    0, 0);
    disconnect(socket, SIGNAL(disconnected()), 0, 0);
    socket->close();
    socket = NULL;
}

// readData — reads available bytes from the socket into the ring buffer,
// capped at the buffer's available space. Emits dataReady() and lineReady()
// as appropriate.
void TCPserver::readData(void)
{
    int space = rb.available();
    int chars = socket->bytesAvailable();
    if(space < chars) chars = space;
    if(chars <= 0) return;
    char *buffer = new char[chars];
    int cr = socket->read(buffer, chars);
    for(int i = 0; i < cr; i++) rb.putch(buffer[i]);
    delete[] buffer;
    if(rb.size()     > 0) emit dataReady();
    if(rb.numLines() > 0) emit lineReady();
}

QString TCPserver::readLine(void)
{
    if(rb.numLines() == 0) return "";
    return rb.getline();
}
