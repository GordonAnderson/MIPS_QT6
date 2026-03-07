// =============================================================================
// filament.h
//
// Class declaration for Filament.
// See filament.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef FILAMENT_H
#define FILAMENT_H

#include "ui_mips.h"
#include "comms.h"

#include <QtCore/QtGlobal>
#include <QObject>
#include <QApplication>

// -----------------------------------------------------------------------------
// Filament — tab panel for up to two MIPS filament heater channels.
// Each channel has leS* setpoint editors (current, voltage, power limits) and
// an enable/disable checkbox. All setpoints are sent to MIPS via the generic
// object-name-derived command format.
// -----------------------------------------------------------------------------
class Filament : public QDialog
{
    Q_OBJECT

public:
    Filament(Ui::MIPS *w, Comms *c);
    void Update(void);

    Ui::MIPS *fui;
    Comms    *comms;

private slots:
    void Filament1Enable(void);
    void Filament2Enable(void);
    void FilamentUpdated(void);
};

#endif // FILAMENT_H
