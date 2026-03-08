// =============================================================================
// fileops.cpp
//
// MIPS file operations — SD-card file transfer, EEPROM read/write, ARB FLASH
// read/write/upload. Extracted from mips.cpp during Phase 3 refactoring.
//
// Depends on:  mips.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "mips.h"
#include "ui_mips.h"
#include "comms.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

/*! \brief MIPS::GetFileFromMIPS
 * Prompts for an SD-card file name and local save path, then downloads the file from MIPS.
 */
void MIPS::GetFileFromMIPS(void)
{
    bool ok;

    // Get file name from user
    QString text = QInputDialog::getText(0, "MIPS", "File name to copy from MIPS SD card:", QLineEdit::Normal,"", &ok );
    if ( ok && !text.isEmpty() )
    {
        // Let user pick detination location and file name
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save to File"),"",tr("All files (*.*)"));
        if(fileName != "")
        {
            // If on terminal tab then turn off the slot for received chars
            if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal") disconnect(comms, &Comms::DataReady, nullptr, nullptr);
            comms->GetMIPSfile(text,fileName);
            if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal")
            {
                connect(comms, &Comms::DataReady, this, &MIPS::readData2Console);
                readData2Console();
            }
        }
     }
}

/*! \brief MIPS::PutFiletoMIPS
 * Prompts for a local file and SD-card destination name, then uploads the file to MIPS.
 */
void MIPS::PutFiletoMIPS(void)
{
    bool ok;

    // Let user pick source location and file name
    QString fileName = QFileDialog::getOpenFileName(this, tr("File to MIPS"),tr("All files (*)", 0));
    if(fileName == "") return;
    // If on terminal tab then turn off the slot for received chars
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal") disconnect(comms, &Comms::DataReady, nullptr, nullptr);
    // Get file name from user
    QString text = QInputDialog::getText(0, "MIPS", "File name to copy to MIPS SD card:", QLineEdit::Normal,"", &ok );
    if ( ok && !text.isEmpty() )
    {
        comms->PutMIPSfile(text,fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal")
    {
        connect(comms, &Comms::DataReady, this, &MIPS::readData2Console);
        readData2Console();
    }
}

/*! \brief MIPS::ReadEEPROM
 * Reads the EEPROM of a selected module board (A or B) and saves the data to a local file.
 */
void MIPS::ReadEEPROM(void)
{
    bool ok;

    QMessageBox msgbox;
    msgbox.setText("MIPS Module EEPROM read function");
    msgbox.setInformativeText("Select the module A or B:");
    msgbox.addButton(tr("B"), QMessageBox::NoRole);
    msgbox.addButton(tr("A"), QMessageBox::NoRole);
    msgbox.addButton(tr("Cancel"), QMessageBox::NoRole);
    int brd = msgbox.exec();
    if(brd == 2) return;
    QString Address = QInputDialog::getText(this, "Read MIPS EEPROM", "Enter EEPROM address in hex:", QLineEdit::Normal,QString(), &ok);
    if(ok && !Address.isEmpty())
    {
        // Here with the EEPROM module address and physical address
        // Selet a file to save the EEPROM contents
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save to File"),"",tr("All files (*.*)"));
        if(fileName != "")
        {
            // Read the data from MIPS
            if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal") disconnect(comms, &Comms::DataReady, nullptr, nullptr);
            if(brd == 1) comms->GetEEPROM(fileName,"A",Address.toInt(0,16));
            else comms->GetEEPROM(fileName,"B",Address.toInt(0,16));
            if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal")
            {
                connect(comms, &Comms::DataReady, this, &MIPS::readData2Console);
                readData2Console();
            }
        }
    }
}

/*! \brief MIPS::WriteEEPROM
 * Writes a local file to the EEPROM of a selected module board (A or B).
 */
void MIPS::WriteEEPROM(void)
{
    bool ok;

    QMessageBox msgbox;
    msgbox.setText("MIPS Module EEPROM write function. This function will allow you to reprogram the configuration memory of the selected module. Proceed with caution, you can render your system inoperable by entering invalid information.");
    msgbox.setInformativeText("Select the module A or B or cancel:");
    msgbox.addButton(tr("B"), QMessageBox::NoRole);
    msgbox.addButton(tr("A"), QMessageBox::NoRole);
    msgbox.addButton(tr("Cancel"), QMessageBox::NoRole);
    int brd = msgbox.exec();
    if(brd == 2) return;
    QString Address = QInputDialog::getText(this, "Write MIPS EEPROM", "Enter EEPROM address in hex:", QLineEdit::Normal,QString(), &ok);
    if(ok && !Address.isEmpty())
    {
        // Here with the EEPROM module address and physical address
        // Selet a file to write to the EEPROM
        QString fileName = QFileDialog::getOpenFileName(this, tr("File to MIPS"),tr("All files (*)", 0));
        if(fileName != "")
        {
            // Read the data from MIPS
            if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal") disconnect(comms, &Comms::DataReady, nullptr, nullptr);
            if(brd == 1) comms->PutEEPROM(fileName,"A",Address.toInt(0,16));
            else comms->PutEEPROM(fileName,"B",Address.toInt(0,16));
            if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal")
            {
                connect(comms, &Comms::DataReady, this, &MIPS::readData2Console);
                readData2Console();
            }
        }
    }
}

/*! \brief MIPS::ReadARBFLASH
 * Reads the ARB module FLASH memory and saves it to a local file.
 */
void MIPS::ReadARBFLASH(void)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save to File"),"",tr("All files (*.*)"));
    if(fileName != "")
    {
        // Read the data from MIPS
        if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal") disconnect(comms, &Comms::DataReady, nullptr, nullptr);
        comms->GetARBFLASH(fileName);
        if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal")
        {
            connect(comms, &Comms::DataReady, this, &MIPS::readData2Console);
            readData2Console();
        }
    }
}

/*! \brief MIPS::WriteARBFLASH
 * Writes a local file to the ARB module FLASH memory.
 */
void MIPS::WriteARBFLASH(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("File to MIPS"),tr("All files (*)", 0));
    if(fileName != "")
    {
        // Read the data from MIPS
        if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal") disconnect(comms, &Comms::DataReady, nullptr, nullptr);
        comms->PutARBFLASH(fileName);
        if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal")
        {
            connect(comms, &Comms::DataReady, this, &MIPS::readData2Console);
            readData2Console();
        }
    }
}

/*! \brief MIPS::ARBupload
 * Uploads a local file to the ARB module FLASH at a user-specified hex address.
 */
void MIPS::ARBupload(void)
{
    bool ok;

    QString Faddress = QInputDialog::getText(0, "ARB upload", "ARB Module FLASH write function. This function will allow\nyou to upload a file and place it in FLASH at the address\nyou select. Proceed with caution, you can render your\nsystem inoperable by entering invalid information.\n\nEnter FLASH address in hex or cancel:\nmover.bin at c0000\narb.bin at d0000", QLineEdit::Normal,"", &ok );
    if(ok)
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("File to upload to ARB FLASH"),tr("All files (*)", 0));
        if(fileName != "")
        {
            if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal") disconnect(comms, &Comms::DataReady, nullptr, nullptr);
            comms->ARBupload(Faddress, fileName);
            if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal")
            {
                connect(comms, &Comms::DataReady, this, &MIPS::readData2Console);
                readData2Console();
            }
        }
    }
}
