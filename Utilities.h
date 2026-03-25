// =============================================================================
// Utilities.h
//
// Declarations for general-purpose UI utility functions and the
// MySemaphoreLocker RAII helper class used across the MIPS host application.
//
// Depends on:  Qt Widgets, Qt Events, QSemaphore
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef UTILITIES_H
#define UTILITIES_H

#include <QObject>
#include <QEvent>
#include <QMouseEvent>
#include <qframe.h>
#include <QGroupBox>
#include <QPushButton>
#include <QTabWidget>
#include <QLineEdit>
#include <QApplication>
#include <QWidget>
#include <QSemaphore>

// Utility functions — see Utilities.cpp for full documentation
bool moveWidget(QObject *obj, QWidget *frame, QObject *hook, QEvent *event);
bool adjustValue(QObject *obj, QLineEdit *Vsp, QEvent *event, float multiplier);
int  generateRandomInt(int min, int max);

// -----------------------------------------------------------------------------
// MySemaphoreLocker
//
// RAII wrapper for QSemaphore. Acquires numPermits on construction and releases
// them on destruction, ensuring the semaphore is always released even if an
// exception is thrown. Use in place of manual acquire/release pairs.
//
// Example:
//   MySemaphoreLocker lock(&mySemaphore);   // acquires 1 permit
//   // ... do work ...
//   // permit released automatically when lock goes out of scope
// -----------------------------------------------------------------------------
class MySemaphoreLocker
{
public:
    explicit MySemaphoreLocker(QSemaphore* semaphore, int numPermits = 1)
        : m_semaphore(semaphore), m_numPermits(numPermits)
    {
        if (m_semaphore) {
            m_semaphore->acquire(m_numPermits);
            m_locked = true;
        } else {
            m_locked = false;
        }
    }

    ~MySemaphoreLocker()
    {
        if (m_semaphore && m_locked) {
            m_semaphore->release(m_numPermits);
        }
    }

    // Non-copyable — semaphore ownership must not be shared
    MySemaphoreLocker(const MySemaphoreLocker&) = delete;
    MySemaphoreLocker& operator=(const MySemaphoreLocker&) = delete;

private:
    QSemaphore* m_semaphore;
    int         m_numPermits;
    bool        m_locked;
};

#endif // UTILITIES_H
