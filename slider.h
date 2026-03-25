// =============================================================================
// slider.h
//
// Slider — horizontal or vertical slider widget for the MIPS custom control
// panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef SLIDER_H
#define SLIDER_H

#include <QWidget>
#include <QSlider>

// -----------------------------------------------------------------------------
// Slider
//
// Displays a horizontal or vertical QSlider with configurable min/max range.
// The current value can be read and written via ProcessCommand(). Emits
// change(scriptName) when the slider moves, if a script is assigned.
// -----------------------------------------------------------------------------
class Slider : public QWidget
{
    Q_OBJECT
signals:
    void change(QString scriptName);
public:
    Slider(QWidget *parent, QString name, QString orientation, int min, int max,
           int width, int x, int y);
    void    Show(void);
    QString Report(void);
    bool    SetValues(QString strVals);
    QString ProcessCommand(QString cmd);

    QWidget *p;
    QString  Title;
    int      X, Y;
    QString  scriptName = "";
    QString  scriptCall = "";
    QString  Orientation;
    int      Min;
    int      Max;
    int      Width;

private:
    QSlider *slider;

public slots:
    void sliderChange(int value);
};

#endif // SLIDER_H
