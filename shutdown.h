// =============================================================================
// shutdown.h
//
// Shutdown — system shutdown/enable toggle button for the MIPS custom control
// panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#include <QWidget>
#include <QPushButton>

// -----------------------------------------------------------------------------
// Shutdown
//
// Displays a single push button that toggles between "System shutdown" and
// "System enable". Emits ShutdownSystem() or EnableSystem() accordingly.
// The button can be dragged to a new position on the control panel background.
// -----------------------------------------------------------------------------
class Shutdown : public QWidget
{
    Q_OBJECT
signals:
    void ShutdownSystem(void);
    void EnableSystem(void);
public:
    Shutdown(QWidget *parent, QString name, int x, int y);
    void Show(void);
    void SetState(bool shutDown);

    QWidget     *p;
    QString      Title;
    int          X, Y;
    QPushButton *pbShutdown;

private slots:
    void pbPressed(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // SHUTDOWN_H
