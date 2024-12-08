#include "mips.h"
#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    QString cpf;

    QApplication a(argc, argv);
    if(argc >= 1) cpf = argv[1];
    else cpf = "";
    MIPS w(0,cpf);
    w.setWindowIcon(QIcon(":/GAACElogo.ico"));
    // Windows
    // WindowsXP
    // WindowsVista
    // Fusion
    // Macintosh
    #if defined(Q_OS_WIN)
    QApplication::setStyle("WindowsVista");
    #endif
    //connect(&a, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(setWidgets(QWidget*, QWidget*)));    connect(ui->actionClear, SIGNAL(triggered()), console, SLOT(clear()));
    QObject::connect(&a, SIGNAL(focusChanged(QWidget*,QWidget*)), &w, SLOT(setWidgets(QWidget*,QWidget*)));
    w.show();

    return a.exec();
}
