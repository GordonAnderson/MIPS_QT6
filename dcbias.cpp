// =============================================================================
// dcbias.cpp
//
// Implements four classes that together cover all DC bias functionality in the
// MIPS host application:
//
//   DCbias      — tab-based control for the built-in 8/16/24-channel DC bias
//                 board. Reads all channel voltages each update cycle, supports
//                 grouped voltage adjustment (ApplyDelta), Save/Load of
//                 settings, and a power enable/disable toggle.
//
//   DCBchannel  — draggable single-channel widget showing setpoint (Vsp) and
//                 readback (Vrb). Readback background turns green when within
//                 1% or 2 V of setpoint, red when out of tolerance. Supports
//                 linked-channel grouping so changing one channel shifts all
//                 linked channels by the same delta.
//
//   DCBoffset   — draggable single-channel offset/range widget. Sends
//                 SDCBOF/GDCBOF commands to adjust the per-board voltage range.
//
//   DCBenable   — draggable checkbox widget that sends SDCPWR ON/OFF.
//                 Supports Shutdown/Restore for safe power sequencing.
//
// MIPS commands used (partial):
//   GDCPWR / SDCPWR,ON|OFF   — DC bias board power
//   GDCB,ch / SDCB,ch,val    — channel setpoint read/write
//   GDCBV,ch                 — channel readback voltage
//   GDCBOF,ch / SDCBOF,ch,v  — channel offset/range
//   G<WIDGET>,<CHANNEL>      — generic get used in DCbias::Update()
//
// Depends on:  ui_mips.h, comms.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "dcbias.h"
#include "Utilities.h"

namespace Ui {
class MIPS;
}

// Readback colour thresholds
static const float DCB_ABS_ERROR_THRESHOLD  = 2.0f;   // volts — absolute error limit
static const float DCB_REL_ERROR_THRESHOLD  = 0.01f;  // 1 % of setpoint

// =============================================================================
// DCbias — tab panel for the built-in DC bias board
// =============================================================================

// DCbias — constructor. Stores the UI handle and Comms pointer, initialises
// state flags, sets up QDoubleValidator and event filters on every leSDCB_*
// line edit across all three bias group boxes, and connects the Update button
// and power-enable checkbox.
DCbias::DCbias(Ui::MIPS *w, Comms *c)
{
    dui   = w;
    comms = c;

    Updating  = false;
    UpdateOff = false;
    SetNumberOfChannels(8);

    // Wire up all leSDCB_* line edits across all three bias groups
    selectedLineEdit = NULL;
    QObjectList widgetList = dui->gbDCbias1->children();
    widgetList += dui->gbDCbias2->children();
    widgetList += dui->gbDCbias3->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("leSDCB"))
        {
            ((QLineEdit *)w)->setValidator(new QDoubleValidator);
            ((QLineEdit *)w)->installEventFilter(this);
            ((QLineEdit *)w)->setMouseTracking(true);
            connect(((QLineEdit *)w), SIGNAL(editingFinished()), this, SLOT(DCbiasUpdated()));
        }
    }
    connect(dui->pbDCbiasUpdate,  SIGNAL(pressed()),       this, SLOT(UpdateDCbias()));
    connect(dui->chkPowerEnable,  SIGNAL(toggled(bool)),   this, SLOT(DCbiasPower()));
}

// eventFilter — suppresses normal processing while the update cycle is active.
// Sets UpdateOff during mouse-wheel value adjustment via adjustValue() to
// prevent background polling from overwriting the in-progress edit.
bool DCbias::eventFilter(QObject *obj, QEvent *event)
{
    if(Updating) return true;
    UpdateOff = true;
    if(adjustValue(obj, (QLineEdit *)obj, event, 1))
    {
        UpdateOff = false;
        return true;
    }
    UpdateOff = false;
    return QObject::eventFilter(obj, event);
}

// SetNumberOfChannels — enables the appropriate group boxes for the fitted
// board size: 8 channels = group 1 only; 16 = groups 1+2; 24 = all three.
void DCbias::SetNumberOfChannels(int num)
{
    NumChannels = num;
    dui->gbDCbias1->setEnabled(false);
    dui->gbDCbias2->setEnabled(false);
    dui->gbDCbias3->setEnabled(false);
    if(NumChannels >= 8)  dui->gbDCbias1->setEnabled(true);
    if(NumChannels > 8)   dui->gbDCbias2->setEnabled(true);
    if(NumChannels > 16)  dui->gbDCbias3->setEnabled(true);
}

// ApplyDelta — applies a voltage delta to every channel whose group label
// matches GrpName. Skips the channel that currently has focus (that is the
// one the user is editing and whose new value has already been applied).
void DCbias::ApplyDelta(QString GrpName, float change)
{
    QString res;

    for(int i = 1; i <= 24; i++)
    {
        res = "leGRP" + QString::number(i);
        QLineEdit *leGR = dui->gbDCbias1->findChild<QLineEdit *>(res);
        if(leGR != NULL && leGR->text() == GrpName)
        {
            res = "leSDCB_" + QString::number(i);
            QLineEdit *leDCB = dui->gbDCbias1->findChild<QLineEdit *>(res);
            if(leDCB != NULL && !leDCB->hasFocus())
            {
                leDCB->setText(QString::number(leDCB->text().toFloat() - change));
                leDCB->setModified(true);
                emit leDCB->editingFinished();
            }
        }
    }
}

// DCbiasUpdated — slot called when any leSDCB_* line edit finishes editing.
// Sends the updated voltage to MIPS. If the channel belongs to a group,
// reads the current MIPS value first to calculate the delta, then calls
// ApplyDelta to shift all group members by the same amount.
void DCbias::DCbiasUpdated(void)
{
    QObject     *obj = sender();
    QString      res, ans;
    QStringList  resList;

    if(Updating) return;
    if(!((QLineEdit *)obj)->isModified()) return;

    if((obj->objectName().startsWith("leSDCB_")) && (((QLineEdit *)obj)->hasFocus()))
    {
        resList = obj->objectName().split("_");
        res = "leGRP" + resList[1];
        QLineEdit *leGR = NULL;
        if(resList[1].toInt() <= 8)  leGR = dui->gbDCbias1->findChild<QLineEdit *>(res);
        else if(resList[1].toInt() <= 16) leGR = dui->gbDCbias2->findChild<QLineEdit *>(res);
        else if(resList[1].toInt() <= 24) leGR = dui->gbDCbias3->findChild<QLineEdit *>(res);
        if(leGR != NULL && leGR->text() != "")
        {
            // Channel is in a group — read current MIPS value to compute delta
            res = "G" + obj->objectName().mid(3).replace("_", ",") + "\n";
            ans = comms->SendMess(res);
            if(ans != "")
            {
                float oldvalue = ans.toFloat();
                float change   = oldvalue - ((QLineEdit *)obj)->text().toFloat();
                ApplyDelta(leGR->text(), change);
            }
        }
    }
    res = obj->objectName().mid(2).replace("_", ",") + "," + ((QLineEdit *)obj)->text() + "\n";
    comms->SendCommand(res.toStdString().c_str());
    ((QLineEdit *)obj)->setModified(false);
    UpdateOff = false;
}

// Update — reads all DC bias channel voltages from MIPS and refreshes the UI.
// Bails out early if the DCbias tab is not currently visible, or if UpdateOff
// is set (user is in the middle of editing a value). Also refreshes the power
// state and adjusts the displayed voltage range by the offset value.
void DCbias::Update(void)
{
    QString res;

    if(UpdateOff) return;
    Updating = true;

    dui->leGCHAN_DCB->setText(QString::number(NumChannels));
    res = comms->SendMess("GDCPWR\n");
    if(res == "ON")  dui->chkPowerEnable->setChecked(true);
    if(res == "OFF") dui->chkPowerEnable->setChecked(false);

    // --- Group 1 (channels 1–8) ---
    QObjectList widgetList = dui->gbDCbias1->children();
    foreach(QObject *w, widgetList)
    {
        if(dui->tabMIPS->tabText(dui->tabMIPS->currentIndex()) != "DCbias") { Updating = false; return; }
        if((w->objectName().contains("le")) && (!w->objectName().contains("leGRP")))
        {
            if(!((QLineEdit *)w)->hasFocus())
            {
                res = "G" + w->objectName().mid(3).replace("_", ",") + "\n";
                res = comms->SendMess(res);
                if(res != "") ((QLineEdit *)w)->setText(res);
            }
        }
    }
    // Adjust displayed range to include offset
    dui->leGDCMIN_1->setText(QString::number(dui->leGDCMIN_1->text().toFloat() + dui->leSDCBOF_1->text().toFloat()));
    dui->leGDCMAX_1->setText(QString::number(dui->leGDCMAX_1->text().toFloat() + dui->leSDCBOF_1->text().toFloat()));

    // --- Group 2 (channels 9–16) ---
    if(NumChannels > 8)
    {
        QObjectList widgetList = dui->gbDCbias2->children();
        foreach(QObject *w, widgetList)
        {
            if(dui->tabMIPS->tabText(dui->tabMIPS->currentIndex()) != "DCbias") { Updating = false; return; }
            if((w->objectName().contains("le")) && (!w->objectName().contains("leGRP")))
            {
                if(!((QLineEdit *)w)->hasFocus())
                {
                    res = "G" + w->objectName().mid(3).replace("_", ",") + "\n";
                    res = comms->SendMess(res);
                    if(res != "") ((QLineEdit *)w)->setText(res);
                }
            }
        }
        dui->leGDCMIN_9->setText(QString::number(dui->leGDCMIN_9->text().toFloat()  + dui->leSDCBOF_9->text().toFloat()));
        dui->leGDCMAX_9->setText(QString::number(dui->leGDCMAX_9->text().toFloat()  + dui->leSDCBOF_9->text().toFloat()));
    }

    // --- Group 3 (channels 17–24) ---
    if(NumChannels > 16)
    {
        QObjectList widgetList = dui->gbDCbias3->children();
        foreach(QObject *w, widgetList)
        {
            if(dui->tabMIPS->tabText(dui->tabMIPS->currentIndex()) != "DCbias") { Updating = false; return; }
            if(w->objectName().contains("le"))
            {
                if(!((QLineEdit *)w)->hasFocus())
                {
                    res = "G" + w->objectName().mid(3).replace("_", ",") + "\n";
                    res = comms->SendMess(res);
                    if(res != "") ((QLineEdit *)w)->setText(res);
                }
            }
        }
        dui->leGDCMIN_17->setText(QString::number(dui->leGDCMIN_17->text().toFloat() + dui->leSDCBOF_17->text().toFloat()));
        dui->leGDCMAX_17->setText(QString::number(dui->leGDCMAX_17->text().toFloat() + dui->leSDCBOF_17->text().toFloat()));
    }

    Updating = false;
}

// DCbiasPower — slot for chkPowerEnable toggled. Sends SDCPWR,ON or
// SDCPWR,OFF to the connected MIPS unit.
void DCbias::DCbiasPower(void)
{
    if(dui->chkPowerEnable->isChecked()) comms->SendCommand("SDCPWR,ON\n");
    else                                 comms->SendCommand("SDCPWR,OFF\n");
}

// UpdateDCbias — slot for the manual "Update" push-button. Delegates to
// Update() to re-read all channel values from MIPS.
void DCbias::UpdateDCbias(void)
{
    Update();
}

// Save — writes all leS* widget values and the power enable state to a
// timestamped CSV settings file.
void DCbias::Save(QString Filename)
{
    QString res;

    if(NumChannels == 0) return;
    if(Filename == "") return;
    QFile file(Filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QDateTime dateTime = QDateTime::currentDateTime();
        stream << "# DCbias settings, " + dateTime.toString() + "\n";
        QObjectList widgetList = dui->DCbias->children();
        widgetList += dui->gbDCbias1->children();
        if(NumChannels > 8) widgetList += dui->gbDCbias2->children();
        foreach(QObject *w, widgetList)
        {
            if(w->objectName() == "chkPowerEnable")
            {
                stream << "chkPowerEnable,";
                if(dui->chkPowerEnable->isChecked()) stream << "ON\n";
                else stream << "OFF\n";
            }
            if(w->objectName().mid(0,3) == "leS")
            {
                res = w->objectName() + "," + ((QLineEdit *)w)->text() + "\n";
                stream << res;
            }
        }
        file.close();
        dui->statusBar->showMessage("Settings saved to " + Filename, 2000);
    }
}

// Load — reads a settings file and applies each leS* value by invoking
// editingFinished, causing the value to be sent to MIPS immediately.
void DCbias::Load(QString Filename)
{
    QStringList resList;

    if(NumChannels == 0) return;
    if(Filename == "") return;
    QObjectList widgetList = dui->DCbias->children();
    widgetList += dui->gbDCbias1->children();
    if(NumChannels > 8) widgetList += dui->gbDCbias2->children();
    QFile file(Filename);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            resList = line.split(",");
            if(resList.count() == 2)
            {
                foreach(QObject *w, widgetList)
                {
                    if(w->objectName().mid(0,3) == "leS")
                    {
                        if(resList[1] != "" && w->objectName() == resList[0])
                        {
                            ((QLineEdit *)w)->setText(resList[1]);
                            ((QLineEdit *)w)->setModified(true);
                            QMetaObject::invokeMethod(w, "editingFinished");
                        }
                    }
                    if(w->objectName() == "chkPowerEnable" && w->objectName() == resList[0])
                    {
                        ((QCheckBox *)w)->setChecked(resList[1] == "ON");
                    }
                }
            }
        } while(!line.isNull());
        file.close();
        dui->statusBar->showMessage("Settings loaded from " + Filename, 2000);
    }
}

// =============================================================================
// DCBchannel — draggable single-channel DC bias setpoint + readback widget
// =============================================================================

// DCBchannel — constructor. Records position and identity information;
// call Show() afterwards to create the visible setpoint/readback widgets.
DCBchannel::DCBchannel(QWidget *parent, QString name, QString MIPSname, int x, int y)
    : QWidget(parent)
{
    p          = parent;
    Title      = name;
    MIPSnm     = MIPSname;
    X          = x;
    Y          = y;
    comms      = NULL;
    isShutdown = false;
    Updating   = false;
    UpdateOff  = false;
    VspEdited  = true;
    DCBs.clear();
    LinkEnable = false;
    CurrentVsp = 0;
}

// Show — creates the frame with setpoint (Vsp) and readback (Vrb) line edits
// and installs drag support on the frame and title label.
void DCBchannel::Show(void)
{
    frmDCB    = new QFrame(p);      frmDCB->setGeometry(X, Y, 241, 21);
    Vsp       = new QLineEdit(frmDCB); Vsp->setGeometry(70,  0, 70, 21); Vsp->setValidator(new QDoubleValidator);
    Vrb       = new QLineEdit(frmDCB); Vrb->setGeometry(140, 0, 70, 21); Vrb->setReadOnly(true);
    labels[0] = new QLabel(Title,  frmDCB); labels[0]->setGeometry(0,   0, 59, 16);
    labels[1] = new QLabel("V",    frmDCB); labels[1]->setGeometry(220, 0, 21, 16);
    connect(Vsp, SIGNAL(editingFinished()), this, SLOT(VspChange()));
    Vsp->setToolTip(MIPSnm + " channel " + QString::number(Channel));
    frmDCB->installEventFilter(this);
    frmDCB->setMouseTracking(true);
    labels[0]->installEventFilter(this);
    labels[0]->setMouseTracking(true);
    Vsp->installEventFilter(this);
    Vsp->setMouseTracking(true);
}

// eventFilter — handles drag-to-move via moveWidget() and mouse-wheel value
// adjustment on Vsp via adjustValue(). Sets UpdateOff while the wheel event
// is in progress so background polling does not overwrite the edit.
bool DCBchannel::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, frmDCB, labels[0], event)) return true;
    if(Updating) return true;
    UpdateOff = true;
    if(adjustValue(obj, Vsp, event, 1))
    {
        UpdateOff = false;
        return true;
    }
    UpdateOff = false;
    return QObject::eventFilter(obj, event);
}

// Report — returns a "title,Vsp,Vrb" CSV string representing the channel's
// current state. While shut down, the saved active voltage replaces Vsp.
QString DCBchannel::Report(void)
{
    QString res, title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(isShutdown) res = title + "," + activeVoltage + "," + Vrb->text();
    else           res = title + "," + Vsp->text()   + "," + Vrb->text();
    return(res);
}

// SetValues — parses a "title,vsp" CSV string and applies the setpoint.
// While shut down, stores the value for later Restore rather than sending it.
// Returns false if the string does not match this channel's title.
bool DCBchannel::SetValues(QString strVals)
{
    QStringList resList;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!strVals.startsWith(title)) return false;
    resList = strVals.split(",");
    if(resList[0] != title) return false;
    if(resList.count() < 2) return false;
    if(isShutdown)
    {
        activeVoltage = resList[1];
        CurrentVsp    = activeVoltage.toFloat();
    }
    else
    {
        Vsp->setText(resList[1]);
        CurrentVsp = Vsp->text().toFloat();
        Vsp->setModified(true);
        emit Vsp->editingFinished();
    }
    return true;
}

// ProcessCommand — scripting API for this channel.
// Supported commands:
//   title            — returns the setpoint voltage
//   title=val        — sets the setpoint voltage
//   title.readback   — returns the readback voltage
//   title.channel    — returns the MIPS channel number
//   title.mips       — returns the MIPS module name
//   title.color=val  — sets the frame background colour (CSS colour string)
//   title.hide=TRUE|FALSE — shows or hides the widget frame
// Returns "?" for unrecognised commands.
QString DCBchannel::ProcessCommand(QString cmd)
{
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";
    if(cmd == title)               return Vsp->text();
    if(cmd == title + ".readback") return Vrb->text();
    if(cmd == title + ".channel")  return QString::number(Channel);
    if(cmd == title + ".mips")     return MIPSnm;

    QStringList resList = cmd.split("=");
    if(resList.count() == 2)
    {
        if(resList[0].trimmed() == title.trimmed())
        {
            Vsp->setText(resList[1].trimmed());
            Vsp->setModified(true);
            emit Vsp->editingFinished();
            return "";
        }
        if(resList[0].trimmed() == title.trimmed() + ".color")
        {
            frmDCB->setStyleSheet("background-color: " + resList[1].trimmed());
            return "";
        }
        if(resList[0].trimmed() == title.trimmed() + ".hide")
        {
            if(resList[1].toUpper()      == "TRUE")  frmDCB->setVisible(false);
            else if(resList[1].toUpper() == "FALSE") frmDCB->setVisible(true);
            else return "?";
            return "";
        }
    }
    return "?";
}

// Update — refreshes the setpoint and readback from MIPS.
//
// sVals can be:
//   ""              — read both Vsp and Vrb from MIPS
//   "vsp,vrb"       — apply provided values directly
//   ",vrb"          — skip Vsp update (only update readback)
//
// Readback background colour:
//   Green  — within DCB_ABS_ERROR_THRESHOLD V and DCB_REL_ERROR_THRESHOLD %
//   Red    — outside tolerance
//   White  — either value is empty
//
// Note: Vsp is only re-read from MIPS when VspEdited is false (i.e. the user
// has not recently changed the value). This prevents overwriting in-flight edits.
void DCBchannel::Update(QString sVals)
{
    QString     res;
    QStringList sValsList;
    bool        ok;

    sValsList = sVals.split(",");
    if(comms == NULL) return;
    if(UpdateOff) return;
    Updating = true;
    comms->rb.clear();

    if(sVals.isEmpty() || (sValsList[0].isEmpty() && VspEdited))
    {
        res = "GDCB," + QString::number(Channel) + "\n";
        res = comms->SendMess(res);
        if(res == "") { Updating = false; return; }
    }
    else res = sValsList[0];

    VspEdited = false;
    if(!res.isEmpty())
    {
        res.toFloat(&ok);
        if(!Vsp->hasFocus() && ok) Vsp->setText(res);
        if(!Vsp->hasFocus() && ok) CurrentVsp = Vsp->text().toFloat();
    }

    if(sVals.isEmpty())
    {
        res = "GDCBV," + QString::number(Channel) + "\n";
        res = comms->SendMess(res);
        if(res == "") { Updating = false; return; }
    }
    else res = sValsList[1];

    if(!res.isEmpty())
    {
        res.toFloat(&ok);
        if(ok) Vrb->setText(res);

        if((Vsp->text() == "") || (Vrb->text() == ""))
        {
            Vrb->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); }");
            Updating = false;
            return;
        }
        float error = fabs(Vsp->text().toFloat() - Vrb->text().toFloat());
        if((error > DCB_ABS_ERROR_THRESHOLD) && (error > fabs(Vsp->text().toFloat() * DCB_REL_ERROR_THRESHOLD)))
            Vrb->setStyleSheet("QLineEdit { background: rgb(255, 204, 204); }");
        else Vrb->setStyleSheet("QLineEdit { background: rgb(204, 255, 204); }");
    }
    Updating = false;
}

// VspChange — slot called when the user finishes editing the setpoint.
// Sends SDCB,channel,value to MIPS. If linked channels are configured,
// applies the same delta to all other channels in the group.
void DCBchannel::VspChange(void)
{
    if(!Vsp->isModified()) return;
    VspEdited = true;
    QString res = "SDCB," + QString::number(Channel) + "," + Vsp->text() + "\n";
    if(comms != NULL) comms->SendCommand(res.toStdString().c_str());

    if((LinkEnable) && (DCBs.count() > 0) && (CurrentVsp != Vsp->text().toFloat()))
    {
        float delta = CurrentVsp - Vsp->text().toFloat();
        foreach(DCBchannel *item, DCBs)
        {
            if(item != this)
            {
                item->CurrentVsp -= delta;
                item->Vsp->setText(QString::number(item->CurrentVsp, 'f', 2));
                item->CurrentVsp = item->Vsp->text().toFloat();
                item->Vsp->setModified(true);
                emit item->Vsp->editingFinished();
            }
        }
    }
    CurrentVsp = Vsp->text().toFloat();
    Vsp->setModified(false);
}

// Shutdown — saves the current setpoint and drives the channel to 0 V.
// Restore — returns the channel to the saved setpoint.
void DCBchannel::Shutdown(void)
{
    if(isShutdown) return;
    isShutdown    = true;
    activeVoltage = Vsp->text();
    Vsp->setText("0");
    Vsp->setModified(true);
    emit Vsp->editingFinished();
}

void DCBchannel::Restore(void)
{
    if(!isShutdown) return;
    isShutdown = false;
    Vsp->setText(activeVoltage);
    Vsp->setModified(true);
    emit Vsp->editingFinished();
}

// =============================================================================
// DCBoffset — draggable per-board offset/range adjustment widget
// =============================================================================

// DCBoffset — constructor. Records position and identity information;
// call Show() afterwards to create the visible offset widget.
DCBoffset::DCBoffset(QWidget *parent, QString name, QString MIPSname, int x, int y)
    : QWidget(parent)
{
    p      = parent;
    Title  = name;
    MIPSnm = MIPSname;
    X      = x;
    Y      = y;
    comms  = NULL;
}

// Show — creates the offset frame with a single Voff line edit, installs
// drag support on the frame and title label.
void DCBoffset::Show(void)
{
    frmDCBO   = new QFrame(p);        frmDCBO->setGeometry(X, Y, 170, 21);
    Voff      = new QLineEdit(frmDCBO); Voff->setGeometry(70,  0, 70, 21); Voff->setValidator(new QDoubleValidator);
    labels[0] = new QLabel(Title, frmDCBO); labels[0]->setGeometry(0,   0, 59, 16);
    labels[1] = new QLabel("V",   frmDCBO); labels[1]->setGeometry(150, 0, 21, 16);
    Voff->setToolTip("Offset/range control " + MIPSnm);
    connect(Voff, SIGNAL(editingFinished()), this, SLOT(VoffChange()));
    frmDCBO->installEventFilter(this);
    frmDCBO->setMouseTracking(true);
    labels[0]->installEventFilter(this);
    labels[0]->setMouseTracking(true);
}

// eventFilter — delegates drag-to-move to moveWidget(); no value-scroll support.
bool DCBoffset::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, frmDCBO, labels[0], event)) return true;
    return false;
}

// Report — returns a "title,offset" CSV string with the current Voff value.
QString DCBoffset::Report(void)
{
    QString res, title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    res = title + "," + Voff->text();
    return(res);
}

// SetValues — parses a "title,offset" CSV string and applies the offset,
// triggering VoffChange to send SDCBOF to MIPS.
// Returns false if the string does not match this widget's title.
bool DCBoffset::SetValues(QString strVals)
{
    QStringList resList;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!strVals.startsWith(title)) return false;
    resList = strVals.split(",");
    if(resList[0] != title) return false;
    if(resList.count() < 2) return false;
    Voff->setText(resList[1]);
    Voff->setModified(true);
    emit Voff->editingFinished();
    return true;
}

// ProcessCommand — scripting API for this offset widget.
//   title        — returns the current offset value
//   title=val    — sets the offset value
// Returns "?" for unrecognised commands.
QString DCBoffset::ProcessCommand(QString cmd)
{
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";
    if(cmd == title) return Voff->text();
    QStringList resList = cmd.split("=");
    if((resList.count() == 2) && (resList[0].trimmed() == title.trimmed()))
    {
        Voff->setText(resList[1]);
        Voff->setModified(true);
        emit Voff->editingFinished();
        return "";
    }
    return "?";
}

// Update — queries GDCBOF,channel from MIPS and refreshes Voff unless it
// currently has focus.
void DCBoffset::Update(void)
{
    QString res;

    if(comms == NULL) return;
    comms->rb.clear();
    res = "GDCBOF," + QString::number(Channel) + "\n";
    res = comms->SendMess(res);
    if(res == "") return;
    if(!Voff->hasFocus()) Voff->setText(res);
}

// VoffChange — slot called when the user finishes editing Voff. Sends
// SDCBOF,channel,value to MIPS.
void DCBoffset::VoffChange(void)
{
    if(comms == NULL) return;
    if(!Voff->isModified()) return;
    QString res = "SDCBOF," + QString::number(Channel) + "," + Voff->text() + "\n";
    comms->SendCommand(res.toStdString().c_str());
    Voff->setModified(false);
}

// =============================================================================
// DCBenable — draggable checkbox that controls DC bias board power (SDCPWR)
// =============================================================================

// DCBenable — constructor. Records position and identity information;
// call Show() afterwards to create the visible checkbox widget.
DCBenable::DCBenable(QWidget *parent, QString name, QString MIPSname, int x, int y)
    : QWidget(parent)
{
    p          = parent;
    Title      = name;
    MIPSnm     = MIPSname;
    X          = x;
    Y          = y;
    comms      = NULL;
    isShutdown = false;
}

// Show — creates the enable frame with a labelled QCheckBox, installs drag
// support, and connects checkStateChanged to DCBenaChange.
void DCBenable::Show(void)
{
    frmDCBena = new QFrame(p);       frmDCBena->setGeometry(X, Y, 170, 21);
    DCBena    = new QCheckBox(frmDCBena); DCBena->setGeometry(0, 0, 170, 21);
    DCBena->setText(Title);
    DCBena->setToolTip("Enables all DC bias channels on " + MIPSnm);
    connect(DCBena, SIGNAL(checkStateChanged(Qt::CheckState)), this, SLOT(DCBenaChange()));
    frmDCBena->installEventFilter(this);
    frmDCBena->setMouseTracking(true);
    DCBena->installEventFilter(this);
    DCBena->setMouseTracking(true);
}

// eventFilter — delegates drag-to-move to moveWidget().
bool DCBenable::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, frmDCBena, DCBena, event)) return true;
    return false;
}

// Report — returns a "title,ON|OFF" CSV string. While shut down, reports the
// saved enable state rather than the live checkbox state.
QString DCBenable::Report(void)
{
    QString res, title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    res = title + ",";
    if(isShutdown)
        res += activeEnableState ? "ON" : "OFF";
    else res += DCBena->isChecked() ? "ON" : "OFF";
    return(res);
}

// SetValues — parses a "title,ON|OFF" CSV string and applies it to the
// checkbox. While shut down, stores the state for Restore rather than
// applying it immediately. Returns false if the title does not match.
bool DCBenable::SetValues(QString strVals)
{
    QStringList resList;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!strVals.startsWith(title)) return false;
    resList = strVals.split(",");
    if(resList[0] != title) return false;
    if(resList.count() < 2) return false;
    if(isShutdown)
    {
        activeEnableState = resList[1].contains("ON");
        return true;
    }
    DCBena->setChecked(resList[1].contains("ON"));
    if(resList[1].contains("ON")) emit DCBena->checkStateChanged(Qt::Checked);
    else                          emit DCBena->checkStateChanged(Qt::Unchecked);
    return true;
}

// ProcessCommand — scripting API for this enable widget.
//   title        — returns ON or OFF
//   title=ON|OFF — sets the power state
// Returns "?" for unrecognised commands.
QString DCBenable::ProcessCommand(QString cmd)
{
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";
    if(cmd == title)
    {
        return DCBena->isChecked() ? "ON" : "OFF";
    }
    QStringList resList = cmd.split("=");
    if(resList.count() == 2)
    {
        if(resList[1] == "ON")       DCBena->setChecked(true);
        else if(resList[1] == "OFF") DCBena->setChecked(false);
        else return "?";
        if(resList[1] == "ON") emit DCBena->checkStateChanged(Qt::Checked);
        else                   emit DCBena->checkStateChanged(Qt::Unchecked);
        return "";
    }
    return "?";
}

// Update — queries GDCPWR from MIPS and syncs the checkbox state. Signals
// are blocked during the update to prevent DCBenaChange from firing.
void DCBenable::Update(void)
{
    QString res;

    if(comms == NULL) return;
    comms->rb.clear();
    res = comms->SendMess("GDCPWR\n");
    bool oldState = DCBena->blockSignals(true);
    if(res.contains("ON"))  DCBena->setChecked(true);
    if(res.contains("OFF")) DCBena->setChecked(false);
    DCBena->blockSignals(oldState);
}

// DCBenaChange — slot called when the checkbox state changes. Sends
// SDCPWR,ON or SDCPWR,OFF to MIPS.
void DCBenable::DCBenaChange(void)
{
    DCBena->setFocus();
    if(comms == NULL) return;
    QString res = DCBena->checkState() ? "SDCPWR,ON\n" : "SDCPWR,OFF\n";
    comms->SendCommand(res.toStdString().c_str());
}

// Shutdown — saves enable state and disables the board.
// Restore  — re-enables the board to its saved state.
void DCBenable::Shutdown(void)
{
    if(isShutdown) return;
    isShutdown        = true;
    activeEnableState = DCBena->checkState();
    DCBena->setChecked(false);
    emit DCBena->checkStateChanged(Qt::Unchecked);
}

void DCBenable::Restore(void)
{
    if(!isShutdown) return;
    isShutdown = false;
    DCBena->setChecked(activeEnableState);
    emit DCBena->checkStateChanged(Qt::Checked);
}
