// =============================================================================
// slider.cpp
//
// Slider — horizontal or vertical slider widget for the MIPS custom control
// panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  slider.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "slider.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Constructor — stores parent widget, title, orientation, range, width, and
// position.
// -----------------------------------------------------------------------------
Slider::Slider(QWidget *parent, QString name, QString orientation, int min, int max,
               int width, int x, int y)
    : QWidget(parent)
{
    p           = parent;
    Title       = name;
    X           = x;
    Y           = y;
    Orientation = orientation;
    Min         = min;
    Max         = max;
    Width       = width;
}

// Show — creates the QSlider with the configured orientation and range,
// sets its initial value to zero, and connects the valueChanged signal.
void Slider::Show(void)
{
    if (Orientation.toUpper().contains("H"))
    {
        slider = new QSlider(Qt::Horizontal, p);
        slider->setGeometry(X, Y, Width, 21);
    }
    else
    {
        slider = new QSlider(Qt::Vertical, p);
        slider->setGeometry(X, Y, 21, Width);
    }
    if (slider == nullptr) return;
    slider->setMinimum(Min);
    slider->setMaximum(Max);
    slider->setValue(0);
    slider->setToolTip(Title);
    slider->show();
    connect(slider, &QSlider::valueChanged, this, &Slider::sliderChange);
}

// ProcessCommand — reads or writes the slider value, and configures scripting,
// tooltip, and colour properties.
// Returns "?" if the command does not match this slider.
//
// Supported commands:
//   title          — returns the current slider value
//   title=val      — sets the slider value (must be within Min..Max)
//   title.script=name    — assigns a script to the change event
//   title.scriptCall=name — assigns a script call string
//   title.toolTip=text   — sets the slider tooltip
//   title.color=colour   — sets the slider background colour
QString Slider::ProcessCommand(QString cmd)
{
    QString res;

    if (slider == nullptr) return "?";
    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (!cmd.startsWith(res)) return "?";
    QStringList cmdList = cmd.split("=");
    if (cmdList.count() == 1)
    {
        if (cmdList[0] == res) return QString::number(slider->value());
    }
    if (cmdList.count() == 2)
    {
        if (cmdList[0] == res)
        {
            int val = cmdList[1].toInt();
            if (val < Min || val > Max) return "?";
            slider->setValue(val);
            return "";
        }
        if (cmdList[0] == res + ".script")
        {
            scriptName = cmdList[1].trimmed();
            return "";
        }
        if (cmdList[0] == res + ".scriptCall")
        {
            scriptCall = cmdList[1].trimmed();
            return "";
        }
        if (cmdList[0] == res + ".toolTip")
        {
            slider->setToolTip(cmdList[1].trimmed());
            return "";
        }
        if (cmdList[0] == res + ".color")
        {
            slider->setStyleSheet("background-color: " + cmdList[1].trimmed());
            return "";
        }
    }
    return "?";
}

// Report — returns a comma-separated string of the panel name, title, and
// current slider value, for use in save/restore operations.
QString Slider::Report(void)
{
    QString res;

    if (slider == nullptr) return "";
    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    res += "," + QString::number(slider->value());
    return res;
}

// SetValues — restores the slider value from a save-file line. Returns true
// if the prefix matches this slider and the value is in range, false otherwise.
bool Slider::SetValues(QString strVals)
{
    QStringList resList;
    QString res;

    if (slider == nullptr) return false;
    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (!strVals.startsWith(res)) return false;
    resList = strVals.split(",");
    if (resList.count() != 2) return false;
    if (resList[0] != res) return false;
    int val = resList[1].toInt();
    if (val < Min || val > Max) return false;
    slider->setValue(val);
    return true;
}

// sliderChange — emits change(scriptName) when the slider value changes, if
// a script is assigned.
void Slider::sliderChange(int value)
{
    Q_UNUSED(value);
    if (!scriptName.isEmpty()) emit change(scriptName);
}
