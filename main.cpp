#include "mips.h"
#include <QApplication>
#include <QWidget>
#include <signal.h>
#include <iostream>


void signalHandler(int signal)
{
    std::cerr << "Caught signal: " << signal << std::endl;
    // Perform cleanup or generate a crash report here
    exit(signal);
}

int main(int argc, char *argv[])
{
    QString cpf;

    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGFPE,  signalHandler);
    signal(SIGILL,  signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGINT,  signalHandler);
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
    QObject::connect(&a, SIGNAL(focusChanged(QWidget*,QWidget*)), &w, SLOT(setWidgets(QWidget*,QWidget*)));
    w.show();

    return a.exec();
}
