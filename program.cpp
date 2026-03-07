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
void Program::executeProgrammerCommand(QString cmd)
{
    console->clear();

    if(!comms->serial->isOpen())
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

    // Trigger SAM bootloader: close port, reopen at 1200 baud with DTR low, close again
    console->putData("MIPS bootloader enabled!\n");
    comms->closeSerialPort();
    QThread::sleep(1);
    while(comms->serial->isOpen()) QApplication::processEvents();
    QThread::sleep(1);
    comms->serial->setBaudRate(QSerialPort::Baud1200);
    QApplication::processEvents();
    comms->serial->open(QIODevice::ReadWrite);
    comms->serial->setDataTerminalReady(false);
    QThread::sleep(1);
    comms->serial->close();
    QApplication::processEvents();
    QThread::sleep(5);

    // Launch bossac
    QApplication::processEvents();
    console->putData(cmd.toStdString().c_str());
    console->putData("\n");
    QApplication::processEvents();

    QStringList arguments;
    arguments << "-c" << cmd;
#if defined(Q_OS_MAC)
    process.start("/bin/bash", arguments);
#else
    process.start(cmd);
#endif
    console->putData("Operation should start soon...\n");
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessOutput()));
    connect(&process, SIGNAL(readyReadStandardError()),  this, SLOT(readProcessOutput()));
}

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

    QString cmd = appPath + "/bossac -b --port=" + comms->serial->portName() + " -R";
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

    QString cmd = appPath + "/bossac -r -b --port=" + comms->serial->portName() + " " + fileName + " -R";
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

    QString cmd = appPath + "/bossac -e -w -v -b " + fileName + " -R";
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

    QString cmd = appPath + "/bossac -e -w -v -b --offset=0x4000 --port="
                + comms->serial->portName() + " " + fileName + " -R";
    executeProgrammerCommand(cmd);
}

// readProcessOutput — forwards bossac stdout and stderr to the console.
void Program::readProcessOutput(void)
{
    console->putData(process.readAllStandardOutput());
    console->putData(process.readAllStandardError());
}
