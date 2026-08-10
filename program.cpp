// =============================================================================
// program.cpp
//
// Implements the Program class — firmware upload/download for MIPS and RFmega
// using the bossac programmer tool bundled with the application.
//
// All operations follow the same sequence:
//   1. User confirms via a warning dialog
//   2. Serial port is closed, briefly reopened at 1200 baud with DTR low to
//      trigger the Arduino SAM bootloader, then closed again
//   3. bossac is invoked via QProcess with the appropriate flags
//   4. stdout/stderr from bossac is streamed to the Console terminal tab
//
// bossac must be present in the application directory (bossac on macOS,
// bossac.exe on Windows). If it cannot be found the operation is aborted.
//
// Supported actions (wired to main menu actions):
//   programMIPS()         — erase + write new MIPS firmware binary
//   saveMIPSfirmware()    — read current MIPS firmware to a .bin file
//   setBootloaderBootBit()— set the SAM bootloader boot flag (recovery only)
//   programRFmega()       — erase + write new RFmega firmware binary
//
// Depends on:  ui_mips__.h, comms.h, console.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "program.h"

// Program — constructor. Stores UI, Comms, and Console references, resolves
// the application directory for finding bossac, and connects the four firmware
// action menu items to their respective slots.
Program::Program(Ui::MIPS *w, Comms *c, Console *con)
{
    pui     = w;
    comms   = c;
    console = con;
    appPath = QApplication::applicationDirPath();

    connect(pui->actionProgram_MIPS,              SIGNAL(triggered()), this, SLOT(programMIPS()));
    connect(pui->actionSave_current_MIPS_firmware,SIGNAL(triggered()), this, SLOT(saveMIPSfirmware()));
    connect(pui->actionSet_bootloader_boot_flag,  SIGNAL(triggered()), this, SLOT(setBootloaderBootBit()));
    connect(pui->actionProgram_RFmega,            SIGNAL(triggered()), this, SLOT(programRFmega()));
}

// executeProgrammerCommand — common entry point for all bossac operations.
// Triggers the SAM bootloader via a 1200-baud DTR-low pulse, then launches
// bossac via QProcess with the provided command string.
//
// The bootloader reset commonly causes the board to re-enumerate under a
// *different* serial port (reliably on Windows, intermittently on macOS), so
// the caller must not bake a --port= value into cmd. Instead cmd should
// contain the literal placeholder "%PORT%" where the port name belongs; this
// function fills it in with whatever port the board actually comes back on,
// discovered by diffing the available-ports list before and after the reset.
void Program::executeProgrammerCommand(QString cmd)
{
    console->clear();

    if(!comms->serialPort()->isOpen())
    {
        console->putData("This application is not connected to MIPS!\n");
        return;
    }

    // Verify that bossac is present in the application directory
#if defined(Q_OS_MAC)
    QString pcheck = appPath + "/bossac";
#else
    QString pcheck = appPath + "/bossac.exe";
#endif
    QFileInfo checkFile(pcheck);
    if(!checkFile.exists() || !checkFile.isFile())
    {
        console->putData("Can't find the programmer!\n");
        console->putData(cmd.toStdString().c_str());
        return;
    }

    // Snapshot the port we're on and the full set of available ports. The
    // bootloader reset below typically makes the board re-enumerate under a
    // new port name, so the pre-reset name can't be trusted after the reset.
    QString originalPort = comms->serialPort()->portName();
    QStringList portsBefore;
    for(const QSerialPortInfo &info : QSerialPortInfo::availablePorts())
        portsBefore << info.portName();

    // Trigger SAM bootloader: close port, reopen at 1200 baud with DTR low, close again
    console->putData("MIPS bootloader enabled!\n");
    comms->closeSerialPort();
    QThread::sleep(1);
    while(comms->serialPort()->isOpen()) QApplication::processEvents();
    QThread::sleep(1);
    comms->serialPort()->setBaudRate(QSerialPort::Baud1200);
    QApplication::processEvents();
    comms->serialPort()->open(QIODevice::ReadWrite);
    comms->serialPort()->setDataTerminalReady(false);
    QThread::sleep(1);
    comms->serialPort()->close();
    QApplication::processEvents();

    // Wait for the board to reset into the bootloader and rediscover which
    // port it came back on. Poll rather than assuming a fixed delay is long
    // enough, and prefer a port that wasn't present before the reset (the
    // usual sign of a re-enumeration) over the original port name.
    QString bootPort = originalPort;
    bool foundNewPort = false;
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() < 10000 && !foundNewPort)
    {
        QApplication::processEvents();
        QThread::msleep(250);
        for(const QSerialPortInfo &info : QSerialPortInfo::availablePorts())
        {
            if(!portsBefore.contains(info.portName()))
            {
                bootPort = info.portName();
                foundNewPort = true;
                break;
            }
        }
    }
    // Give the OS a moment to finish settling the port before bossac opens it
    QThread::sleep(foundNewPort ? 1 : 3);

    cmd.replace("%PORT%", bootPort);

    // Launch bossac
    QApplication::processEvents();
    console->putData(cmd.toStdString().c_str());
    console->putData("\n");
    QApplication::processEvents();

    // Disconnect any connections left over from a previous run before
    // reconnecting, so repeated attempts don't stack duplicate output/errors.
    disconnect(&process, &QProcess::readyReadStandardOutput, this, &Program::readProcessOutput);
    disconnect(&process, &QProcess::readyReadStandardError,  this, &Program::readProcessOutput);
    disconnect(&process, &QProcess::errorOccurred,            this, &Program::processErrorOccurred);
    connect(&process, &QProcess::readyReadStandardOutput, this, &Program::readProcessOutput);
    connect(&process, &QProcess::readyReadStandardError,  this, &Program::readProcessOutput);
    connect(&process, &QProcess::errorOccurred,            this, &Program::processErrorOccurred);

#if defined(Q_OS_MAC)
    process.start("/bin/bash", QStringList() << "-c" << cmd);
#else
    // Don't hand the whole command line to the single-string start()
    // overload — it has to re-split and re-quote it into a Windows command
    // line internally, which is fragile. Split it ourselves and use the
    // explicit program+arguments overload so bossac.exe gets exactly the
    // argv a manually-typed command at a prompt would produce.
    QStringList tokens = QProcess::splitCommand(cmd);
    QString bossacProgram = tokens.isEmpty() ? cmd : tokens.takeFirst();
    process.start(bossacProgram, tokens);
#endif
    console->putData("Operation should start soon...\n");
}

// setBootloaderBootBit — sets the SAM bootloader boot flag via bossac -b.
// Intended as an error recovery tool; warns the user before proceeding.
void Program::setBootloaderBootBit(void)
{
    QMessageBox msgBox;
    QString msg = "This function will attemp to set the bootloader boot flag in the MIPS system. "
                  "This function is provided as part of an error recovery process and should not "
                  "normally be necessary. If the boot flag is set on an erased DUE the results "
                  "are unpredictable.\n";
    msgBox.setText(msg);
    msgBox.setInformativeText("Are you sure you want to contine?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::No) return;

    pui->tabMIPS->setCurrentIndex(1);

    msg = "Unplug any RF drive heads from MIPS before you proceed. This includes unplugging the "
          "FAIMS RF deck as well. It is assumed that you have already established communications "
          "with the MIPS system. If the connection is not establised this function will exit with "
          "no action.";
    msgBox.setText(msg);
    msgBox.setInformativeText("");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();

    QString cmd = appPath + "/bossac -b --port=%PORT% -R";
    executeProgrammerCommand(cmd);
}

// saveMIPSfirmware — reads the current MIPS firmware and saves it to a
// user-selected .bin file.
void Program::saveMIPSfirmware(void)
{
    QMessageBox msgBox;
    QString msg = "This will read the current MIPS firmware and save to a file. "
                  "You should save to a file with the .bin extension and indicate the current version.\n";
    msgBox.setText(msg);
    msgBox.setInformativeText("Are you sure you want to contine?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::No) return;

    pui->tabMIPS->setCurrentIndex(1);

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save MIPS firmware .bin file"), "", tr("Files (*.bin *.*)"));
    if(fileName.isEmpty()) return;

    msg = "Unplug any RF drive heads from MIPS before you proceed. This includes unplugging the "
          "FAIMS RF deck as well. It is assumed that you have already established communications "
          "with the MIPS system. If the connection is not establised this function will exit with "
          "no action.";
    msgBox.setText(msg);
    msgBox.setInformativeText("");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();

    QString cmd = appPath + "/bossac -r -b --port=%PORT% " + fileName + " -R";
    executeProgrammerCommand(cmd);
}

// programMIPS — erases and reprograms MIPS with a user-selected .bin file.
void Program::programMIPS(void)
{
    QMessageBox msgBox;
    QString msg = "This will erase the MIPS firmware and attempt to load a new version. "
                  "Make sure you have a new MIPS binary file to load, it should have a .bin extension.\n"
                  "The MIPS firmware will be erased so if your bin file is invalid or fails to "
                  "program, MIPS will be rendered useless!\n";
    msgBox.setText(msg);
    msgBox.setInformativeText("Are you sure you want to contine?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::No) return;

    pui->tabMIPS->setCurrentIndex(1);

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Load MIPS firmware .bin file"), "", tr("Files (*.bin *.*)"));
    if(fileName.isEmpty()) return;

    msg = "Unplug any RF drive heads from MIPS before you proceed. This includes unplugging the "
          "FAIMS RF deck as well. It is assumed that you have already established communications "
          "with the MIPS system. If the connection is not establised this function will exit with "
          "no action.";
    msgBox.setText(msg);
    msgBox.setInformativeText("");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();

    QString cmd = appPath + "/bossac -e -w -v -b --port=%PORT% " + fileName + " -R";
    executeProgrammerCommand(cmd);
}

// programRFmega — erases and reprograms the RFmega with a user-selected .bin
// file. Uses --offset=0x4000 to target the RFmega application region.
void Program::programRFmega(void)
{
    QMessageBox msgBox;
    QString msg = "This will erase the RFmega firmware and attempt to load a new version. "
                  "Make sure you have a new RFmega binary file to load, it should have a .bin extension.\n"
                  "The RFmega firmware will be erased so if your bin file is invalid or fails to "
                  "program, RFmega will be rendered useless!\n";
    msgBox.setText(msg);
    msgBox.setInformativeText("Are you sure you want to contine?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::No) return;

    pui->tabMIPS->setCurrentIndex(1);

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Load RFmega firmware .bin file"), "", tr("Files (*.bin *.*)"));
    if(fileName.isEmpty()) return;

    QString cmd = appPath + "/bossac -e -w -v -b --offset=0x4000 --port=%PORT% " + fileName + " -R";
    executeProgrammerCommand(cmd);
}

// readProcessOutput — forwards bossac stdout and stderr to the console.
void Program::readProcessOutput(void)
{
    console->putData(process.readAllStandardOutput());
    console->putData(process.readAllStandardError());
}

// processErrorOccurred — reports a QProcess launch/runtime failure to the
// console. Without this, a failed bossac launch (bad path, couldn't start,
// crashed, etc.) happened silently — the console would just stop updating
// right after the command line was printed, with no indication why.
void Program::processErrorOccurred(QProcess::ProcessError error)
{
    QString msg;
    switch(error)
    {
    case QProcess::FailedToStart:
        msg = "bossac failed to start (not found or not executable).\n";
        break;
    case QProcess::Crashed:
        msg = "bossac crashed.\n";
        break;
    case QProcess::Timedout:
        msg = "bossac timed out.\n";
        break;
    case QProcess::WriteError:
        msg = "Error writing to bossac.\n";
        break;
    case QProcess::ReadError:
        msg = "Error reading from bossac.\n";
        break;
    default:
        msg = "Unknown error running bossac.\n";
        break;
    }
    console->putData(msg.toStdString().c_str());
}
