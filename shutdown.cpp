// =============================================================================
// shutdown.cpp
//
// Shutdown — system shutdown/enable toggle button for the MIPS custom control
// panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  shutdown.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "shutdown.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Constructor — stores parent widget and position parameters.
// -----------------------------------------------------------------------------
Shutdown::Shutdown(QWidget *parent, QString name, int x, int y)
    : QWidget(parent)
{
    p     = parent;
    Title = name;
    X     = x;
    Y     = y;
}

// Show — creates the push button and installs the event filter so the button
// can be repositioned by dragging on the control panel background.
void Shutdown::Show(void)
{
    pbShutdown = new QPushButton("System shutdown", p);
    pbShutdown->setGeometry(X, Y, 150, 32);
    pbShutdown->setAutoDefault(false);
    connect(pbShutdown, &QPushButton::pressed, this, &Shutdown::pbPressed);
    pbShutdown->installEventFilter(this);
    pbShutdown->setMouseTracking(true);
}

// eventFilter — delegates mouse drag events to moveWidget() so the button
// can be repositioned interactively on the control panel background.
bool Shutdown::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, pbShutdown, pbShutdown, event)) return true;
    return QObject::eventFilter(obj, event);
}

// SetState — updates the button label to reflect whether the system is
// currently shut down (true) or running (false).
void Shutdown::SetState(bool shutDown)
{
    if(shutDown) pbShutdown->setText("System enable");
    else         pbShutdown->setText("System shutdown");
}

// pbPressed — toggles the button label and emits the appropriate signal
// when the user presses the shutdown/enable button.
void Shutdown::pbPressed(void)
{
    if(pbShutdown->text() == "System shutdown")
    {
        pbShutdown->setText("System enable");
        emit ShutdownSystem();
    }
    else
    {
        pbShutdown->setText("System shutdown");
        emit EnableSystem();
    }
}
