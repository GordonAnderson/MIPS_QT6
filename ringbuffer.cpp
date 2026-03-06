/*! \file ringbuffer.cpp
 * \brief Implementation of the RingBuffer class.
 *
 * This file contains the implementation of the RingBuffer class, which is used to manage a
 * ring buffer for storing characters received from MIPS. It provides methods for adding,
 * retrieving, and managing characters and lines within the buffer.
 *
 * \author Gordon Anderson
 * \date   2015-2025
 */
#include "ringbuffer.h"
#include <QElapsedTimer>
#include <QDebug>


/*! \brief RingBuffer is a simple ring buffer implementation.
 * This class is used to store characters received from MIPS.
 * It can hold up to rbSIZE characters and supports basic operations like
 * adding characters, retrieving characters, and managing lines.
 */
RingBuffer::RingBuffer(void)
{
    clear();
}

/*! \brief clear resets the ring buffer to its initial state.
 * This function clears the buffer, resets the head and tail pointers,
 * and sets the count of characters and lines to zero.
 */
void RingBuffer::clear(void)
{
    head = 0;
    tail = 0;
    count = 0;
    lines = 0;
}

/*! \brief size returns the current number of characters in the ring buffer.
 * This function returns the count of characters currently stored in the buffer.
 * @return The number of characters in the buffer.
 */
int RingBuffer::size(void)
{
    return count;
}

/*! \brief available returns the number of free slots in the ring buffer.
 * This function calculates how many more characters can be added to the buffer
 * without exceeding its maximum size.
 * @return The number of available slots in the buffer.
 */
int  RingBuffer::available(void)
{
    return (rbSIZE-count);
}

/*! \brief numLines returns the number of lines in the ring buffer.
 * This function returns the count of lines (terminated by newline characters)
 * currently stored in the buffer.
 * @return The number of lines in the buffer.
 */
int RingBuffer::numLines(void)
{
    return lines;
}

/*! \brief getch retrieves a character from the ring buffer.
 * This function removes a character from the buffer and returns it.
 * If the character is a newline, it decrements the line count.
 * It can optionally strip special characters (0x06, 0x15, and '\r').
 * @param strip If true, special characters are ignored.
 * @return The character retrieved from the buffer, or 0 if the buffer is empty.
 */
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
        if(strip && (c == 0x06 || c == 0x15 || c == '\r')) continue; // ignore special characters
        break; // exit the loop if a valid character is found
    }
    return c;
}

/*! \brief putch adds a character to the ring buffer.
 * This function adds a character to the buffer and increments the count.
 * If the character is a newline, it increments the line count.
 * It returns the new count of characters in the buffer, or -1 if the buffer is full.
 * @param c The character to add to the buffer.
 * @return The new count of characters in the buffer, or -1 if full.
 */
int RingBuffer::putch(char c)
{
    if(c == '\r') return(count);        // ignore \r
    if(count >= rbSIZE) return(-1);
    if(c == '\n')
    {
        lines++;
    }
    buffer[head++] = c;
    if(head >= rbSIZE) head = 0;
    count++;
    return(count);
}

/*! \brief getline retrieves a line from the ring buffer.
 * This function reads characters from the buffer until a newline character is encountered.
 * It returns the line as a QString. If an ACK (0x06) or NAK (0x15) character is received,
 * it can set the acknak parameter to true.
 * @param acknak Pointer to a boolean that will be set to true if an ACK or NAK is received.
 * @return The line read from the buffer as a QString.
 */
QString RingBuffer::getline(bool *acknak)
{
    QString str="";
    char c;

    if(lines <= 0) return str;
    while(1)
    {
        c = getch(false); // Get a character without stripping special characters
        if((c == 0x06) || (c == 0x15))
        {
            if(acknak != NULL)  *acknak = true; // Set acknak to true if an ACK or NAK is received
            continue; // Ignore ACK/NAK characters
        }
        if(c == '\n') break;
        if(count <= 0) break;
        str += c;
    }
    return str;
}

/*! \brief putString adds a string to the ring buffer.
 * This function takes a QString, converts each character to its Latin-1 representation,
 * and adds it to the buffer using putch.
 * @param str The QString to add to the buffer.
 */
void RingBuffer::putString(QString str)
{
    for(int i=0; i<str.length(); i++)
    {
        char c = str.at(i).toLatin1();
        putch(c);
    }
}
