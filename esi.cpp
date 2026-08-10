// =============================================================================
// esi.cpp
//
// ESI — electrospray ionisation high-voltage channel widget for the MIPS
// custom control panel. Extracted from controlpanel.cpp/.h during Phase 3
// refactoring.
//
// Depends on:  esi.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "esi.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Constructor — stores parent widget, channel name, MIPS parameter name, and
// position. Initialises comms to nullptr and isShutdown to false.
// -----------------------------------------------------------------------------
ESI::ESI(QWidget *parent, QString name, QString MIPSname, int x, int y)
    : QWidget(parent)
{
    p          = parent;
    Title      = name;
    MIPSnm     = MIPSname;
    X          = x;
    Y          = y;
    comms      = nullptr;
    isShutdown = false;
}

// Show — creates the frame, setpoint/readback line-edits, enable checkbox, and
// labels; installs event filters so the group can be dragged as a unit.
void ESI::Show(void)
{
    frmESI = new QFrame(p);
    frmESI->setGeometry(X, Y, 241, 42);

    ESIsp = new QLineEdit(frmESI);
    ESIsp->setGeometry(70, 0, 70, 21);
    ESIsp->setValidator(new QDoubleValidator);
    ESIsp->setToolTip(MIPSnm + " ESI channel " + QString::number(Channel));

    ESIrb = new QLineEdit(frmESI);
    ESIrb->setGeometry(140, 0, 70, 21);
    ESIrb->setReadOnly(true);

    ESIena = new QCheckBox(frmESI);
    ESIena->setGeometry(70, 22, 70, 21);
    ESIena->setText("Enable");

    labels[0] = new QLabel(Title, frmESI);
    labels[0]->setGeometry(0, 0, 59, 16);
    labels[1] = new QLabel("V", frmESI);
    labels[1]->setGeometry(220, 0, 21, 16);

    connect(ESIsp,  &QLineEdit::returnPressed,          this, [this]() {ESIsp->setModified(true);});
    connect(ESIsp,  &QLineEdit::editingFinished,        this, &ESI::ESIChange);
    connect(ESIena, &QCheckBox::checkStateChanged,      this, &ESI::ESIenaChange);

    frmESI->installEventFilter(this);
    frmESI->setMouseTracking(true);
    labels[0]->installEventFilter(this);
    labels[0]->setMouseTracking(true);
}

// eventFilter — delegates mouse drag events to moveWidget() so the widget
// can be repositioned interactively on the control panel background.
bool ESI::eventFilter(QObject *obj, QEvent *event)
{
    if (moveWidget(obj, frmESI, frmESI, event)) return true;
    return QObject::eventFilter(obj, event);
}

// Report — returns a comma-separated string of panel name, title, enable state,
// setpoint, and readback value, for use in save/restore operations.
QString ESI::Report(void)
{
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title + ",";
    if (isShutdown)
    {
        if (activeEnableState) res += "ON,";
        else                   res += "OFF,";
    }
    else
    {
        if (ESIena->isChecked()) res += "ON,";
        else                     res += "OFF,";
    }
    res += ESIsp->text() + "," + ESIrb->text();
    return res;
}

// SetValues — restores the channel enable state and setpoint from a save-file
// line. Returns true if the prefix matches this channel, false otherwise.
bool ESI::SetValues(QString strVals)
{
    QStringList resList;
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (!strVals.startsWith(res)) return false;
    resList = strVals.split(",");
    if (resList.count() < 3) return false;
    if (isShutdown)
    {
        activeEnableState = resList[1].contains("ON");
    }
    else
    {
        ESIena->setChecked(resList[1].contains("ON"));
        if (resList[1].contains("ON")) emit ESIena->checkStateChanged(Qt::Checked);
        else                           emit ESIena->checkStateChanged(Qt::Unchecked);
    }
    ESIsp->setText(resList[2]);
    ESIsp->setModified(true);
    emit ESIsp->editingFinished();
    return true;
}

// ProcessCommand — handles text commands for reading or writing the setpoint
// and enable state. Returns "?" if the command does not match this channel.
// Supported commands:
//   title           — returns the setpoint
//   title=val       — sets the setpoint
//   title.readback  — returns the readback value
//   title.ena       — returns "ON" or "OFF"
//   title.ena=ON/OFF — sets the enable state
QString ESI::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (!cmd.startsWith(res)) return "?";
    if (cmd == res)               return ESIsp->text();
    if (cmd == res + ".readback") return ESIrb->text();
    if (cmd == res + ".ena")
    {
        if (ESIena->isChecked()) return "ON";
        return "OFF";
    }
    QStringList resList = cmd.split("=");
    if (resList.count() == 2)
    {
        if (resList[0] == Title)
        {
            ESIsp->setText(resList[1]);
            ESIsp->setModified(true);
            emit ESIsp->editingFinished();
            return "";
        }
        if (resList[0] == res + ".ena")
        {
            if      (resList[1] == "ON")  ESIena->setChecked(true);
            else if (resList[1] == "OFF") ESIena->setChecked(false);
            else return "?";
            if (resList[1] == "ON") emit ESIena->checkStateChanged(Qt::Checked);
            else                    emit ESIena->checkStateChanged(Qt::Unchecked);
            return "";
        }
    }
    return "?";
}

// Update — polls the hardware for setpoint, readback voltage, and enable state;
// updates the UI without overwriting a field the user is actively editing.
void ESI::Update(void)
{
    QString res;

    if (comms == nullptr) return;

    comms->rb.clear();
    res = "GHV," + QString::number(Channel) + "\n";
    res = comms->SendMess(res);
    if (res == "") return;
    if (!ESIsp->hasFocus()) ESIsp->setText(res);

    res = "GHVV," + QString::number(Channel) + "\n";
    res = comms->SendMess(res);
    if (res == "") return;
    ESIrb->setText(res);

    res = comms->SendMess("GHVSTATUS," + QString::number(Channel) + "\n");
    bool oldState = ESIena->blockSignals(true);
    if (res.contains("ON"))  ESIena->setChecked(true);
    if (res.contains("OFF")) ESIena->setChecked(false);
    ESIena->blockSignals(oldState);
}

// ESIChange — called when the user presses Return in the setpoint line-edit;
// sends the new setpoint to the hardware.
void ESI::ESIChange(void)
{
    if (comms == nullptr) return;
    if (!ESIsp->isModified()) return;
    QString res = "SHV," + QString::number(Channel) + "," + ESIsp->text() + "\n";
    comms->SendCommand(res);
    ESIsp->setModified(false);
}

// ESIenaChange — sends an enable or disable command to the hardware when the
// Enable checkbox state changes.
void ESI::ESIenaChange(void)
{
    QString res;

    if (comms == nullptr) return;
    if (ESIena->checkState()) res = "SHVENA," + QString::number(Channel) + "\n";
    else                      res = "SHVDIS,"  + QString::number(Channel) + "\n";
    comms->SendCommand(res);
}

// Shutdown — saves the current enable state and disables the ESI output.
void ESI::Shutdown(void)
{
    if (isShutdown) return;
    isShutdown        = true;
    activeEnableState = ESIena->checkState();
    ESIena->setChecked(false);
    emit ESIena->checkStateChanged(Qt::Unchecked);
}

// Restore — re-applies the enable state that was saved by Shutdown().
void ESI::Restore(void)
{
    if (!isShutdown) return;
    isShutdown = false;
    ESIena->setChecked(activeEnableState);
    emit ESIena->checkStateChanged(Qt::Unchecked);
}
