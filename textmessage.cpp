// =============================================================================
// textmessage.cpp
//
// TextMessage — inline text message display widget for the MIPS custom control
// panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  textmessage.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "textmessage.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Constructor — stores parent widget, title, and position. Clears the message
// text.
// -----------------------------------------------------------------------------
TextMessage::TextMessage(QWidget *parent, QString name, int x, int y)
    : QWidget(parent)
{
    p     = parent;
    Title = name;
    X     = x;
    Y     = y;
    MessageText.clear();
}

// Show — creates the title label and message label; installs an event filter
// on the title label so the widget can be dragged to a new position.
void TextMessage::Show(void)
{
    Message = new QLabel(p);
    Message->setGeometry(X, Y, 1, 1);

    label = new QLabel(Title, p);
    label->setGeometry(X, Y, 1, 1);
    label->adjustSize();

    TL = new QLabel(p);
    TL->setGeometry(X, Y, 1, 1);

    label->installEventFilter(this);
    label->setMouseTracking(true);
}

// eventFilter — delegates mouse drag events to moveWidget() so the widget
// can be repositioned interactively on the control panel background.
bool TextMessage::eventFilter(QObject *obj, QEvent *event)
{
    if (moveWidget(obj, label, label, event)) return true;
    return QObject::eventFilter(obj, event);
}

// ShowMessage — updates the message label text and repositions it immediately
// to the right of the title label.
void TextMessage::ShowMessage(void)
{
    Message->setText(MessageText);
    Message->adjustSize();
    Message->setGeometry(X + label->width(), Y, Message->width(), Message->height());
}

// ProcessCommand — reads or sets the message text.
// Returns "?" if the command does not match this widget.
//
// Supported commands:
//   title     — returns the current message text
//   title=txt — sets the message text and refreshes the display
QString TextMessage::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (!cmd.startsWith(res)) return "?";
    if (cmd.trimmed() == res) return MessageText;
    QStringList resList = cmd.split("=");
    if (resList.count() == 2)
    {
        if (res == resList[0].trimmed())
        {
            MessageText = resList[1];
            ShowMessage();
            return "";
        }
    }
    return "?";
}
