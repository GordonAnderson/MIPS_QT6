#include "help.h"
#include <ui_help.h>
#include <QTextOption>
#include <QDir>

Help::Help(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Help)
{
    ui->setupUi(this);

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

Help::~Help()
{
    delete ui;
}

QString Help::getText(void)
{
    return(ui->plainTextEdit->toPlainText());
}
void Help::setText(QString comment)
{
    ui->plainTextEdit->setPlainText(comment);
}
void Help::appendText(QString comment)
{
    ui->plainTextEdit->appendPlainText(comment);
}


void Help::SetTitle(QString title)
{
    QWidget::setWindowTitle (title);
}

void Help::LoadStr(QString DisplayText)
{
    ui->plainTextEdit->setPlainText(DisplayText);
}

void Help::LoadHelpText(QString FileName)
{
    ui->plainTextEdit->clear();
    ui->plainTextEdit->setWordWrapMode(QTextOption::NoWrap);
    if(FileName == "") return;
    QFile file(FileName);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        // We're going to streaming the file
        // to the QString
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            ui->plainTextEdit->appendPlainText(line);
        } while(!line.isNull());
        file.close();
    }
    ui->plainTextEdit->moveCursor (QTextCursor::Start);
}

void Help::resizeEvent(QResizeEvent*)
{
   ui->plainTextEdit->setFixedWidth(Help::width());
   ui->plainTextEdit->setFixedHeight(Help::height()); //-(ui->statusBar->height()));
}
