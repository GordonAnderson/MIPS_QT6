// =============================================================================
// codeeditor.cpp
//
// CodeEditor. See codeeditor.h.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     November 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "codeeditor.h"

#include <QFontDatabase>
#include <QPainter>
#include <QTextBlock>

namespace
{

// The gutter widget: it only forwards sizing and painting to its editor.
class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor) : QWidget(editor), editor_(editor) {}
    QSize sizeHint() const override { return QSize(editor_->lineNumberAreaWidth(), 0); }

protected:
    void paintEvent(QPaintEvent *event) override { editor_->lineNumberAreaPaintEvent(event); }

private:
    CodeEditor *editor_;
};

} // namespace

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea_ = new LineNumberArea(this);

    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(f.pointSize() + 1);
    setFont(f);
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setTabStopDistance(4 * fontMetrics().horizontalAdvance(' '));

    connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::refreshHighlights);
    updateLineNumberAreaWidth();
    refreshHighlights();
}

int CodeEditor::lineNumberAreaWidth() const
{
    int digits = 1;
    for(int max = qMax(1, blockCount()); max >= 10; max /= 10) digits++;
    return 8 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * qMax(digits, 3);
}

void CodeEditor::updateLineNumberAreaWidth()
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if(dy) lineNumberArea_->scroll(0, dy);
    else   lineNumberArea_->update(0, rect.y(), lineNumberArea_->width(), rect.height());
    if(rect.contains(viewport()->rect())) updateLineNumberAreaWidth();
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    const QRect cr = contentsRect();
    lineNumberArea_->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea_);
    painter.fillRect(event->rect(), palette().alternateBase());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while(block.isValid() && top <= event->rect().bottom())
    {
        if(block.isVisible() && bottom >= event->rect().top())
        {
            const bool problem = (blockNumber + 1 == problemLine_);
            painter.setPen(problem ? QColor(Qt::red) : palette().color(QPalette::Mid));
            QFont f = painter.font();
            f.setBold(problem);
            painter.setFont(f);
            painter.drawText(0, top, lineNumberArea_->width() - 4, fontMetrics().height(),
                             Qt::AlignRight, QString::number(blockNumber + 1));
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

// The current line is shaded lightly; the problem line, if any, in red.
void CodeEditor::refreshHighlights()
{
    QList<QTextEdit::ExtraSelection> selections;

    if(!isReadOnly())
    {
        QTextEdit::ExtraSelection current;
        current.format.setBackground(palette().alternateBase());
        current.format.setProperty(QTextFormat::FullWidthSelection, true);
        current.cursor = textCursor();
        current.cursor.clearSelection();
        selections.append(current);
    }
    if(problemLine_ > 0)
    {
        QTextBlock block = document()->findBlockByNumber(problemLine_ - 1);
        if(block.isValid())
        {
            QTextEdit::ExtraSelection problem;
            problem.format.setBackground(QColor(255, 120, 120, 90));
            problem.format.setProperty(QTextFormat::FullWidthSelection, true);
            problem.cursor = QTextCursor(block);
            selections.append(problem);
        }
    }
    setExtraSelections(selections);
}

void CodeEditor::showProblem(int line, int column)
{
    problemLine_ = qMax(0, line);
    if(problemLine_ > 0)
    {
        QTextBlock block = document()->findBlockByNumber(problemLine_ - 1);
        if(block.isValid())
        {
            QTextCursor c(block);
            c.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor,
                           qBound(0, column - 1, qMax(0, block.length() - 1)));
            setTextCursor(c);
            centerCursor();
        }
    }
    lineNumberArea_->update();
    refreshHighlights();
}

void CodeEditor::clearProblem()
{
    if(problemLine_ == 0) return;
    problemLine_ = 0;
    lineNumberArea_->update();
    refreshHighlights();
}
