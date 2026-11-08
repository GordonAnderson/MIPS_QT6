// =============================================================================
// codeeditor.h
//
// CodeEditor -- a plain-text editor with a line-number gutter and a
// highlighted "problem line", used by the Device Scripts dialog for GAACE_Script
// source. Adapted from Qt's Code Editor example.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     November 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    int  lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    //! Marks `line` (1-based) as the problem line and moves the cursor to
    //! `line`, `column` (1-based; 0 = start of line). Line 0 clears the mark.
    void showProblem(int line, int column = 0);
    void clearProblem();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect &rect, int dy);
    void refreshHighlights();

private:
    QWidget *lineNumberArea_;
    int      problemLine_ = 0;
};

#endif // CODEEDITOR_H
