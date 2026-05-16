// =============================================================================
// eventcontrol.cpp
//
// EventControl — single-event editor widget (label + QLineEdit).
// Extracted from timinggenerator.cpp during Phase 3 refactoring.
//
// Depends on:  timinggenerator.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "timinggenerator.h"
#include "ui_timinggenerator.h"
#include "Utilities.h"
#include "controlpanel.h"

/*! \brief EventControl::EventControl
 * Constructor. Stores parent, name, event name, and position.
 */
EventControl::EventControl(QWidget *parent, QString name, QString Ename, int x, int y)
{
    p      = parent;
    Title  = name;
    ECname = Ename;
    X      = x;
    Y      = y;
    comms  = nullptr;
    Gcommand.clear();
    Scommand.clear();
}

/*! \brief EventControl::Show
 * Creates the frame, label, and value line-edit; connects returnPressed to EventChange.
 */
void EventControl::Show(void)
{
    frmEvent = new QFrame(p); frmEvent->setGeometry(X,Y,170,21);
    EventValue = new QLineEdit(frmEvent); EventValue->setGeometry(70,0,70,21);
    label = new QLabel(Title,frmEvent); label->setGeometry(0,0,59,16);
    EventValue->setToolTip("Timing generator event value editor, " + ECname);
    connect(EventValue, &QLineEdit::returnPressed,   this, [this]() {EventValue->setModified(true);});
    connect(EventValue, &QLineEdit::editingFinished, this, &EventControl::EventChange);
}

/*! \brief EventControl::Update
 * Reads the current event value from MIPS and updates the line-edit.
 */
void EventControl::Update(void)
{
    QString res;

    if(Gcommand.isEmpty()) return;
    if(comms == nullptr) return;
    comms->rb.clear();
    res = Gcommand + "\n";
    res = comms->SendMess(res);
    if(res == "") return;
    if(res.contains("?")) return;
    if(!EventValue ->hasFocus()) EventValue->setText(res);
}

/*! \brief EventControl::Report
 * Returns a save-file line containing the event name and current value.
 */
QString EventControl::Report(void)
{
    QString res;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    res = title + "," + EventValue->text();
    return(res);
}

/*! \brief EventControl::SetValues
 * Restores the event value from a save-file line if the name matches.
 */
bool EventControl::SetValues(QString strVals)
{
    QStringList resList;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!strVals.startsWith(title)) return false;
    resList = strVals.split(",");
    if(resList.count() < 2) return false;
    EventValue->setText(resList[1]);
    EventValue->setModified(true);
    emit EventValue->editingFinished();
    return true;
}

// Sets or returns this controls value
/*! \brief EventControl::ProcessCommand
 * Reads or writes the event value in response to a command string.
 */
QString EventControl::ProcessCommand(QString cmd)
{
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";
    if(cmd == title) return EventValue->text();
    QStringList resList = cmd.split("=");
    if(resList.count()==2)
    {
       EventValue->setText(resList[1]);
       EventValue->setModified(true);
       emit EventValue->editingFinished();
       return "";
    }
    return "?";
}

/*! \brief EventControl::EventChange
 * Slot: sends the new event value to MIPS when the user commits a change.
 */
void EventControl::EventChange(void)
{
    emit EventChanged(ECname,EventValue->text());
    if(Scommand.isEmpty()) return;
    if(!EventValue->isModified()) return;
    QString res = Scommand + "," + EventValue->text() + "\n";
    if(comms != nullptr) comms->SendCommand(res.toLocal8Bit());
    EventValue->setModified(false);
}

// *************************************************************************************************
// TimingGenerator implementation  *****************************************************************
// *************************************************************************************************
