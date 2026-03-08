// =============================================================================
// saveload.cpp
//
// SaveLoad — paired Save/Load push buttons for the MIPS custom control panel.
// Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  saveload.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "saveload.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Constructor — stores parent widget, button titles, and position parameters.
// -----------------------------------------------------------------------------
SaveLoad::SaveLoad(QWidget *parent, QString nameSave, QString nameLoad, int x, int y)
    : QWidget(parent)
{
    p         = parent;
    TitleSave = nameSave;
    TitleLoad = nameLoad;
    X         = x;
    Y         = y;
}

// Show — creates the Save and Load push buttons, installs event filters so
// both buttons can be repositioned by dragging on the control panel background.
void SaveLoad::Show(void)
{
    pbSave = new QPushButton(TitleSave, p);
    pbSave->setGeometry(X, Y, 150, 32);
    pbSave->setAutoDefault(false);
    pbSave->setMouseTracking(true);
    pbSave->installEventFilter(this);
    connect(pbSave, &QPushButton::pressed, this, &SaveLoad::pbSavePressed);

    pbLoad = new QPushButton(TitleLoad, p);
    pbLoad->setGeometry(X, Y + 40, 150, 32);
    pbLoad->setAutoDefault(false);
    pbLoad->setMouseTracking(true);
    pbLoad->installEventFilter(this);
    connect(pbLoad, &QPushButton::pressed, this, &SaveLoad::pbLoadPressed);
}

// eventFilter — delegates mouse drag events to moveWidget() so each button
// can be repositioned interactively on the control panel background.
bool SaveLoad::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == pbSave) { if (moveWidget(obj, pbSave, pbSave, event)) return true; }
    if (obj == pbLoad) { if (moveWidget(obj, pbLoad, pbLoad, event)) return true; }
    return QObject::eventFilter(obj, event);
}

// pbSavePressed — clears the button pressed state and emits Save().
void SaveLoad::pbSavePressed(void)
{
    pbSave->setDown(false);
    emit Save();
}

// pbLoadPressed — clears the button pressed state and emits Load().
void SaveLoad::pbLoadPressed(void)
{
    pbLoad->setDown(false);
    emit Load();
}
