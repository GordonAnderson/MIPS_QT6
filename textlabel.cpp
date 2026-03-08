// =============================================================================
// textlabel.cpp
//
// TextLabel — draggable text-label widget for the MIPS custom control panel.
// Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  textlabel.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "textlabel.h"
#include "Utilities.h"

#include <QFont>

// -----------------------------------------------------------------------------
// Constructor — stores parent widget and display parameters.
// -----------------------------------------------------------------------------
TextLabel::TextLabel(QWidget *parent, QString name, int s, int x, int y)
    : QWidget(parent)
{
    p     = parent;
    Title = name;
    Size  = s;
    X     = x;
    Y     = y;
}

// Show — creates the QLabel, applies the requested font size, and installs
// the event filter so the label can be repositioned by dragging.
void TextLabel::Show(void)
{
    label = new QLabel(Title, p);
    label->setGeometry(X, Y, 1, 1);
    QFont font = label->font();
    font.setPointSize(Size);
    label->setFont(font);
    label->adjustSize();
    label->installEventFilter(this);
    label->setMouseTracking(true);
}

// eventFilter — delegates mouse drag events to moveWidget() so the label
// can be repositioned interactively on the control panel background.
bool TextLabel::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, label, label, event)) return true;
    return QObject::eventFilter(obj, event);
}
