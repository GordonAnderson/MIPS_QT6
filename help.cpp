// =============================================================================
// help.cpp
//
// Implements the Help dialog — a resizable plain-text viewer used throughout
// the application to display module help files and scripting console output.
// Uses a monospaced font with platform-appropriate point size and 8-space tabs.
//
// Depends on:  ui_help.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "help.h"
#include <ui_help.h>
#include <QTextOption>
#include <QDir>

// Help — constructor. Sets up the plain-text viewer with a monospaced font
// (Courier, 14 pt on macOS / 11 pt elsewhere) and 8-space tab stops.
Help::Help(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Help)
{
    ui->setupUi(this);

    // Configure monospaced font with platform-appropriate size
    QFont font = ui->plainTextEdit->font();
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
#if defined(Q_OS_MAC)
    font.setPointSize(14);
#else
    font.setPointSize(11);
#endif
    QFontMetrics metrics(font);
    ui->plainTextEdit->setTabStopDistance(8 * metrics.horizontalAdvance(' '));
    ui->plainTextEdit->setFont(font);
}

// ~Help — destructor. Releases the UI form object.
Help::~Help()
{
    delete ui;
}

// getText — returns the current plain-text content of the viewer.
QString Help::getText(void)
{
    return(ui->plainTextEdit->toPlainText());
}

// setText — replaces the viewer content with comment.
void Help::setText(QString comment)
{
    ui->plainTextEdit->setPlainText(comment);
}

// appendText — appends comment as a new paragraph to the viewer.
void Help::appendText(QString comment)
{
    ui->plainTextEdit->appendPlainText(comment);
}

// SetTitle — sets the dialog window title.
void Help::SetTitle(QString title)
{
    QWidget::setWindowTitle(title);
}

// LoadStr — replaces the viewer content with DisplayText directly (no file I/O).
void Help::LoadStr(QString DisplayText)
{
    ui->plainTextEdit->setPlainText(DisplayText);
}

// LoadHelpText — clears the viewer, disables word wrap, and loads the
// contents of FileName line by line. Scrolls to the top after loading.
// Does nothing if FileName is empty or the file cannot be opened.
void Help::LoadHelpText(QString FileName)
{
    ui->plainTextEdit->clear();
    ui->plainTextEdit->setWordWrapMode(QTextOption::NoWrap);
    if(FileName == "") return;
    QFile file(FileName);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            ui->plainTextEdit->appendPlainText(line);
        } while(!line.isNull());
        file.close();
    }
    ui->plainTextEdit->moveCursor(QTextCursor::Start);
}

// resizeEvent — keeps the text edit filling the full dialog area on resize.
void Help::resizeEvent(QResizeEvent*)
{
    ui->plainTextEdit->setFixedWidth(Help::width());
    ui->plainTextEdit->setFixedHeight(Help::height());
}
