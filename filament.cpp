// =============================================================================
// filament.cpp
//
// Implements the Filament tab panel, which controls up to two filament heater
// channels on the MIPS system. Each channel has a set of leS* line-edit
// setpoints (current, voltage, power limits) wired to FilamentUpdated(), and
// an enable/disable checkbox.
//
// MIPS commands used:
//   GFLENA,ch / SFLENA,ch,ON|OFF   — filament channel enable
//   G<WIDGET>,<CH>                 — generic get for leS* widgets
//   S<WIDGET>,<CH>,<VAL>           — generic set built from widget names
//
// Depends on:  ui_mips.h, comms.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "filament.h"

// Filament — constructor. Installs QDoubleValidator and connects returnPressed
// on all leS* setpoints in both filament group boxes, and wires the enable
// checkboxes.
Filament::Filament(Ui::MIPS *w, Comms *c)
{
    fui   = w;
    comms = c;

    // Wire all leS* setpoint editors in both filament groups to FilamentUpdated()
    QObjectList widgetList = fui->gpFilament1->children();
    widgetList += fui->gpFilament2->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().startsWith("leS"))
        {
            ((QLineEdit *)w)->setValidator(new QDoubleValidator);
            connect(((QLineEdit *)w), &QLineEdit::returnPressed,   this, [le=(QLineEdit *)w]() {le->setModified(true);});
            connect(((QLineEdit *)w), &QLineEdit::editingFinished, this, &Filament::FilamentUpdated);
        }
    }
    connect(fui->chkEnableFilament1, SIGNAL(toggled(bool)), this, SLOT(Filament1Enable()));
    connect(fui->chkEnableFilament2, SIGNAL(toggled(bool)), this, SLOT(Filament2Enable()));
}

// Filament1Enable — slot for chkEnableFilament1 toggled. Sends SFLENA,1,ON
// or SFLENA,1,OFF to MIPS.
void Filament::Filament1Enable(void)
{
    if(fui->chkEnableFilament1->isChecked()) comms->SendCommand("SFLENA,1,ON\n");
    else                                     comms->SendCommand("SFLENA,1,OFF\n");
}

// Filament2Enable — slot for chkEnableFilament2 toggled. Sends SFLENA,2,ON
// or SFLENA,2,OFF to MIPS.
void Filament::Filament2Enable(void)
{
    if(fui->chkEnableFilament2->isChecked()) comms->SendCommand("SFLENA,2,ON\n");
    else                                     comms->SendCommand("SFLENA,2,OFF\n");
}

// FilamentUpdated — slot called when any leS* setpoint editor finishes editing.
// Builds the MIPS set command from the widget's object name (e.g. leSSPV_1
// becomes SSPV,1,<value>\n) and sends it.
void Filament::FilamentUpdated(void)
{
    QObject *obj = sender();
    QString  res;

    if(!((QLineEdit *)obj)->isModified()) return;
    res = obj->objectName().mid(2).replace("_", ",") + "," + ((QLineEdit *)obj)->text() + "\n";
    comms->SendCommand(res.toStdString().c_str());
    ((QLineEdit *)obj)->setModified(false);
}

// Update — polls all filament channel values and enable states from MIPS and
// refreshes the UI. Skips any line edit that currently has keyboard focus to
// avoid overwriting values the user is actively editing.
void Filament::Update(void)
{
    QString res;

    res = comms->SendMess("GFLENA,1\n");
    if(res == "ON")       fui->chkEnableFilament1->setChecked(true);
    else if(res == "OFF") fui->chkEnableFilament1->setChecked(false);

    QObjectList widgetList = fui->gpFilament1->children();
    widgetList += fui->gpFilament2->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().startsWith("le"))
        {
            if(!((QLineEdit *)w)->hasFocus())
            {
                res = "G" + w->objectName().mid(3).replace("_", ",") + "\n";
                ((QLineEdit *)w)->setText(comms->SendMess(res));
            }
        }
    }

    res = comms->SendMess("GFLENA,2\n");
    if(res == "ON")       fui->chkEnableFilament2->setChecked(true);
    else if(res == "OFF") fui->chkEnableFilament2->setChecked(false);
}
