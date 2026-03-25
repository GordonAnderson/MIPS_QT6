// =============================================================================
// help.h
//
// Class declaration for Help.
// See help.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef HELP_H
#define HELP_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QFontMetrics>
#include <QObject>
#include <QApplication>

namespace Ui {
class Help;
}

// -----------------------------------------------------------------------------
// Help — resizable plain-text viewer dialog. Used to display module help
// files (loaded from disk) and scripting console output. Font is monospaced
// with 8-space tab stops.
// -----------------------------------------------------------------------------
class Help : public QDialog
{
    Q_OBJECT

public:
    explicit Help(QWidget *parent = 0);
    ~Help();

    void    LoadHelpText(QString FileName);  // load a text file into the viewer
    void    LoadStr(QString DisplayText);    // replace viewer contents with a string
    void    SetTitle(QString title);         // set the dialog window title
    QString getText(void);                   // return current viewer contents
    void    setText(QString);               // replace viewer contents
    void    appendText(QString);            // append a line to the viewer

    virtual void resizeEvent(QResizeEvent* event);

private:
    Ui::Help *ui;
};

#endif // HELP_H
