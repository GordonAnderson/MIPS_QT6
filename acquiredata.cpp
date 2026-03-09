// =============================================================================
// acquiredata.cpp
//
// AcquireData — drives an external command-line data-acquisition application.
// Extracted from timinggenerator.cpp during Phase 3 refactoring.
//
// Depends on:  timinggenerator.h, cmdlineapp.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "timinggenerator.h"
#include "ui_timinggenerator.h"
#include "Utilities.h"
#include "controlpanel.h"

/*! \brief AcquireData::AcquireData
 * Constructor. Initialises state flags and nulls the internal cmdlineapp pointer.
 */
AcquireData::AcquireData(QWidget *parent)
{
    p = parent;
    comms  = nullptr;
    cla = nullptr;
    Acquire = "";
    statusBar = nullptr;
    properties = nullptr;
    Acquiring = false;
    fileName = "U1084A.data";
    TriggerMode = "Software";
    LastFrameSize = 0;
    LastAccumulations = 0;
}

/*! \brief AcquireData::StartAcquire
 * Starts the external acquisition application with the given file path, frame size, and accumulation count.
 */
void AcquireData::StartAcquire(QString path, int FrameSize, int Accumulations)
{
    QString StatusMessage;

    StatusMessage.clear();
    // If the Acquire string is defined then we call the program defined
    // by The Acquire string. The program is called with optional arguments
    // as follows:
    // - Filename, this will result in a dialog box to appear
    //             allowing the user to select a filename to hold
    //             the data
    // - TOFscans, passes the total number of tof scans to acquire, this is
    //             the product of Frame size and accumulations
    // The acquire app is expected to return "Ready" when ready to accept a trigger.
    if(properties != nullptr) properties->Log("StartAcquire:" + path);
    filePath = "";
    // Make sure the system is in table mode
    QString TbkCmd = "SMOD,ONCE\n";
    if(TriggerMode != "Software") TbkCmd = "SMOD,TBL\n";
    if(comms == nullptr) StatusMessage += "No open comms channels!\n";
    else if(comms->SendCommand(TbkCmd))
    {
        if(!TableDownloaded)
        {
            QString msg = "Pulse sequence table has not been downloaded to MIPS!";
            QMessageBox msgBox;
            msgBox.setText(msg);
            msgBox.setInformativeText("");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
            return;
        }
        if(statusBar != nullptr) statusBar->showMessage("System mode changed to Table.", 5000);
        StatusMessage += "System mode changed to Table.\n";
    }
    else StatusMessage += "MIPS failed to enter table mode!\n";
    // Acquire contains the name and full path to the acquire command
    // line application. This is set in the controlpanel startup
    // function.
    if(Acquire != "")
    {
        QString cmd = "";
        QStringList resList = Acquire.split(",");
        cmd = Acquire;
        for(int i=0;i<resList.count();i++)
        {
            if(i==0) cmd = resList[0];
            if(resList[i].toUpper() == "ACCUMULATIONS")
            {
                cmd += " -A" + QString::number(Accumulations);
            }
            if(resList[i].toUpper() == "TOFSCANS")
            {
                cmd += " -S" + QString::number(FrameSize * Accumulations);
            }
            if(resList[i].toUpper() == "FILENAME")
            {
                // If the path is undefined popup and folder selection dialog
                while(path == "")
                {
                    CDirSelectionDlg *cds = new CDirSelectionDlg(QDir::currentPath(),p);
                    cds->setTitle("Select/enter folder to save data files");
                    cds->show();
                    while(cds->isVisible()) QApplication::processEvents();
                    if(cds->result() != 0)
                    {
                        QString selectedPath = cds->selectedPath();
                        delete cds;
                        // See if the directory is present
                        if(QDir(selectedPath).exists())
                        {
                            QString msg = "Selected folder exists, please define a unique folder to save data files.";
                            QMessageBox msgBox;
                            msgBox.setText(msg);
                            msgBox.setInformativeText("");
                            msgBox.setStandardButtons(QMessageBox::Ok);
                            msgBox.exec();
                        }
                        else
                        {
                            filePath = selectedPath;
                            break;
                        }
                    }
                    else
                    {
                       if(comms != nullptr) comms->SendCommand("SMOD,LOC\n"); // Return to local mode
                       return;
                    }
                }
                // Create the folder and define the data storage path and file
                if(path != "")
                {
                    if(properties->AutoFileName) filePath = MakePathUnique(path);
                    else
                    {
                        filePath = path;
                        if(QDir(filePath).exists())
                        {
                            if(statusBar != nullptr) statusBar->showMessage("File path not unique, please try again!", 5000);
                            if(comms != nullptr) comms->SendCommand("SMOD,LOC\n"); // Return to local mode
                            return;
                        }
                    }
                }
                if(!QDir().mkdir(filePath))
                {
                    // If here the defined path cannot be created so warn the user!
                    QMessageBox msgBox;
                    QString msg = "The path you have defined cannot be created. You will need to ";
                    msg += "abort this acquisition and restart with a valid path. The path you defined ";
                    msg += "is: " + filePath + "\n";
                    msgBox.setText(msg);
                    msgBox.exec();
                }
                QDir().setCurrent(filePath);
                cmd += " " + filePath + "/" + fileName;
            }
        }
        if(cla != nullptr)
        {
            if((LastFrameSize != FrameSize) || (LastAccumulations != Accumulations))
            {
                disconnect(cla, &cmdlineapp::Ready, nullptr, nullptr);
                disconnect(cla, &cmdlineapp::AppCompleted, nullptr, nullptr);
                disconnect(cla, &cmdlineapp::DialogClosed, nullptr, nullptr);
                delete cla;
                cla = nullptr;
            }
        }
        if(cla == nullptr)
        {
           // Here is the command line window has not been created or destroyed by
           // the user.
           cla = new cmdlineapp(p);
           connect(cla, &cmdlineapp::Ready, this, &AcquireData::slotAppReady);
           connect(cla, &cmdlineapp::AppCompleted, this, &AcquireData::slotAppFinished);
           connect(cla, &cmdlineapp::DialogClosed, this, &AcquireData::slotDialogClosed);
        }
        cla->appPath = cmd;
        fileName.remove(" -L");
        fileName.remove(" -l");
        cla->appPathNoOptions = filePath + "/" + fileName;
        emit dataFileDefined(cla->appPathNoOptions);
        cla->Clear();
        cla->show();
        cla->raise();
        cla->AppendText(StatusMessage);
        cla->ReadyMessage = "Ready";
        cla->InputRequest = "? Y/N :";
        cla->ContinueMessage = "Acquire complete, continue?: ";
        if(filePath != "") cla->fileName = filePath + "/Acquire.data";
        cla->Execute();
        Acquiring = true;
        if(properties != nullptr) properties->Log("Aquire app started: " + filePath + "/Acquire.data");
        LastFrameSize = FrameSize;
        LastAccumulations = Accumulations;
    }
    else
    {
        if(comms == nullptr) return;
        // Send table start command if in software trigger mode else we just wait
        // for an external trigger
        if(TriggerMode == "Software")
        {
           if(comms->SendCommand("TBLSTRT\n")) {if(statusBar != nullptr) statusBar->showMessage("Table trigger command accepted!", 5000);}
           else if(statusBar != nullptr) statusBar->showMessage("Table trigger command rejected!", 5000);
        }
    }
}

/*! \brief AcquireData::slotDialogClosed
 * Slot: called when the external app window is dismissed; clears the Acquiring flag.
 */
void AcquireData::slotDialogClosed(void)
{
    if(properties != nullptr) properties->Log("Aquire application dialog was closed!");
    cla = nullptr;
}

/*! \brief AcquireData::isRunning
 * Returns true if data acquisition is currently in progress.
 */
bool AcquireData::isRunning(void)
{
   // Oct 8, 2019. Changed this to use the MIPS system table state
   if(cla != nullptr) cla->raise();
   if(isTblMode(comms, TriggerMode)) return(true);
   return(false);
}

/*! \brief AcquireData::slotAppReady
 * Slot: called when the external app signals it is ready; arms MIPS for acquisition.
 */
void AcquireData::slotAppReady(void)
{
    if(properties != nullptr) properties->Log("Aquire app ready");
    if(comms == nullptr) return;
    // Send table start command
    if(comms->SendCommand("TBLSTRT\n"))
    {
        if(statusBar != nullptr) statusBar->showMessage("Table trigger command accepted!", 5000);
        if(properties != nullptr) properties->Log("Table triggered");
    }
    else
    {
        if(statusBar != nullptr) statusBar->showMessage("Table trigger command rejected!", 5000);
        if(properties != nullptr) properties->Log("Table trigger failed");
    }
}

/*! \brief AcquireData::slotAppFinished
 * Slot: called when the external app completes; emits dataAcquired with the result file path.
 */
void AcquireData::slotAppFinished(void)
{
    if(properties != nullptr) properties->Log("Aquire finished");
    // Send a signal that the data collection has finished.
    Acquiring = false;
    if(comms == nullptr) return;
    if(TriggerMode == "Software")
    {
        comms->SendCommand("SMOD,LOC\n");
        if(statusBar != nullptr) statusBar->showMessage("Acquire app finished, returning to local mode.", 5000);
    }
    else
    {
        if(statusBar != nullptr) statusBar->showMessage("Acquire app finished, waiting for next trigger", 5000);
        // This needs work for sure!
    }
    emit dataAcquired(filePath);
}

/*! \brief AcquireData::Dismiss
 * Closes the external app window if it is open.
 */
void AcquireData::Dismiss(void)
{
    if(cla == nullptr) return;
    cla->Dismiss();
}


// *************************************************************************************************
// TimingControl implementation  *******************************************************************
// *************************************************************************************************
