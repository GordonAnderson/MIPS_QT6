// =============================================================================
// saveload.h
//
// SaveLoad — paired Save/Load push buttons for the MIPS custom control panel.
// Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef SAVELOAD_H
#define SAVELOAD_H

#include <QWidget>
#include <QPushButton>

// -----------------------------------------------------------------------------
// SaveLoad
//
// Displays a Save button and a Load button stacked vertically. The buttons
// can be dragged to new positions on the control panel background. Emits
// Save() or Load() when the corresponding button is pressed.
// -----------------------------------------------------------------------------
class SaveLoad : public QWidget
{
    Q_OBJECT
signals:
    void Save(void);
    void Load(void);
public:
    SaveLoad(QWidget *parent, QString nameSave, QString nameLoad, int x, int y);
    void Show(void);

    QWidget     *p;
    QString      TitleSave;
    QString      TitleLoad;
    int          X, Y;
    QPushButton *pbSave;
    QPushButton *pbLoad;

private slots:
    void pbSavePressed(void);
    void pbLoadPressed(void);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // SAVELOAD_H
