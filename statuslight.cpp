// =============================================================================
// statuslight.cpp
//
// StatusLight — traffic-light style status indicator for the MIPS custom
// control panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  statuslight.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "statuslight.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Constructor — stores parent widget, title, and position. Clears the Status
// and Mode strings.
// -----------------------------------------------------------------------------
StatusLight::StatusLight(QWidget *parent, QString name, int x, int y)
    : QWidget(parent)
{
    p     = parent;
    Title = name;
    X     = x;
    Y     = y;
    Status.clear();
    Mode.clear();
}

// Show — creates the title label, message label, and traffic light widget;
// installs an event filter on the label so the whole group can be dragged.
void StatusLight::Show(void)
{
    Message = new QLabel(p);
    Message->setGeometry(X, Y, 1, 1);

    label = new QLabel(Title, p);
    label->setGeometry(X, Y, 1, 1);
    label->adjustSize();
    label->setGeometry(X + 20 - (label->width() / 2), Y, label->width(), label->height());

    TL = new QLabel(p);
    TL->setGeometry(X, Y + label->height(), 40, 96);

    widget = new TrafficLightWidget(TL);
    widget->resize(40, 96);
    widget->show();

    label->installEventFilter(this);
    label->setMouseTracking(true);
}

// eventFilter — handles widget drag via moveWidget(). On a drag, repositions
// the title label, traffic light, and message label to follow.
bool StatusLight::eventFilter(QObject *obj, QEvent *event)
{
    if (moveWidget(obj, label, label, event))
    {
        X = label->x();
        Y = label->y();
        Message->setGeometry(X, Y, 1, 1);
        label->setGeometry(X + 20 - (label->width() / 2), Y, label->width(), label->height());
        TL->setGeometry(X, Y + label->height(), 40, 96);
        ShowMessage();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

// ShowMessage — composes the status and mode strings into a single label
// and repositions it below the traffic light widget.
void StatusLight::ShowMessage(void)
{
    QString mes = Status;
    if (!Mode.isEmpty()) mes = mes + "\n" + Mode;
    Message->setText(mes);
    Message->adjustSize();
    Message->setGeometry(X + 20 - (Message->width() / 2),
                         Y + 96 + label->height(),
                         Message->width(), Message->height());
}

// ProcessCommand — reads or sets the traffic light colour, status message, or
// mode text. Returns "?" if the command does not match this widget.
//
// Supported commands:
//   title             — returns "GREEN", "RED", "YELLOW", or "NONE"
//   title=GREEN/RED/YELLOW — sets the active light
//   title.message     — returns the status message string
//   title.message=txt — sets the status message string
//   title.mode        — returns the mode string
//   title.mode=txt    — sets the mode string
QString StatusLight::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (!cmd.startsWith(res)) return "?";
    if ((res + ".message") == cmd.trimmed()) return Status;
    if ((res + ".mode")    == cmd.trimmed()) return Mode;
    if (cmd.trimmed() == res)
    {
        if (widget->greenLight()->isOn())  return "GREEN";
        if (widget->redLight()->isOn())    return "RED";
        if (widget->yellowLight()->isOn()) return "YELLOW";
        return "NONE";
    }
    QStringList resList = cmd.split("=");
    if (resList.count() == 2)
    {
        if ((res + ".message") == resList[0].trimmed())
        {
            Status = resList[1];
            ShowMessage();
            return "";
        }
        if ((res + ".mode") == resList[0].trimmed())
        {
            Mode = resList[1];
            ShowMessage();
            return "";
        }
        widget->greenLight()->turnOff();
        widget->redLight()->turnOff();
        widget->yellowLight()->turnOff();
        if (resList[1].trimmed() == "GREEN")  widget->greenLight()->turnOn();
        if (resList[1].trimmed() == "RED")    widget->redLight()->turnOn();
        if (resList[1].trimmed() == "YELLOW") widget->yellowLight()->turnOn();
        return "";
    }
    return "?";
}
