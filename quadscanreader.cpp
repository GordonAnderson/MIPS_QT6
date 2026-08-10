// =============================================================================
// quadscanreader.cpp
//
// Implementation of QUADscanReader. See quadscanreader.h for the stream format
// and for why this does not reuse Comms::readData2ADCBuffer.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     August 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "quadscanreader.h"

// A scan cannot exceed QUADscanMAXPOINTS in the firmware. Enforcing the same
// ceiling here means a corrupted length field cannot make the reader allocate
// a large buffer and then sit waiting for data that will never arrive.
#define QUADscanMAXPOINTS 2000

QUADscanReader::QUADscanReader(QObject *parent) : QObject(parent)
{
    reset();
}

void QUADscanReader::reset(void)
{
    state     = QUADwaitingForHeader;
    last      = 0;
    headerPtr = 0;
    numPoints = 0;
    scanNum   = 0;
    lastScan  = false;
    aeRun     = 0;
    raw.clear();
    for(int i = 0; i < 6; i++) header[i] = 0;
}

/*! \brief Emit the scan currently held in raw and prepare for the next one.
 *
 * On a normal completion raw holds exactly numPoints points. On an abort it
 * holds fewer, because the firmware breaks out of the point loop without
 * sending the final point. Both cases are emitted; the caller decides whether a
 * partial spectrum is useful.
 */
void QUADscanReader::finishScan(bool complete)
{
    QVector<qint32> points;
    int             n = raw.size() / 4;

    if((raw.size() % 4) != 0)
        emit frameError(QString("QSCAN data block is %1 bytes, not a multiple of 4")
                        .arg(raw.size()));
    if(complete && (n != numPoints))
        emit frameError(QString("QSCAN scan %1 completed with %2 points, header said %3")
                        .arg(scanNum).arg(n).arg(numPoints));

    points.reserve(n);
    for(int i = 0; i < n; i++)
    {
        // Little endian, signed. Assembled byte by byte rather than cast through
        // a pointer so the result does not depend on host alignment or endianness.
        quint32 v = (quint32)(quint8)raw[i*4]           |
                   ((quint32)(quint8)raw[i*4 + 1] <<  8) |
                   ((quint32)(quint8)raw[i*4 + 2] << 16) |
                   ((quint32)(quint8)raw[i*4 + 3] << 24);
        points.append((qint32)v);
    }

    emit scanReady(scanNum, points, complete);

    raw.clear();
    aeRun = 0;

    // An abort ends the whole QSCAN command, not just the current scan: the
    // firmware's outer loop tests the aborted flag and exits. Treat it as done
    // regardless of the last flag in the header.
    if(lastScan || !complete)
    {
        state = QUADdone;
        emit allScansDone();
    }
    else state = QUADwaitingForHeader;
}

void QUADscanReader::processData(const QByteArray &data)
{
    for(int i = 0; i < data.size(); i++)
    {
        quint8 c = (quint8)data[i];

        switch(state)
        {
        case QUADwaitingForHeader:
            // Same match as readData2ADCBuffer: the last two bytes of the
            // 0x55 x4, 0xAA preamble are enough to identify the start.
            if((last == 0x55) && (c == QUADscanHDRBYTE))
            {
                state     = QUADreadingHeader;
                headerPtr = 0;
            }
            break;

        case QUADreadingHeader:
            header[headerPtr++] = c;
            if(headerPtr >= 6)
            {
                numPoints = (int)header[0] | ((int)header[1] << 8) | ((int)header[2] << 16);
                scanNum   = (int)header[3] | ((int)header[4] << 8);
                lastScan  = (header[5] == 0xFF);
                raw.clear();
                aeRun = 0;
                if((numPoints < 1) || (numPoints > QUADscanMAXPOINTS))
                {
                    emit frameError(QString("QSCAN header point count %1 is out of range, resynchronising")
                                    .arg(numPoints));
                    state = QUADwaitingForHeader;
                    break;
                }
                raw.reserve(numPoints * 4);
                state = QUADreadingData;
            }
            break;

        case QUADreadingData:
            // The trailer is searched for on every byte rather than only after
            // numPoints*4 bytes have arrived, because an aborted scan sends a
            // short data block. See the header for why a false match on point
            // data is not possible.
            if((aeRun >= 4) && ((c == QUADscanTRAILEROK) || (c == QUADscanTRAILERABRT)))
            {
                raw.chop(4);                    // drop the 0xAE bytes already appended
                finishScan(c == QUADscanTRAILEROK);
                break;
            }
            raw.append((char)c);
            if(c == QUADscanTRAILERAE) aeRun++;
            else aeRun = 0;
            // Guard against a desynchronised stream growing without bound. The
            // +8 allows for the trailer bytes that are appended before they are
            // recognised as a trailer.
            if(raw.size() > ((numPoints * 4) + 8))
            {
                emit frameError("QSCAN data block overran the header point count, resynchronising");
                raw.clear();
                aeRun = 0;
                state = QUADwaitingForHeader;
            }
            break;

        case QUADdone:
        default:
            break;
        }
        last = c;
    }
}
