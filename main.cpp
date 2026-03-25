// =============================================================================
// main.cpp
//
// Application entry point. Installs signal handlers for crash reporting,
// creates the QApplication and main MIPS window, applies platform-specific
// style, and starts the event loop.
//
// An optional command-line argument is passed to the MIPS constructor as the
// initial control panel file path.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "mips.h"
#include <QApplication>
#include <QWidget>
#include <signal.h>
#include <iostream>

// signalHandler — catches fatal signals, prints the signal number to stderr,
// and exits. Provides a hook for crash reporting or cleanup if needed.
void signalHandler(int signal)
{
    std::cerr << "Caught signal: " << signal << std::endl;
    exit(signal);
}

// main — application entry point. Installs signal handlers, constructs the
// QApplication and MIPS main window, optionally passing argv[1] as the initial
// control panel file path, then starts the Qt event loop.
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
    else          cpf = "";

    MIPS w(0, cpf);
    w.setWindowIcon(QIcon(":/GAACElogo.ico"));

#if defined(Q_OS_WIN)
    QApplication::setStyle("WindowsVista");
#endif

    QObject::connect(&a, SIGNAL(focusChanged(QWidget*,QWidget*)), &w, SLOT(setWidgets(QWidget*,QWidget*)));
    w.show();
    return a.exec();
}
