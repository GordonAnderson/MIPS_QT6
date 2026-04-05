// =============================================================================
// ccontrol.cpp
//
// Ccontrol — generic configurable control widget for the MIPS custom control
// panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Supports control types: LineEdit, CheckBox, Button, ComboBox.
// Each type has configurable Get, Set, and Readback MIPS commands.
//
// Depends on:  ccontrol.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "ccontrol.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Constructor — stores parent widget, control name, MIPS parameter name, type,
// and MIPS commands. Underscore characters in command strings are converted to
// commas to allow comma-containing commands to be represented in flat config
// files.
// -----------------------------------------------------------------------------
Ccontrol::Ccontrol(QWidget *parent, QString name, QString MIPSname, QString Type,
                   QString Gcmd, QString Scmd, QString RBcmd, QString Units, int x, int y)
    : QWidget(parent)
{
    p           = parent;
    Title       = name;
    MIPSnm      = MIPSname;
    GetCmd      = Gcmd.replace("_", ",");
    SetCmd      = Scmd.replace("_", ",");
    ReadbackCmd = RBcmd.replace("_", ",");
    UnitsText   = Units;
    Ctype       = Type;
    X           = x;
    Y           = y;
    comms       = nullptr;
    Step        = 1.0;
    isShutdown  = false;
    Updating    = false;
    UpdateOff   = false;
    ShutdownValue.clear();
    Dtype    = "Double";
    comboBox = nullptr;
}

// eventFilter — handles widget drag (moveWidget) and mouse-wheel value
// adjustment (adjustValue). String-typed controls pass directly through.
bool Ccontrol::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == frmCc)    if (moveWidget(obj, frmCc,    frmCc,    event)) return true;
    if (obj == pbButton) if (moveWidget(obj, pbButton, pbButton, event)) return true;

    if (Dtype.toUpper() == "STRINGL") return QObject::eventFilter(obj, event);
    if (Dtype.toUpper() == "STRING")  return QObject::eventFilter(obj, event);
    if (Updating) return true;
    UpdateOff = true;
    if (adjustValue(obj, Vsp, event, Step))
    {
        UpdateOff = false;
        return true;
    }
    UpdateOff = false;
    return QObject::eventFilter(obj, event);
}

// Show — creates the appropriate UI widgets for the configured control type
// and installs event filters so the widget can be repositioned by dragging.
//
// LineEdit: one or two QLineEdits (setpoint and optional readback).
// CheckBox: a single checkbox labelled with Title.
// Button:   a push button labelled with Title.
// ComboBox: a labelled combo box with optional units label.
void Ccontrol::Show(void)
{
    updateCount = generateRandomInt(1, skipCount);
    if (Ctype == "LineEdit")
    {
        frmCc    = new QFrame(p);
        labels[0] = new QLabel(Title, frmCc);
        labels[0]->setGeometry(0, 0, 59, 16);
        labels[1] = new QLabel(UnitsText, frmCc);
        // If readback command or set command is empty there is only one line-edit box,
        // otherwise there are two.
        if (SetCmd.isEmpty() || ReadbackCmd.isEmpty())
        {
            if (Dtype.toUpper() == "STRINGL") frmCc->setGeometry(X, Y, 400, 21);
            else                              frmCc->setGeometry(X, Y, 175, 21);
            if (SetCmd.isEmpty())
            {
                Vrb = new QLineEdit(frmCc);
                Vrb->setGeometry(70, 0, 70, 21);
                Vrb->setReadOnly(true);
            }
            else if (ReadbackCmd.isEmpty())
            {
                Vsp = new QLineEdit(frmCc);
                if (Dtype.toUpper() == "STRINGL") Vsp->setGeometry(70, 0, 300, 21);
                else                              Vsp->setGeometry(70, 0, 70, 21);
                if (Dtype.toUpper() == "DOUBLE") Vsp->setValidator(new QDoubleValidator);
                Vsp->setToolTip(MIPSnm + "," + SetCmd);
                connect(Vsp, &QLineEdit::returnPressed,   this, [this]() {Vsp->setModified(true);});
                connect(Vsp, &QLineEdit::editingFinished, this, &Ccontrol::VspChange);
            }
            labels[1]->setGeometry(150, 0, 30, 16);
        }
        else
        {
            frmCc->setGeometry(X, Y, 245, 21);
            Vsp = new QLineEdit(frmCc);
            Vsp->setGeometry(70, 0, 70, 21);
            Vrb = new QLineEdit(frmCc);
            Vrb->setGeometry(140, 0, 70, 21);
            Vrb->setReadOnly(true);
            labels[1]->setGeometry(220, 0, 30, 16);
            Vsp->setToolTip(MIPSnm + "," + SetCmd);
            connect(Vsp, &QLineEdit::returnPressed,   this, [this]() {Vsp->setModified(true);});
            connect(Vsp, &QLineEdit::editingFinished, this, &Ccontrol::VspChange);
        }
    }
    if (Ctype == "CheckBox")
    {
        frmCc  = new QFrame(p);
        frmCc->setGeometry(X, Y, 241, 21);
        chkBox = new QCheckBox(frmCc);
        chkBox->setGeometry(0, 0, 200, 21);
        chkBox->setText(Title);
        connect(chkBox, &QCheckBox::checkStateChanged, this, &Ccontrol::chkBoxStateChanged);
    }
    if (Ctype == "Button")
    {
        pbButton = new QPushButton(Title, p);
        pbButton->setGeometry(X, Y, 150, 32);
        pbButton->setAutoDefault(false);
        connect(pbButton, &QPushButton::pressed, this, &Ccontrol::pbButtonPressed);
    }
    if (Ctype == "ComboBox")
    {
        frmCc    = new QFrame(p);
        frmCc->setGeometry(X, Y, 241, 21);
        labels[0] = new QLabel(Title, frmCc);
        labels[0]->setGeometry(0, 0, 59, 16);
        comboBox = new QComboBox(frmCc);
        comboBox->setGeometry(70, 0, 70, 21);
        labels[1] = new QLabel(UnitsText, frmCc);
        labels[1]->setGeometry(150, 0, 30, 16);
        connect(comboBox, &QComboBox::currentIndexChanged, this, &Ccontrol::comboBoxChanged);
    }
    if (frmCc != nullptr)
    {
        frmCc->installEventFilter(this);
        frmCc->setMouseTracking(true);
    }
    if (labels[0] != nullptr)
    {
        labels[0]->installEventFilter(this);
        labels[0]->setMouseTracking(true);
    }
    if (pbButton != nullptr)
    {
        pbButton->installEventFilter(this);
        pbButton->setMouseTracking(true);
    }
    if (Vsp != nullptr)
    {
        Vsp->installEventFilter(this);
        Vsp->setMouseTracking(true);
    }
}

// Update — polls the hardware for the current value and updates the UI.
// Updates are throttled by skipCount to reduce communication load; the first
// update after Show() always runs regardless of the counter.
void Ccontrol::Update(void)
{
    QString res;

    if (++updateCount > skipCount) updateCount = 1;
    if (comms == nullptr) return;
    comms->rb.clear();
    if (Ctype == "LineEdit")
    {
        if (!GetCmd.isEmpty())
        {
            if ((updateCount != 1) && !firstUpdate) return;
            firstUpdate = false;
            res = comms->SendMess(GetCmd + "\n");
            if (res == "") return;
            if (!Vsp->hasFocus()) Vsp->setText(res);
        }
        if (!ReadbackCmd.isEmpty())
        {
            res = comms->SendMess(ReadbackCmd + "\n");
            if (res == "") return;
            Vrb->setText(res);
        }
    }
    if (Ctype == "CheckBox")
    {
        // If there is a readback string it is the update command. The string
        // contains the checked and unchecked responses delimited by commas.
        if (!ReadbackCmd.isEmpty())
        {
            QStringList resList = ReadbackCmd.split(",");
            if (resList.count() >= 3)
            {
                if ((updateCount != 1) && !firstUpdate) return;
                firstUpdate = false;
                if (resList.count() == 3) res = comms->SendMess(resList[0] + "\n");
                if (resList.count() == 4) res = comms->SendMess(resList[0] + "," + resList[1] + "\n");
                chkBox->blockSignals(true);
                if (res == resList[resList.count() - 2]) chkBox->setChecked(true);
                else if (res == resList[resList.count() - 1]) chkBox->setChecked(false);
                chkBox->blockSignals(false);
            }
            frmCc->repaint();
        }
    }
    if (Ctype == "ComboBox")
    {
        if (!GetCmd.isEmpty())
        {
            if ((updateCount != 1) && !firstUpdate) return;
            firstUpdate = false;
            res = comms->SendMess(GetCmd + "\n");
            if (res == "") return;
            int i = comboBox->findText(res.trimmed());
            if (i < 0) return;
            comboBox->setCurrentIndex(i);
        }
        frmCc->repaint();
    }
}

// Report — returns a string representation of the current control state for
// save/restore operations. Format varies by control type.
QString Ccontrol::Report(void)
{
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (Ctype == "LineEdit")
    {
        if (!GetCmd.isEmpty() || !SetCmd.isEmpty())
        {
            res += "," + Vsp->text();
            if (isShutdown) res += "," + ActiveValue;
            else            res += "," + Vsp->text();
        }
        if (!ReadbackCmd.isEmpty()) res += "," + Vrb->text();
        return res;
    }
    if (Ctype == "CheckBox")
    {
        if (!ReadbackCmd.isEmpty())
        {
            QStringList resList = ReadbackCmd.split("_");
            if (resList.count() == 3)
            {
                if (chkBox->isChecked()) return resList[1];
                else                     return resList[2];
            }
        }
        if (chkBox->isChecked()) return res + "," + "TRUE";
        else                     return res + "," + "FALSE";
    }
    if (Ctype == "ComboBox")
    {
        return res + "," + comboBox->currentText();
    }
    return "";
}

// SetValues — restores the control state from a save-file line. Returns true
// if the prefix matches this control, false otherwise.
bool Ccontrol::SetValues(QString strVals)
{
    QStringList resList;
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (Ctype == "LineEdit")
    {
        if (!strVals.startsWith(res)) return false;
        resList = strVals.split(",");
        if (resList.count() < 2) return false;
        if (resList[0] != res) return false;
        if (SetCmd.isEmpty()) return false;
        if (isShutdown) ActiveValue = resList[1];
        else
        {
            Vsp->setText(resList[1]);
            Vsp->setModified(true);
            emit Vsp->editingFinished();
        }
        return true;
    }
    if (Ctype == "CheckBox")
    {
        if (!strVals.startsWith(res)) return false;
        resList = strVals.split(",");
        if (resList.count() < 2) return false;
        if (resList[0] != res) return false;
        if (!ReadbackCmd.isEmpty())
        {
            QStringList resCmd = ReadbackCmd.split(",");
            if (resCmd.count() == 3)
            {
                bool returnstate = true;
                if      (resCmd[1] == resList[1]) chkBox->setChecked(true);
                else if (resCmd[2] == resList[1]) chkBox->setChecked(false);
                else returnstate = false;
                return returnstate;
            }
        }
        bool returnstate = true;
        if      (resList[1] == "TRUE")  chkBox->setChecked(true);
        else if (resList[1] == "FALSE") chkBox->setChecked(false);
        else returnstate = false;
        return returnstate;
    }
    if (Ctype == "ComboBox")
    {
        if (!strVals.startsWith(res)) return false;
        resList = strVals.split(",");
        if (resList.count() < 2) return false;
        if (resList[0] != res) return false;
        int i = comboBox->findText(resList[1].trimmed());
        if (i < 0) return false;
        comboBox->setCurrentIndex(i);
        comboBoxChanged(i);
        return true;
    }
    return false;
}

// Shutdown — for LineEdit controls: saves the current value, writes the
// configured ShutdownValue to the hardware, and sets isShutdown.
void Ccontrol::Shutdown(void)
{
    if (Ctype == "LineEdit")
    {
        if (ShutdownValue.isEmpty()) return;
        if (isShutdown) return;
        isShutdown  = true;
        ActiveValue = Vsp->text();
        Vsp->setText(ShutdownValue);
        Vsp->setModified(true);
        emit Vsp->editingFinished();
    }
}

// Restore — for LineEdit controls: re-applies the value saved by Shutdown().
void Ccontrol::Restore(void)
{
    if (Ctype == "LineEdit")
    {
        if (!isShutdown) return;
        isShutdown = false;
        Vsp->setText(ActiveValue);
        Vsp->setModified(true);
        emit Vsp->editingFinished();
    }
}

// SetList — populates the combo box with a comma-separated list of options.
void Ccontrol::SetList(QString strOptions)
{
    if (comboBox != nullptr)
    {
        comboBox->clear();
        QStringList resList = strOptions.split(",");
        for (int i = 0; i < resList.count(); i++)
        {
            comboBox->addItem(resList[i]);
        }
    }
}

// ProcessCommand — handles text commands for reading or writing the control
// state, and for configuring scripting, colour, visibility, and MIPS commands.
// Returns "?" if the command does not match this control.
//
// Supported commands:
//   title              — returns the setpoint (LineEdit/ComboBox) or state (CheckBox)
//   title=val          — sets the value
//   title.readback     — returns the readback value (LineEdit only)
//   title.script=name  — assigns a script to the change event
//   title.scriptCall=x — assigns a script call string
//   title.color=c      — sets the frame background colour
//   title.hide=TRUE/FALSE — shows or hides the frame
//   title.getCmd=c     — overrides the Get command
//   title.setCmd=c     — overrides the Set command
//   title.rbCmd=c      — overrides the Readback command
//   title.mips=name    — overrides the MIPS parameter name
//   title.toolTip=text — sets the tooltip on the active widget
QString Ccontrol::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    QStringList resList = cmd.split("=");
    if ((resList.count() == 2) && (resList[0] == res + ".script"))
    {
        scriptName = resList[1].trimmed();
        return "";
    }
    if ((resList.count() == 2) && (resList[0] == res + ".scriptCall"))
    {
        scriptCall = resList[1].trimmed();
        return "";
    }
    if ((resList.count() == 2) && (resList[0] == res + ".color"))
    {
        if (frmCc != nullptr) frmCc->setStyleSheet("background-color: " + resList[1].trimmed());
        return "";
    }
    if ((resList.count() == 2) && (resList[0] == res + ".hide"))
    {
        if (frmCc != nullptr)
        {
            if      (resList[1].toUpper() == "TRUE")  frmCc->setVisible(false);
            else if (resList[1].toUpper() == "FALSE") frmCc->setVisible(true);
            else return "?";
        }
        return "";
    }
    if ((resList.count() == 2) && (resList[0] == res + ".getCmd"))
    {
        GetCmd = resList[1].trimmed();
        return "";
    }
    if ((resList.count() == 2) && (resList[0] == res + ".setCmd"))
    {
        SetCmd = resList[1].trimmed();
        return "";
    }
    if ((resList.count() == 2) && (resList[0] == res + ".rbCmd"))
    {
        ReadbackCmd = resList[1].trimmed();
        return "";
    }
    if ((resList.count() == 2) && (resList[0] == res + ".mips"))
    {
        MIPSnm = resList[1].trimmed();
        return "";
    }
    if (cmd.startsWith(res + ".toolTip"))
    {
        int startIndex = cmd.indexOf('=') + 1;
        if      (Ctype == "LineEdit")  Vsp->setToolTip(cmd.mid(startIndex));
        else if (Ctype == "CheckBox")  chkBox->setToolTip(cmd.mid(startIndex));
        else if (Ctype == "Button")    pbButton->setToolTip(cmd.mid(startIndex));
        else if (Ctype == "ComboBox")  comboBox->setToolTip(cmd.mid(startIndex));
        return "";
    }
    if (Ctype == "LineEdit")
    {
        if (!cmd.startsWith(res)) return "?";
        if (cmd == res)
        {
            if (!SetCmd.isEmpty() || !GetCmd.isEmpty()) return Vsp->text();
            if (!ReadbackCmd.isEmpty()) return Vrb->text();
        }
        if (cmd == res + ".getCmd")  return GetCmd;
        if (cmd == res + ".setCmd")  return SetCmd;
        if (cmd == res + ".rbCmd")   return ReadbackCmd;
        if (cmd == res + ".mips")    return MIPSnm;
        if (cmd == res + ".script")  return scriptName;
        if (cmd == res + ".readback")
        {
            if ((!SetCmd.isEmpty() || !GetCmd.isEmpty()) && !ReadbackCmd.isEmpty()) return Vrb->text();
            return "?";
        }
        QStringList resList = cmd.split("=");
        if ((resList.count() == 2) && !SetCmd.isEmpty() && (resList[0] == res) && (Vsp != nullptr))
        {
            Vsp->setText(resList[1].trimmed());
            Vsp->setModified(true);
            emit Vsp->editingFinished();
            return "";
        }
        return "?";
    }
    if (Ctype == "CheckBox")
    {
        if (!cmd.startsWith(res)) return "?";
        if (cmd == res)
        {
            if (!ReadbackCmd.isEmpty())
            {
                QStringList resCmd = ReadbackCmd.split("_");
                if (resCmd.count() == 3)
                {
                    if (chkBox->isChecked()) return resCmd[1];
                    else                     return resCmd[2];
                }
            }
            if (chkBox->isChecked()) return "TRUE";
            else                     return "FALSE";
        }
        QStringList resList = cmd.split("=");
        if ((resList.count() == 2) && !SetCmd.isEmpty() && (resList[0] == res))
        {
            if (!ReadbackCmd.isEmpty())
            {
                QStringList resCmd = ReadbackCmd.split("_");
                if (resCmd.count() == 3)
                {
                    if      (resCmd[1] == resList[1]) chkBox->setChecked(true);
                    else if (resCmd[2] == resList[1]) chkBox->setChecked(false);
                    else return "?";
                    return "";
                }
            }
            if      (resList[1] == "TRUE")  chkBox->setChecked(true);
            else if (resList[1] == "FALSE") chkBox->setChecked(false);
            else return "?";
            return "";
        }
        if ((resList.count() == 2) && (resList[0] == res + ".color"))
        {
            frmCc->setStyleSheet("background-color: " + resList[1].trimmed());
            return "";
        }
    }
    if (Ctype == "Button")
    {
        if (!cmd.startsWith(res)) return "?";
        if (cmd == res)
        {
            pbButtonPressed();
            return "";
        }
    }
    if (Ctype == "ComboBox")
    {
        if (!cmd.startsWith(res)) return "?";
        if (cmd == res) return comboBox->currentText();
        QStringList resList = cmd.split("=");
        int i = comboBox->findText(resList[1].trimmed());
        if ((i < 0) || (resList[0] != res)) return "?";
        comboBox->setCurrentIndex(i);
        comboBoxChanged(i);
        return "";
    }
    return "?";
}

// VspChange — called when the user presses Return in the setpoint line-edit.
// Sends the new value to the hardware and emits change() if a script is assigned.
void Ccontrol::VspChange(void)
{
    if (!Vsp->isModified()) return;
    updateCount = 0;
    QString res = SetCmd + "," + Vsp->text() + "\n";
    if (comms != nullptr) comms->SendCommand(res);
    Vsp->setModified(false);
    if (!scriptName.isEmpty()) emit change(scriptName);
}

// pbButtonPressed — sends the Set command to the hardware when the button is
// pressed, and emits change() if a script is assigned.
void Ccontrol::pbButtonPressed(void)
{
    pbButton->setFocus();
    pbButton->setDown(false);
    if (comms != nullptr) comms->SendCommand(SetCmd + "\n");
    if (!scriptName.isEmpty()) emit change(scriptName);
}

// chkBoxStateChanged — sends the Set or Get command to the hardware when the
// checkbox state changes, and emits change() if a script is assigned.
void Ccontrol::chkBoxStateChanged(Qt::CheckState state)
{
    chkBox->setFocus();
    if (comms != nullptr)
    {
        updateCount = 0;
        if      (state == Qt::Checked)   comms->SendCommand(SetCmd + "\n");
        else if (state == Qt::Unchecked) comms->SendCommand(GetCmd + "\n");
    }
    if (!scriptName.isEmpty()) emit change(scriptName);
}

// comboBoxChanged — sends the Set command with the selected item text to the
// hardware when the combo box selection changes, and emits change() if a
// script is assigned.
void Ccontrol::comboBoxChanged(int i)
{
    updateCount = 0;
    if (comms != nullptr) comms->SendCommand(SetCmd + "," + comboBox->itemText(i) + "\n");
    if (!scriptName.isEmpty()) emit change(scriptName);
}
