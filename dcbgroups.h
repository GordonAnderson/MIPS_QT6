// =============================================================================
// dcbgroups.h
//
// DCBiasGroups — DC bias channel group definition widget for the MIPS custom
// control panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef DCBGROUPS_H
#define DCBGROUPS_H

#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>

// -----------------------------------------------------------------------------
// DCBiasGroups
//
// Displays a group box containing a combo box for defining DC bias channel
// groups and a checkbox to enable/disable group tracking. Emits disable() or
// enable() when the checkbox state changes.
// The widget can be dragged to a new position on the control panel background.
// -----------------------------------------------------------------------------
class DCBiasGroups : public QWidget
{
    Q_OBJECT
signals:
    void disable(void);
    void enable(void);
public:
    DCBiasGroups(QWidget *parent, int x, int y);
    void Show(void);
    bool SetValues(QString strVals);
    QString Report(void);

    QWidget   *p;
    int        X, Y;
    QComboBox *comboGroups;
    QGroupBox *gbDCBgroups;

private:
    QCheckBox *DCBenaGroups;

private slots:
    void slotEnableChange(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // DCBGROUPS_H
