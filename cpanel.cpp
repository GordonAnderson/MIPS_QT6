// =============================================================================
// cpanel.cpp
//
// Cpanel — embedded sub-panel widget for the MIPS custom control panel.
// Creates a ControlPanel instance and places a button on the parent panel
// that reveals it. Extracted from controlpanel.cpp/.h during Phase 3
// refactoring.
//
// controlpanel.h is included here (not in cpanel.h) to break the circular
// dependency: ControlPanel → cpanel.h would re-include controlpanel.h.
//
// Depends on:  cpanel.h, controlpanel.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "cpanel.h"
#include "controlpanel.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Constructor — creates the child ControlPanel from CPfileName and connects its
// DialogClosed() signal so the panel is hidden rather than destroyed on close.
// -----------------------------------------------------------------------------
Cpanel::Cpanel(QWidget *parent, QString name, QString CPfileName, int x, int y,
               QList<Comms*> S, Properties *prop, ControlPanel *pcp)
    : QWidget(parent)
{
    cp    = new ControlPanel(0, CPfileName, S, prop, pcp);
    p     = parent;
    X     = x;
    Y     = y;
    Title = name;
    connect(cp, &ControlPanel::DialogClosed, this, &Cpanel::slotDialogClosed);
}

// Show — creates the push button and installs the event filter so the button
// can be repositioned by dragging on the control panel background.
void Cpanel::Show(void)
{
    pbButton = new QPushButton(Title, p);
    pbButton->setGeometry(X, Y, 150, 32);
    pbButton->setAutoDefault(false);
    pbButton->installEventFilter(this);
    pbButton->setMouseTracking(true);
    connect(pbButton, &QPushButton::pressed, this, &Cpanel::pbButtonPressed);
}

// Update — forwards Update() to the child panel if it is currently visible.
void Cpanel::Update(void)
{
    if (cp->isVisible()) cp->Update();
}

// eventFilter — delegates mouse drag events to moveWidget() so the button
// can be repositioned interactively on the control panel background.
bool Cpanel::eventFilter(QObject *obj, QEvent *event)
{
    if (moveWidget(obj, pbButton, pbButton, event)) return true;
    return QObject::eventFilter(obj, event);
}

// ProcessCommand — activates the panel button if cmd matches the qualified
// title, or forwards sub-commands into the child panel's Command() handler.
// Returns "" on success, "?" if the command is not recognised.
QString Cpanel::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (cmd == res)
    {
        pbButtonPressed();
        return "";
    }
    res = Title + ".";
    if (!cmd.startsWith(res)) return "?";
    return cp->Command(cmd.mid(res.length()));
}

// pbButtonPressed — clears the button pressed state and shows the child panel.
void Cpanel::pbButtonPressed(void)
{
    pbButton->setDown(false);
    cp->show();
    cp->raise();
}

// slotDialogClosed — hides the child panel when it signals that it has closed.
void Cpanel::slotDialogClosed(void)
{
    cp->hide();
}
