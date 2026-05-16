// =============================================================================
// dacchannel.cpp
//
// DACchannel — single DAC output channel widget for the MIPS custom control
// panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  dacchannel.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "dacchannel.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Constructor — stores parent widget, channel name, MIPS parameter name, and
// position. Sets default scaling (m=1, b=0), format, and units.
// -----------------------------------------------------------------------------
DACchannel::DACchannel(QWidget *parent, QString name, QString MIPSname, int x, int y)
    : QWidget(parent)
{
    p         = parent;
    Title     = name;
    MIPSnm    = MIPSname;
    X         = x;
    Y         = y;
    comms     = nullptr;
    Units     = "V";
    m         = 1;
    b         = 0;
    Format    = "%.3f";
    Updating  = false;
    UpdateOff = false;
}

// Show — creates the frame, value line-edit, and unit labels; installs event
// filters on all child widgets so the group can be dragged as a unit.
void DACchannel::Show(void)
{
    frmDAC = new QFrame(p);
    frmDAC->setGeometry(X, Y, 180, 21);

    Vdac = new QLineEdit(frmDAC);
    Vdac->setGeometry(70, 0, 70, 21);
    Vdac->setValidator(new QDoubleValidator);
    Vdac->setToolTip("DAC output CH" + QString::number(Channel) + ", " + MIPSnm);

    labels[0] = new QLabel(Title, frmDAC);
    labels[0]->setGeometry(0, 0, 59, 16);

    labels[1] = new QLabel(Units, frmDAC);
    labels[1]->setGeometry(150, 0, 31, 16);

    connect(Vdac, &QLineEdit::returnPressed,   this, [this]() {Vdac->setModified(true);});
    connect(Vdac, &QLineEdit::editingFinished, this, &DACchannel::VdacChange);

    frmDAC->installEventFilter(this);
    frmDAC->setMouseTracking(true);
    labels[0]->installEventFilter(this);
    labels[0]->setMouseTracking(true);
    Vdac->installEventFilter(this);
    Vdac->setMouseTracking(true);
}

// eventFilter — handles widget drag (moveWidget) and mouse-wheel value
// adjustment (adjustValue). Blocks adjustValue while an update is in progress.
bool DACchannel::eventFilter(QObject *obj, QEvent *event)
{
    if (moveWidget(obj, frmDAC, labels[0], event)) return true;
    if (Updating) return true;
    UpdateOff = true;
    if (adjustValue(obj, Vdac, event, 1))
    {
        UpdateOff = false;
        return true;
    }
    UpdateOff = false;
    return QObject::eventFilter(obj, event);
}

// Report — returns a comma-separated string of the panel name, channel title,
// and current displayed value, for use in save/restore operations.
QString DACchannel::Report(void)
{
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title + "," + Vdac->text();
    return res;
}

// SetValues — restores the channel value from a save-file line. Returns true
// if the line prefix matches this channel, false otherwise.
bool DACchannel::SetValues(QString strVals)
{
    QStringList resList;
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (!strVals.startsWith(res)) return false;
    resList = strVals.split(",");
    if (resList.count() < 2) return false;
    Vdac->setText(resList[1]);
    Vdac->setModified(true);
    emit Vdac->editingFinished();
    return true;
}

// ProcessCommand — handles text commands of the form "title" (read) or
// "title=val" (write). Returns "?" if the command does not match this channel.
// The following commands are processed:
//   title        — returns the current displayed value
//   title=val    — sets the displayed value and writes to hardware
QString DACchannel::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (!cmd.startsWith(res)) return "?";
    if (cmd == Title) return Vdac->text();
    QStringList resList = cmd.split("=");
    if (resList.count() == 2)
    {
        Vdac->setText(resList[1]);
        Vdac->setModified(true);
        emit Vdac->editingFinished();
        return "";
    }
    return "?";
}

// Update — polls the hardware for the current DAC voltage, applies linear
// scaling (display = m*raw + b), and updates the line-edit if it lacks focus.
// display = m * readValue + b
void DACchannel::Update(void)
{
    QString res;

    if (comms == nullptr) return;
    if (UpdateOff) return;
    Updating = true;
    comms->rb.clear();
    res = "GDACV,CH" + QString::number(Channel) + "\n";
    res = comms->SendMess(res);
    if (res == "")
    {
        Updating = false;
        return;
    }
    res = res.asprintf(qPrintable(Format), res.toFloat() * m + b);
    if (!Vdac->hasFocus()) Vdac->setText(res);
    Updating = false;
}

// VdacChange — called when the user presses Return in the value line-edit.
// Applies inverse scaling (writeValue = (display - b) / m) and sends the
// result to the hardware.
// writeValue = (display - b) / m
void DACchannel::VdacChange(void)
{
    QString val;

    if (comms == nullptr) return;
    if (!Vdac->isModified()) return;
    val = val.asprintf("%.3f", (Vdac->text().toFloat() - b) / m);
    QString res = "SDACV,CH" + QString::number(Channel) + "," + val + "\n";
    comms->SendCommand(res);
    Vdac->setModified(false);
}
