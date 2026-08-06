// =============================================================================
// quadscanreader.h
//
// QUADscanReader — binary stream reader for the firmware resident QUAD m/z
// scan (QSCAN). Consumes raw bytes from the serial port or TCP socket and
// emits one signal per completed scan.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     August 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
//
// -----------------------------------------------------------------------------
// Why this is not Comms::readData2ADCBuffer
// -----------------------------------------------------------------------------
// The QSCAN stream uses the same framing convention as the ADC vector stream,
// but it is not the same stream and the existing state machine cannot read it:
//
//   1. Point width. ADC vectors are 16 bit samples and readData2ADCBuffer ends
//      the data block at (Vlength * 2) bytes. QSCAN points are 32 bit raw sums,
//      so the block is (numPoints * 4) bytes. Reusing the ADC reader stops half
//      way through the data and then hunts for a trailer inside the payload.
//
//   2. Abort terminator. readData2ADCBuffer recognises only 0xAE,0xEA. QSCAN
//      terminates an aborted scan with 0xAE x4 followed by 0xEB. The ADC reader
//      never matches it and waits forever.
//
//   3. Short blocks on abort. When a scan is aborted the firmware breaks out of
//      the point loop and does NOT send the final point, so fewer than numPoints
//      points arrive. Any parser that waits for the full byte count before
//      looking for a trailer will hang on every aborted scan. This reader scans
//      for the trailer signature continuously and treats the header point count
//      as an upper bound rather than a promise.
//
// -----------------------------------------------------------------------------
// Why scanning for the trailer signature is safe
// -----------------------------------------------------------------------------
// The trailer is four 0xAE bytes followed by a terminator, so a false match
// requires 0xAE in the most significant byte of a point. Points are sums of
// ADCnumsamples unsigned 12 bit conversions, so the most significant byte
// reaches 0xAE only at a sum of 0xAE000000, which needs about 713,000 samples.
// ADCbuffer is a uint16_t array in the Due's 96 kB of SRAM, which caps the
// sample count near 40,000 and the sum near 0x09C35F58. The high byte of a
// valid point is therefore always small and can never be 0xAE.
//
// Note that SADCSAMPS is a bare CMDint with no range check, so this bound is a
// property of the available RAM, not of an enforced limit.
//
// -----------------------------------------------------------------------------
// Stream format, from QUADscan.cpp
// -----------------------------------------------------------------------------
//   Per scan header:  0x55 0x55 0x55 0x55 0xAA
//                     numPoints  3 bytes, little endian
//                     scanNum    2 bytes, little endian, zero based
//                     lastFlag   1 byte, 0xFF on the final scan else 0x00
//   Data:             numPoints x 4 bytes, little endian, signed raw sum.
//                     The sum is NOT divided by ADCnumsamples; the host divides
//                     if it wants an average.
//   Trailer:          0xAE 0xAE 0xAE 0xAE then
//                     0xEA  normal completion
//                     0xEB  scan was aborted
//
// -----------------------------------------------------------------------------
// Integration with Comms
// -----------------------------------------------------------------------------
// Follow the GetADCbuffer/ADCrelease pattern: disconnect readData2RingBuffer
// from the readyRead signals for the duration of the scan, feed every byte to
// processData(), then reconnect. Sketch:
//
//     reader->reset();
//     disconnect(&client, &QTcpSocket::readyRead, nullptr, nullptr);
//     disconnect(serial,  &QSerialPort::readyRead, nullptr, nullptr);
//     connect(serial, &QSerialPort::readyRead, this, &MyClass::pump);
//     SendCommand("QSCAN," + QString::number(module) + "\n");
//     // ... in pump():  reader->processData(serial->readAll());
//     // ... on allScansDone(): restore readData2RingBuffer as ADCrelease does
//
// QSCAN sends an ACK before the first header, so consume that first. QSCAN also
// blocks the firmware for the whole scan: no other command is answered until it
// finishes. Write 0x1B (ESC) to the port to abort.
// =============================================================================
#ifndef QUADSCANREADER_H
#define QUADSCANREADER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QVector>
#include <QtGlobal>

//! Framing states for the QSCAN binary reader.
enum QUADreadState
{
    QUADwaitingForHeader,   //!< Hunting for 0x55,0xAA
    QUADreadingHeader,      //!< Collecting the 6 header bytes
    QUADreadingData,        //!< Collecting points, watching for the trailer
    QUADdone                //!< Last scan seen, further bytes ignored
};

//! Terminator bytes, mirroring QUADscan.h
#define QUADscanHDRBYTE     0xAA
#define QUADscanTRAILERAE   0xAE
#define QUADscanTRAILEROK   0xEA
#define QUADscanTRAILERABRT 0xEB
#define QUADscanABORTCHAR   0x1B

/*! \class QUADscanReader
 * \brief Reassembles the QSCAN binary stream into per scan point vectors.
 *
 * Feed it bytes with processData(); it emits scanReady() once per scan. The
 * reader is byte oriented and holds no assumptions about chunk boundaries, so
 * a header, a data block and a trailer may be split across any number of
 * readAll() results.
 */
class QUADscanReader : public QObject
{
    Q_OBJECT

public:
    explicit QUADscanReader(QObject *parent = nullptr);

    //! Clear all state. Call before every QSCAN, not just the first: the
    //! reader must not carry a partial frame from an aborted run into the next.
    void reset(void);

    //! Consume a block of received bytes.
    void processData(const QByteArray &data);

    //! True once the scan marked last has been received.
    bool isDone(void) const { return state == QUADdone; }

    //! Point count advertised by the most recent header.
    int expectedPoints(void) const { return numPoints; }

signals:
    //! One completed scan. complete is false if the firmware reported an abort,
    //! in which case points holds the partial spectrum received before the
    //! abort took effect.
    void scanReady(int scanNum, const QVector<qint32> &points, bool complete);

    //! The scan marked as last has been received, or a scan was aborted.
    //! Restore normal ring buffer routing here.
    void allScansDone(void);

    //! Framing problem. The reader resynchronises by returning to header hunt.
    void frameError(const QString &message);

private:
    void finishScan(bool complete);

    QUADreadState state;
    quint8        last;          // previous byte, for the 0x55,0xAA match
    int           headerPtr;     // 0..5 while reading the header
    quint8        header[6];
    int           numPoints;
    int           scanNum;
    bool          lastScan;
    QByteArray    raw;           // accumulated point bytes
    int           aeRun;         // trailing run of 0xAE bytes in raw
};

#endif // QUADSCANREADER_H
