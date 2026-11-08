// =============================================================================
// gscrunner.h
//
// GscRunner -- compiles GAACE_Script source by running the one compiler,
// tools/gsc.py (https://github.com/GordonAnderson/GAACE_Script), as a separate
// process: `python gsc.py - --json --syscalls T --commands C` with the source on
// stdin. The syscall table and command list are the connected device's own, so
// there is no syscall file for anyone to manage (see MIPS_HOST_APP_INTEGRATION.md).
//
// Finding Python and gsc.py (first match wins):
//   Python: the bundled runtime (python\python.exe next to the executable on
//           Windows; Contents/Resources/python/bin/python3 in the macOS app),
//           then python3 / python on the PATH and in the usual install folders
//           (a development fallback).
//   gsc.py: $GSC_PATH, then the copy the build puts next to the executable
//           (Windows) or in Contents/Resources (macOS), then the source tree's
//           tools/gsc/ when this was built with GSC_SOURCE_DIR.
//
// Calls are synchronous (a compile takes well under a second), like the rest
// of the app's device I/O, and run on the GUI thread.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     November 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef GSCRUNNER_H
#define GSCRUNNER_H

#include <QByteArray>
#include <QString>

#include "devicescriptproto.h"

class GscRunner
{
public:
    //! The oldest gsc.py this app works with (--json with warnings).
    static const int kMinMajor = 1, kMinMinor = 2, kMinPatch = 0;

    //! Path of the Python interpreter to use, or an empty string.
    static QString findPython();

    //! Path of gsc.py, or an empty string.
    static QString findGsc();

    //! A one-line description of what will be run, for the log.
    static QString describeSetup();

    //! "gsc 1.2.0, Python 3.14.10" (what is actually installed, found by asking
    //! them), or a short note saying what is missing. Runs two quick processes.
    static QString versionText();

    //! Compiles `source`. `syscallJson` / `commandsJson` are the contents of
    //! the files gsc.py's --syscalls / --commands read (see
    //! DeviceScriptProto::syscallTableJson / commandListJson); empty means
    //! "don't pass the option" (the script must then declare its syscalls, and
    //! command names are not checked). On any failure to run the compiler,
    //! result.failure says what went wrong and what to do about it.
    static DeviceScriptProto::CompileResult compile(const QString &source,
                                                    const QByteArray &syscallJson = QByteArray(),
                                                    const QByteArray &commandsJson = QByteArray(),
                                                    int timeoutMs = 15000);
};

#endif // GSCRUNNER_H
