/*! \file ringbuffer.h
 * \brief RingBuffer class for handling a circular buffer of characters.
 *
 * This class implements a ring buffer to store characters. It supports adding characters,
 * retrieving characters, and reading lines from the buffer.
 */
#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QByteArray>
#include <QString>
#include <QTime>
#include <QApplication>
#include <QtNetwork/QTcpSocket>
#include <QtSerialPort/QSerialPort>

#define rbSIZE 10000

class RingBuffer : public QObject
{
    Q_OBJECT

public:
    explicit RingBuffer(void);
    char    getch(bool strip=true);
    int     putch(char c);
    int     size(void);
    int     available(void);
    int     numLines(void);
    void    clear(void);
    QString getline(bool *acknak=NULL);
    void    putString(QString str);

protected:

private:
    int     buffer[rbSIZE];
    int     head;
    int     tail;
    int     count;
    int     lines;
};

#endif // RINGBUFFER_H

