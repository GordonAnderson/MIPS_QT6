// =============================================================================
// Utilities.cpp
//
// General-purpose UI utility functions shared across all MIPS host application
// modules. Provides mouse-driven widget dragging, keyboard/scroll wheel value
// adjustment for line edit controls, and a thread-safe random number generator.
//
// Depends on:  Qt Widgets, Qt Events
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "Utilities.h"
#include "properties.h"
#include <random>
#include <ctime>

// Qt key codes for arrow keys used in adjustValue()
static const int KEY_UP_ARROW   = 16777235;
static const int KEY_DOWN_ARROW = 16777237;

// moveWidget — enables right-click drag repositioning of floating widgets.
// Attach to an event filter: returns true if the event was consumed.
bool moveWidget(QObject *obj, QWidget *frame, QObject *hook, QEvent *event)
{
    if((pProps != nullptr) && pProps->ControlPanelEdit)
    {
        if((obj == hook) && (event->type() == QEvent::MouseButtonPress))
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::RightButton)
            {
                if(mouseEvent->position().rx() > 25) return false;
                if(mouseEvent->position().ry() > 25) return false;
                frame->raise();
                if(obj->property("moving").toBool())
                {
                    obj->setProperty("moving", false);
                    // Release the mouse grab when done moving
                    static_cast<QWidget*>(hook)->releaseMouse();
                }
                else
                {
                    obj->setProperty("moving", true);
                    QPoint globalPos = mouseEvent->globalPosition().toPoint();
                    obj->setProperty("grabX", globalPos.x());
                    obj->setProperty("grabY", globalPos.y());
                    // Grab the mouse so all events come here even if cursor
                    // moves outside the hook widget
                    static_cast<QWidget*>(hook)->grabMouse();
                }
                return true;
            }
        }
        if((obj == hook) && (event->type() == QEvent::MouseMove))
        {
            if(obj->property("moving").toBool())
            {
                frame->raise();
                QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
                QPoint globalPos = mouse->globalPosition().toPoint();
                int lastX = obj->property("grabX").toInt();
                int lastY = obj->property("grabY").toInt();
                int dx = globalPos.x() - lastX;
                int dy = globalPos.y() - lastY;
                frame->move(frame->pos().x() + dx, frame->pos().y() + dy);
                obj->setProperty("grabX", globalPos.x());
                obj->setProperty("grabY", globalPos.y());
                return true;
            }
        }
        if((obj == hook) && (event->type() == QEvent::MouseButtonRelease))
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() == Qt::LeftButton)
            {
                obj->setProperty("moving", false);
                static_cast<QWidget*>(hook)->releaseMouse();
                return true;
            }
        }
    }
    return false;
}

// adjustValue — handles up/down arrow key and scroll wheel increment/decrement
// on a QLineEdit setpoint field. multiplier scales the step size: typical values
// are 1.0 (volts), 0.1 (fine), 0.01 (very fine). Modifier keys apply additional
// scaling: Alt=x0.1, Shift=x10, Ctrl=x100. Returns true if the event was consumed.
bool adjustValue(QObject *obj,QLineEdit *Vsp, QEvent *event,float multiplier)
{
    float delta = 0;

    if (((obj == Vsp) && (event->type() == QEvent::KeyPress)) || ((obj == Vsp) && (event->type() == QEvent::Wheel)))
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *key = static_cast<QKeyEvent *>(event);
            if(key->key() == KEY_UP_ARROW)   delta =  0.1;
            if(key->key() == KEY_DOWN_ARROW) delta = -0.1;
            if(QApplication::queryKeyboardModifiers() & QFlags<Qt::KeyboardModifier>(Qt::AltModifier))     delta *= 0.1;
            if(QApplication::queryKeyboardModifiers() & QFlags<Qt::KeyboardModifier>(Qt::ShiftModifier))   delta *= 10;
            if(QApplication::queryKeyboardModifiers() & QFlags<Qt::KeyboardModifier>(Qt::ControlModifier)) delta *= 100;
        }
        else if(event->type() == QEvent::Wheel)
        {
            if((pProps != nullptr) && pProps->ScrollEdit)
            {
                QWheelEvent *wheel = static_cast<QWheelEvent *>(event);
                delta = (float)wheel->angleDelta().ry()/10.0;
            }
        }
        if(delta != 0)
        {
            QString myString;
            if(abs(multiplier) <= 0.01) myString = myString.asprintf("%3.3f", Vsp->text().toFloat() + delta * multiplier);
            else                        myString = myString.asprintf("%3.2f", Vsp->text().toFloat() + delta * multiplier);
            Vsp->setText(myString);
            Vsp->setModified(true);
            emit Vsp->editingFinished();
            return true;
        }
    }
    return false;
}

// Random number generator — seeded once at startup
static std::mt19937 generator(std::time(nullptr));
static std::uniform_int_distribution<> distribution;

// generateRandomInt — returns a uniformly distributed random integer in
// [min, max] inclusive, using the Mersenne Twister seeded at startup.
int generateRandomInt(int min, int max) {
    distribution.param(std::uniform_int_distribution<>::param_type(min, max));
    return distribution(generator);
}
