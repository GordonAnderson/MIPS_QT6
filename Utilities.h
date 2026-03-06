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

bool moveWidget(QObject *obj, QWidget *frame, QObject *hook , QEvent *event);
bool adjustValue(QObject *obj,QLineEdit *Vsp, QEvent *event,float multiplier);
int generateRandomInt(int min, int max);

#include <QSemaphore>

class MySemaphoreLocker
{
public:
    // Constructor: Acquires 'numPermits' from the given semaphore
    explicit MySemaphoreLocker(QSemaphore* semaphore, int numPermits = 1)
        : m_semaphore(semaphore), m_numPermits(numPermits)
    {
        if (m_semaphore) {
            m_semaphore->acquire(m_numPermits);
            m_locked = true;
        } else {
            m_locked = false; // Indicate that no semaphore was provided or acquired
        }
    }

    // Destructor: Releases 'numPermits' back to the semaphore
    ~MySemaphoreLocker()
    {
        if (m_semaphore && m_locked) {
            m_semaphore->release(m_numPermits);
        }
    }

    // You might want to disallow copying and assignment for thread-safe objects
    MySemaphoreLocker(const MySemaphoreLocker&) = delete;
    MySemaphoreLocker& operator=(const MySemaphoreLocker&) = delete;

private:
    QSemaphore* m_semaphore;
    int m_numPermits;
    bool m_locked; // Track if we successfully acquired the semaphore
};

#endif // UTILITIES_H
