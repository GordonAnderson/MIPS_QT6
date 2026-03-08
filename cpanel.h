// =============================================================================
// cpanel.h
//
// Cpanel — embedded sub-panel widget for the MIPS custom control panel.
// Creates a ControlPanel instance and places a button on the parent panel
// that reveals it. Extracted from controlpanel.cpp/.h during Phase 3
// refactoring.
//
// Circular dependency note: this header forward-declares ControlPanel.
// cpanel.cpp includes controlpanel.h for the full definition.
//
// Depends on:  controlpanel.h (in .cpp only), comms.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef CPANEL_H
#define CPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QList>

// Forward declarations to break the circular dependency with controlpanel.h.
class ControlPanel;
class Comms;
class Properties;

// -----------------------------------------------------------------------------
// Cpanel
//
// Displays a push button on the parent control panel. Pressing the button
// shows a child ControlPanel loaded from CPfileName. The child panel is hidden
// (not destroyed) when it emits DialogClosed(). The button can be dragged to
// a new position on the parent panel background.
// -----------------------------------------------------------------------------
class Cpanel : public QWidget
{
    Q_OBJECT
public:
    explicit Cpanel(QWidget *parent, QString name, QString CPfileName, int x, int y,
                    QList<Comms*> S, Properties *prop, ControlPanel *pcp);
    void    Show(void);
    void    Update(void);
    QString ProcessCommand(QString cmd);

    QString      Title;
    int          X, Y;
    QWidget     *p;
    ControlPanel *cp;
    QPushButton *pbButton;

private slots:
    void pbButtonPressed(void);
    void slotDialogClosed(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // CPANEL_H
