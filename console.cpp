/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

// =============================================================================
// console.cpp — GAA Custom Electronics modifications
//
// Extended from the Qt SerialPort terminal example to serve as the MIPS
// terminal tab widget. Provides a black/yellow plain-text console with
// keyboard input forwarding, paste support, and Save/Load of terminal
// session data. Lines beginning with '#' in loaded files are treated as
// comments and not forwarded as commands.
//
// Depends on:  ui_mips.h, comms.h
// Modified by: Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
// =============================================================================
#include "console.h"

#include <QScrollBar>
#include <QApplication>
#include <QClipboard>
#include <qdatetime.h>
#include <QtCore/QDebug>

// Ctrl+V modifier flag on Mac (Qt::MetaModifier maps to Cmd key)
static const int PASTE_MODIFIER = 0x4000000;

// Console — constructor. Sets a black background / yellow text palette to
// match hardware terminal convention. localEchoEnabled defaults to false.
Console::Console(QWidget *parent, Ui::MIPS *w, Comms *c)
    : QPlainTextEdit(parent), localEchoEnabled(false)
{
    cui   = w;
    comms = c;

    // Black background, yellow text — matches hardware terminal convention
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::yellow);
    setPalette(p);
}

// resize — fits the console to fill its parent widget exactly.
void Console::resize(QWidget *parent)
{
    setFixedWidth(parent->width());
    setFixedHeight(parent->height());
}

// putData — appends incoming data to the console and auto-scrolls to the end.
void Console::putData(const QByteArray &data)
{
    insertPlainText(QString(data));
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

// setLocalEchoEnabled — when true, keystrokes are echoed locally in addition
// to being forwarded via getData.
void Console::setLocalEchoEnabled(bool set)
{
    localEchoEnabled = set;
}

// keyPressEvent — forwards keystrokes to the connected MIPS system via the
// getData signal. Backspace sends ASCII 8. Ctrl+V pastes clipboard contents.
// Arrow keys are passed to the base class for cursor movement only.
void Console::keyPressEvent(QKeyEvent *e)
{
    QClipboard *clipboard = QApplication::clipboard();
    QByteArray bksp;

    switch (e->key())
    {
    case Qt::Key_Backspace:
        bksp.append(8);
        emit getData(bksp);
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        QPlainTextEdit::keyPressEvent(e);
        break;
    case Qt::Key_V:
        if(e->modifiers() & PASTE_MODIFIER)
        {
            if(localEchoEnabled) QPlainTextEdit::appendPlainText(clipboard->text());
            emit getData(clipboard->text().toStdString().c_str());
            break;
        }
    default:
        if(localEchoEnabled) QPlainTextEdit::keyPressEvent(e);
        emit getData(e->text().toLocal8Bit());
    }
}

// mousePressEvent — claims keyboard focus so subsequent keystrokes are routed
// to this console widget.
void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

// mouseDoubleClickEvent — intentionally suppressed; double-click has no action.
void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

// contextMenuEvent — intentionally suppressed; right-click menu is disabled.
void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}

// Save — writes the current console contents to a text file with a timestamp
// header line.
void Console::Save(QString Filename)
{
    if(Filename == "") return;
    QFile file(Filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QDateTime dateTime = QDateTime::currentDateTime();
        stream << "# Terminal data, " + dateTime.toString() + "\n";
        stream << document()->toPlainText();
    }
    file.close();
    cui->statusBar->showMessage("Data saved to " + Filename, 2000);
}

// Load — reads a file line by line, displays each line in the console, and
// forwards non-comment lines (not starting with '#') as commands via getData.
void Console::Load(QString Filename)
{
    if(Filename == "") return;
    QFile file(Filename);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            line += "\n";
            QByteArray encodedString = line.toLatin1();
            putData(encodedString);
            if(line.trimmed().mid(0,1) != "#") emit getData(encodedString);
            QApplication::processEvents();
        } while(!line.isNull());
        file.close();
        cui->statusBar->showMessage("Settings loaded from " + Filename, 2000);
    }
}
