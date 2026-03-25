// =============================================================================
// table.h
//
// Table — editable QTableWidget wrapper for the MIPS custom control panel.
// Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef TABLE_H
#define TABLE_H

#include <QWidget>
#include <QTableWidget>

// -----------------------------------------------------------------------------
// Table
//
// Creates a QTableWidget with configurable rows, columns, and dimensions.
// Cell values and column headers can be read and written via ProcessCommand().
// Report() and SetValues() support save/restore of the full table state.
// Emits change(scriptName) when any cell is edited, if a script is assigned.
// The widget can be dragged to a new position on the control panel background.
// -----------------------------------------------------------------------------
class Table : public QWidget
{
    Q_OBJECT
signals:
    void change(QString scriptName);
public:
    Table(QWidget *parent, QString name, int rows, int columns,
          int width, int height, int x, int y);
    void    Show(void);
    QString Report(void);
    bool    SetValues(QString strVals);
    QString ProcessCommand(QString cmd);

    QWidget *p;
    QString  Title;
    int      Width, Height;
    int      X, Y;
    int      Rows, Columns;
    QString  scriptName = "";
    QString  scriptCall = "";

private:
    QTableWidget *QTable;

public slots:
    void tableChange(int row, int column);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // TABLE_H
