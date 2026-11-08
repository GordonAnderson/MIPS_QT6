// =============================================================================
// gscrunner.cpp
//
// GscRunner. See gscrunner.h.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     November 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "gscrunner.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QTemporaryDir>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

using namespace DeviceScriptProto;

namespace
{

bool isExecutableFile(const QString &path)
{
    const QFileInfo fi(path);
    return fi.exists() && fi.isFile() && fi.isExecutable();
}

QString appDir()
{
    return QCoreApplication::applicationDirPath();
}

// python.exe is a console program; started from this GUI application it would
// flash a console window for every compile unless told not to.
void hideConsoleWindow(QProcess &process)
{
#ifdef Q_OS_WIN
    process.setCreateProcessArgumentsModifier(
        [](QProcess::CreateProcessArguments *args) { args->flags |= CREATE_NO_WINDOW; });
#else
    Q_UNUSED(process);
#endif
}

} // namespace

QString GscRunner::findPython()
{
    // The bundled runtime first: next to the executable on Windows; in
    // Contents/Resources on macOS (code signing does not accept it in
    // Contents/MacOS or Contents/Frameworks, see tools/python/README.md).
    const QStringList bundled = {
        appDir() + "/python/python.exe",
        appDir() + "/../Resources/python/bin/python3",
        appDir() + "/python/bin/python3",
    };
    for(const QString &p : bundled)
        if(isExecutableFile(p)) return p;

    // Development fallback: whatever Python is installed. A GUI app started
    // from the Finder has a minimal PATH, so look in the usual places too.
    const QStringList extraDirs = {
        "/opt/homebrew/bin", "/usr/local/bin",
        "/Library/Frameworks/Python.framework/Versions/Current/bin",
        "/usr/bin",
    };
    for(const char *name : { "python3", "python" })
    {
        QString p = QStandardPaths::findExecutable(name);
        if(p.isEmpty()) p = QStandardPaths::findExecutable(name, extraDirs);
        if(!p.isEmpty()) return p;
    }
    return QString();
}

QString GscRunner::findGsc()
{
    QStringList candidates;
    const QByteArray env = qgetenv("GSC_PATH");
    if(!env.isEmpty()) candidates << QString::fromLocal8Bit(env);
    candidates << appDir() + "/gsc.py" << appDir() + "/../Resources/gsc.py";
#ifdef GSC_SOURCE_DIR
    candidates << QString(GSC_SOURCE_DIR) + "/gsc.py";
#endif
    for(const QString &c : candidates)
        if(QFileInfo::exists(c)) return QDir::cleanPath(c);
    return QString();
}

QString GscRunner::describeSetup()
{
    const QString py = findPython(), gsc = findGsc();
    return QString("compiler: %1 %2").arg(py.isEmpty() ? "(no Python found)" : py,
                                          gsc.isEmpty() ? "(gsc.py not found)" : gsc);
}

// Runs `program args` and returns its trimmed stdout, or an empty string.
static QString runCapture(const QString &program, const QStringList &args, int timeoutMs = 5000)
{
    QProcess p;
    hideConsoleWindow(p);
    p.start(program, args);
    if(!p.waitForStarted(timeoutMs) || !p.waitForFinished(timeoutMs) ||
       p.exitStatus() != QProcess::NormalExit || p.exitCode() != 0)
        return QString();
    return QString::fromUtf8(p.readAllStandardOutput()).trimmed();
}

QString GscRunner::versionText()
{
    const QString py = findPython(), gsc = findGsc();
    if(py.isEmpty())  return "Python not found";
    if(gsc.isEmpty()) return "gsc.py not found";
    QString pyVersion = runCapture(py, { "-I", "-B", "--version" });   // "Python 3.14.10"
    pyVersion.remove("Python ");
    const QString gscVersion = runCapture(py, { "-I", "-B", gsc, "--version" });   // "gsc 1.2.0"
    return QString("%1, Python %2").arg(gscVersion.isEmpty() ? "gsc (version unknown)" : gscVersion,
                                        pyVersion.isEmpty() ? "(version unknown)" : pyVersion);
}

CompileResult GscRunner::compile(const QString &source, const QByteArray &syscallJson,
                                 const QByteArray &commandsJson, int timeoutMs)
{
    CompileResult result;
    const QString python = findPython();
    const QString gsc = findGsc();
    if(python.isEmpty())
    {
        result.failure = "Python was not found. The Device Scripts compiler needs Python 3 "
                         "(a copy is meant to ship with the application, in a \"python\" folder "
                         "next to it). Install Python 3, or restore that folder.";
        return result;
    }
    if(gsc.isEmpty())
    {
        result.failure = "gsc.py (the script compiler) was not found next to the application.";
        return result;
    }

    // The device's tables go to temporary files for gsc.py to read.
    QTemporaryDir dir;
    if(!dir.isValid())
    {
        result.failure = "Cannot create a temporary folder for the compiler.";
        return result;
    }
    // -I: ignore the user's Python environment; -B: never write .pyc files (the
    // runtime lives inside a signed application bundle, which must not change).
    QStringList args = { "-I", "-B", gsc, "-", "--json" };
    auto addFile = [&](const char *option, const char *name, const QByteArray &content)
    {
        if(content.isEmpty()) return true;
        QFile f(dir.filePath(name));
        if(!f.open(QIODevice::WriteOnly) || f.write(content) != content.size()) return false;
        f.close();
        args << option << f.fileName();
        return true;
    };
    if(!addFile("--syscalls", "syscalls.json", syscallJson) ||
       !addFile("--commands", "commands.json", commandsJson))
    {
        result.failure = "Cannot write the device's tables for the compiler.";
        return result;
    }

    QProcess process;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PYTHONIOENCODING", "utf-8");   // sources may contain non-ASCII comments
    env.insert("PYTHONUTF8", "1");
    env.insert("PYTHONDONTWRITEBYTECODE", "1");
    process.setProcessEnvironment(env);
    hideConsoleWindow(process);
    process.start(python, args);
    if(!process.waitForStarted(5000))
    {
        result.failure = QString("Could not start %1: %2").arg(python, process.errorString());
        return result;
    }
    process.write(source.toUtf8());
    process.closeWriteChannel();
    if(!process.waitForFinished(timeoutMs))
    {
        process.kill();
        process.waitForFinished(1000);
        result.failure = "The compiler did not finish in time and was stopped.";
        return result;
    }

    const QByteArray out = process.readAllStandardOutput();
    const QString errText = QString::fromUtf8(process.readAllStandardError()).trimmed();
    QString parseError;
    if(!parseGscJson(out, result, &parseError))
    {
        result = CompileResult();
        result.failure = "The compiler gave an unexpected answer (" + parseError + ")" +
                         (errText.isEmpty() ? QString() : ":\n" + errText);
        return result;
    }
    if(!versionAtLeast(result.gscVersion, kMinMajor, kMinMinor, kMinPatch))
    {
        const QString found = result.gscVersion;
        result = CompileResult();
        result.failure = QString("gsc.py is version %1; this application needs %2.%3.%4 or newer.")
                             .arg(found.isEmpty() ? "unknown" : found)
                             .arg(kMinMajor).arg(kMinMinor).arg(kMinPatch);
    }
    return result;
}
