// =============================================================================
// cpbutton.cpp
//
// CPbutton — control panel selector button for the MIPS custom control panel.
// Pressing the button loads a named control panel configuration file.
// Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  cpbutton.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "cpbutton.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Constructor — stores parent widget, button title, config filename, and
// position parameters.
// -----------------------------------------------------------------------------
CPbutton::CPbutton(QWidget *parent, QString name, QString CPfilename, int x, int y)
    : QWidget(parent)
{
    p        = parent;
    Title    = name;
    FileName = CPfilename;
    X        = x;
    Y        = y;
}

// Show — creates the push button and installs the event filter so the button
// can be repositioned by dragging on the control panel background.
void CPbutton::Show(void)
{
    pbCPselect = new QPushButton(Title, p);
    pbCPselect->setGeometry(X, Y, 150, 32);
    pbCPselect->setAutoDefault(false);
    pbCPselect->setMouseTracking(true);
    pbCPselect->installEventFilter(this);
    connect(pbCPselect, &QPushButton::pressed, this, &CPbutton::pbCPselectPressed);
}

// eventFilter — delegates mouse drag events to moveWidget() so the button
// can be repositioned interactively on the control panel background.
bool CPbutton::eventFilter(QObject *obj, QEvent *event)
{
    if (moveWidget(obj, pbCPselect, pbCPselect, event)) return true;
    return QObject::eventFilter(obj, event);
}

// ProcessCommand — activates the button if cmd matches the qualified button
// name (panel.title). Returns an empty string on match, "?" otherwise.
QString CPbutton::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (cmd == res)
    {
        pbCPselectPressed();
        return "";
    }
    return "?";
}

// pbCPselectPressed — sets the button pressed state and emits CPselected()
// with the associated configuration filename.
void CPbutton::pbCPselectPressed(void)
{
    pbCPselect->setDown(true);
    emit CPselected(FileName);
}
