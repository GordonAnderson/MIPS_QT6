// =============================================================================
// cpbutton.h
//
// CPbutton — control panel selector button for the MIPS custom control panel.
// Pressing the button loads a named control panel configuration file.
// Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef CPBUTTON_H
#define CPBUTTON_H

#include <QWidget>
#include <QPushButton>

// -----------------------------------------------------------------------------
// CPbutton
//
// Displays a push button that, when pressed, emits CPselected(FileName) so the
// host can load the named control panel configuration. The button can be
// dragged to a new position on the control panel background.
// ProcessCommand() allows the button to be activated via a text command string.
// -----------------------------------------------------------------------------
class CPbutton : public QWidget
{
    Q_OBJECT
signals:
    void CPselected(QString);
public:
    CPbutton(QWidget *parent, QString name, QString CPfilename, int x, int y);
    void Show(void);
    QString ProcessCommand(QString cmd);

    QWidget     *p;
    QString      Title;
    QString      FileName;
    int          X, Y;
    QPushButton *pbCPselect;

private slots:
    void pbCPselectPressed(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // CPBUTTON_H
