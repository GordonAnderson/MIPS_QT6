// =============================================================================
// ringbuffer.cpp
//
// Fixed-size circular character buffer used by the Comms and TCPserver classes
// to receive and buffer incoming serial/TCP data from MIPS.
//
// Buffer capacity is rbSIZE (10 000) characters. Lines are counted by tracking
// newline characters so callers can poll numLines() before calling getline().
// ACK (0x06), NAK (0x15), and CR ('\r') characters can be stripped on read.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2015
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "ringbuffer.h"
#include <QElapsedTimer>

RingBuffer::RingBuffer(void)
{
    clear();
}

void RingBuffer::clear(void)
{
    head  = 0;
    tail  = 0;
    count = 0;
    lines = 0;
}

// size — returns the number of characters currently in the buffer.
int RingBuffer::size(void)
{
    return count;
}

// available — returns the number of free slots remaining in the buffer.
int RingBuffer::available(void)
{
    return (rbSIZE - count);
}

// numLines — returns the number of newline-terminated lines in the buffer.
int RingBuffer::numLines(void)
{
    return lines;
}

// getch — removes and returns one character from the tail of the buffer.
// If strip is true, ACK (0x06), NAK (0x15), and CR ('\r') are silently
// discarded and the next character is returned instead.
// Returns 0 if the buffer is empty.
char RingBuffer::getch(bool strip)
{
    char c;

    while(true)
    {
        if(count == 0) return(0);
        c = buffer[tail++];
        if(tail >= rbSIZE) tail = 0;
        count--;
        if(c == '\n') lines--;
        if(strip && (c == 0x06 || c == 0x15 || c == '\r')) continue;
        break;
    }
    return c;
}

// putch — appends one character to the head of the buffer.
// CR ('\r') is silently ignored. Returns -1 if the buffer is full,
// otherwise returns the new character count.
int RingBuffer::putch(char c)
{
    if(c == '\r') return(count);
    if(count >= rbSIZE) return(-1);
    if(c == '\n') lines++;
    buffer[head++] = c;
    if(head >= rbSIZE) head = 0;
    count++;
    return(count);
}

// getline — reads characters from the buffer until a newline is encountered
// and returns the line as a QString (without the newline). ACK/NAK characters
// are discarded. If acknak is non-null it is set true when ACK/NAK is seen.
// Returns an empty string if no complete line is available.
QString RingBuffer::getline(bool *acknak)
{
    QString str = "";
    char c;

    if(lines <= 0) return str;
    while(1)
    {
        c = getch(false);
        if((c == 0x06) || (c == 0x15))
        {
            if(acknak != NULL) *acknak = true;
            continue;
        }
        if(c == '\n') break;
        if(count <= 0) break;
        str += c;
    }
    return str;
}

// putString — appends every character of a QString to the buffer via putch.
void RingBuffer::putString(QString str)
{
    for(int i = 0; i < str.length(); i++)
    {
        char c = str.at(i).toLatin1();
        putch(c);
    }
}
