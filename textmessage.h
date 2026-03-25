// =============================================================================
// textmessage.h
//
// TextMessage — inline text message display widget for the MIPS custom control
// panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef TEXTMESSAGE_H
#define TEXTMESSAGE_H

#include <QWidget>
#include <QLabel>

// -----------------------------------------------------------------------------
// TextMessage
//
// Displays a title label and a message label side by side on the control panel.
// The message text is set via ProcessCommand(). The widget can be dragged to a
// new position on the control panel background.
// -----------------------------------------------------------------------------
class TextMessage : public QWidget
{
    Q_OBJECT
public:
    TextMessage(QWidget *parent, QString name, int x, int y);
    void Show(void);
    void ShowMessage(void);
    QString ProcessCommand(QString cmd);

    QWidget *p;
    QString  Title;
    QString  MessageText;
    int      X, Y;
    QLabel  *label;

private:
    QLabel *TL;
    QLabel *Message;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // TEXTMESSAGE_H
