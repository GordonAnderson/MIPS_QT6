// =============================================================================
// ringbuffer.h
//
// Class declaration for RingBuffer.
// See ringbuffer.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2015
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QString>
#include <QObject>

// Buffer capacity in characters
static const int rbSIZE = 10000;

// -----------------------------------------------------------------------------
// RingBuffer — fixed-size circular character buffer. Thread-unsafe; intended
// for single-threaded use within the Qt event loop. Lines are tracked by
// counting newline characters so callers can check numLines() efficiently.
// -----------------------------------------------------------------------------
class RingBuffer : public QObject
{
    Q_OBJECT

public:
    explicit RingBuffer(void);
    void    clear(void);
    int     size(void);
    int     available(void);
    int     numLines(void);
    char    getch(bool strip = true);
    int     putch(char c);
    QString getline(bool *acknak = NULL);
    void    putString(QString str);

private:
    int buffer[rbSIZE];
    int head;    // index of next write position
    int tail;    // index of next read position
    int count;   // number of characters currently in the buffer
    int lines;   // number of newline characters in the buffer
};

#endif // RINGBUFFER_H
