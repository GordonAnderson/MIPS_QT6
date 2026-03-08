// =============================================================================
// textlabel.h
//
// TextLabel — draggable text-label widget for the MIPS custom control panel.
// Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef TEXTLABEL_H
#define TEXTLABEL_H

#include <QWidget>
#include <QLabel>

// -----------------------------------------------------------------------------
// TextLabel
//
// Displays a static text label on a custom control panel background. The label
// can be dragged to a new position at run time via the standard moveWidget()
// helper (right-click drag).
// -----------------------------------------------------------------------------
class TextLabel : public QWidget
{
    Q_OBJECT
public:
    TextLabel(QWidget *parent, QString name, int size, int x, int y);
    void Show(void);

    QWidget *p;
    QString  Title;
    int      X, Y;
    int      Size;
    QLabel  *label;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // TEXTLABEL_H
