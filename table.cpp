// =============================================================================
// table.cpp
//
// Table — editable QTableWidget wrapper for the MIPS custom control panel.
// Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  table.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "table.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Table — implements a table widget with editable cells and configurable
// column headers. Provides ProcessCommand(), Report(), and SetValues() for
// integration with the MIPS control panel scripting system.
// -----------------------------------------------------------------------------

// Constructor — stores parent widget, title, dimensions, and position.
Table::Table(QWidget *parent, QString name, int rows, int columns,
             int width, int height, int x, int y)
    : QWidget(parent)
{
    p       = parent;
    Title   = name;
    X       = x;
    Y       = y;
    Rows    = rows;
    Columns = columns;
    Width   = width;
    Height  = height;
}

// Show — creates the QTableWidget, initialises default column headers and
// empty editable cells, and connects the cellChanged signal.
void Table::Show(void)
{
    QTable = new QTableWidget(Rows, Columns, p);
    if (QTable == nullptr) return;
    QTable->setGeometry(X, Y, Width, Height);
    QTable->setColumnCount(Columns);

    for (int c = 0; c < Columns; c++)
    {
        QTableWidgetItem *item = new QTableWidgetItem;
        if      (c == 0) item->setText("Time, mS");
        else if (c == 1) item->setText("Voltage");
        else             item->setText("");
        QTable->setHorizontalHeaderItem(c, item);
        QTable->setColumnWidth(c, 65);
    }
    for (int r = 0; r < Rows; r++)
    {
        QTable->setRowHeight(r, 20);
        for (int c = 0; c < Columns; c++)
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            item->setText("");
            QTable->setItem(r, c, item);
        }
    }
    QString res;
    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    QTable->setToolTip(res);
    QTable->show();
    connect(QTable, &QTableWidget::cellChanged, this, &Table::tableChange);
}

// eventFilter — delegates mouse drag events to moveWidget() so the table
// can be repositioned interactively on the control panel background.
bool Table::eventFilter(QObject *obj, QEvent *event)
{
    if (moveWidget(obj, QTable, QTable, event)) return true;
    return QObject::eventFilter(obj, event);
}

// ProcessCommand — reads or writes table cells, column headers, row/column
// counts, sort order, and scripting properties.
// Returns "?" if the command does not match this table.
//
// Supported commands:
//   title.Rows               — returns the row count
//   title.Columns            — returns the column count
//   title.Header,col         — returns the column header text
//   title.Header,col=text    — sets the column header text
//   title.Cell,row,col       — returns the cell text
//   title.Cell,row,col=text  — sets the cell text
//   title.sort               — sorts the table by column 0 ascending
//   title.script=name        — assigns a script to the change event
//   title.scriptCall=name    — assigns a script call string
QString Table::ProcessCommand(QString cmd)
{
    QString res;

    if (QTable == nullptr) return "?";
    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (!cmd.startsWith(res)) return "?";
    QStringList cmdList = cmd.split("=");
    QStringList args    = cmdList[0].split(",");
    if (cmdList.count() == 1)
    {
        if (cmdList[0] == res + ".sort")
        {
            QTable->sortItems(0, Qt::AscendingOrder);
            return "";
        }
    }
    if (cmdList.count() == 2)
    {
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
    }
    if (args.count() >= 1)
    {
        if ((args[0] == res + ".Rows") && (args.count() == 1) && (cmdList.count() == 1))
            return QString::number(Rows);
        if ((args[0] == res + ".Columns") && (args.count() == 1) && (cmdList.count() == 1))
            return QString::number(Columns);
        if (args.count() == 2)
        {
            if (args[0] == res + ".Header")
            {
                if (args[1].toInt() < 0 || args[1].toInt() >= Columns) return "?";
                if (cmdList.count() == 1) return QTable->horizontalHeaderItem(args[1].toInt())->text();
                if (cmdList.count() == 2)
                {
                    QTableWidgetItem *item = new QTableWidgetItem(cmdList[1]);
                    QTable->setHorizontalHeaderItem(args[1].toInt(), item);
                    return "";
                }
            }
        }
        if (args.count() == 3)
        {
            if (args[0] == res + ".Cell")
            {
                if (args[1].toInt() < 0 || args[1].toInt() >= Rows)    return "?";
                if (args[2].toInt() < 0 || args[2].toInt() >= Columns) return "?";
                if (cmdList.count() == 1) return QTable->item(args[1].toInt(), args[2].toInt())->text();
                if (cmdList.count() == 2)
                {
                    QTableWidgetItem *item = new QTableWidgetItem(cmdList[1]);
                    item->setFlags(item->flags() | Qt::ItemIsEditable);
                    QTable->setItem(args[1].toInt(), args[2].toInt(), item);
                    return "";
                }
            }
        }
    }
    return "?";
}

// Report — returns a multi-line string containing the full table state:
// row count, column count, all column headers, and all cell values.
// Each line is newline-terminated and suitable for SetValues() on reload.
//
// This function will return a string with multiple newline-terminated lines
// containing the table data: number of rows, number of columns, column
// titles, and cell data.
QString Table::Report(void)
{
    QString res, rpt = "";

    if (QTable == nullptr) return "";
    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    rpt += res + ".Rows,"    + QString::number(Rows)    + "\n";
    rpt += res + ".Columns," + QString::number(Columns) + "\n";
    for (int i = 0; i < Columns; i++)
    {
        rpt += res + ".Header," + QString::number(i) + "," +
               QTable->horizontalHeaderItem(i)->text() + "\n";
    }
    for (int r = 0; r < Rows; r++)
    {
        for (int c = 0; c < Columns; c++)
        {
            if (QTable->item(r, c) != nullptr)
            {
                rpt += res + ".Cell," + QString::number(r) + "," +
                       QString::number(c) + "," + QTable->item(r, c)->text() + "\n";
            }
        }
    }
    return rpt;
}

// SetValues — restores one line of table state saved by Report(). Handles
// row count, column count, column headers, cell values, and script assignment.
// Returns true if the line was recognised and applied, false otherwise.
//
// This function processes a string containing one line saved by Report().
bool Table::SetValues(QString strVals)
{
    QStringList resList;
    QString res;

    res.clear();
    if (p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if (!strVals.startsWith(res)) return false;
    resList = strVals.split(",");
    if (resList[0] == res + ".Rows")
    {
        if (resList.count() != 2) return false;
        int newRows = resList[1].toInt();
        if (newRows < 0 || newRows > 1000) return false;
        QTable->setRowCount(newRows);
        Rows = newRows;
        return true;
    }
    if (resList[0] == res + ".Columns")
    {
        if (resList.count() != 2) return false;
        int newCols = resList[1].toInt();
        if (newCols < 0 || newCols > 100) return false;
        QTable->setColumnCount(newCols);
        Columns = newCols;
        return true;
    }
    if (resList[0] == res + ".Header")
    {
        if (resList.count() >= 5) return false;
        int colIndex = resList[1].toInt();
        if (colIndex < 0 || colIndex >= Columns) return false;
        QString strHdr = "";
        for (int i = 2; i < resList.count(); i++)
        {
            if (i > 2) strHdr += ",";
            strHdr += resList[i];
        }
        QTableWidgetItem *item = new QTableWidgetItem(strHdr);
        QTable->setHorizontalHeaderItem(colIndex, item);
        return true;
    }
    if (resList[0] == res + ".Cell")
    {
        if (resList.count() != 4) return false;
        int rowIndex = resList[1].toInt();
        int colIndex = resList[2].toInt();
        if (rowIndex < 0 || rowIndex >= Rows || colIndex < 0 || colIndex >= Columns) return false;
        QTableWidgetItem *item = new QTableWidgetItem(resList[3]);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        QTable->setItem(rowIndex, colIndex, item);
        return true;
    }
    if ((resList.count() == 2) && (resList[0] == res + ".script"))
    {
        scriptName = resList[1].trimmed();
        return true;
    }
    return false;
}

// tableChange — emits change(scriptName) when a cell is edited, if a script
// is assigned to this table.
void Table::tableChange(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!scriptName.isEmpty()) emit change(scriptName);
}
