// =============================================================================
// timingcontrol.cpp
//
// TimingControl — top-level timing control widget wrapping TimingGenerator
with Edit/Trigger/Abort buttons and AcquireData. Extracted from timinggenerator.cpp.
//
// Depends on:  timinggenerator.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "timinggenerator.h"
#include "ui_timinggenerator.h"
#include "Utilities.h"
#include "controlpanel.h"

/*! \brief TimingControl::TimingControl
 * Constructor. Stores parent, name, MIPS name, and position; creates the AcquireData helper.
 */
TimingControl::TimingControl(QWidget *parent, QString name, QString MIPSname, int x, int y) : QWidget(parent)
{
    p      = parent;
    Title  = name;
    MIPSnm = MIPSname;
    X      = x;
    Y      = y;
    comms  = nullptr;
    Acquire = "";
    statusBar = nullptr;
    properties = nullptr;
    Acquiring = false;
    Downloading = false;
    fileName = "U1084A.data";
    FrameCtAdj = 1;
    AlwaysGenerate = false;
}

/*! \brief TimingControl::Show
 * Creates the group box, Edit/Trigger/Abort buttons, connects signals, and shows the widget.
 */
void TimingControl::Show(void)
{
    // Make a group box
    gbTC = new QGroupBox(Title,p);
    gbTC->setGeometry(X,Y,140,120);
    gbTC->setToolTip(MIPSnm + " Timing generation");
    // Place the controls on the group box
    Edit = new QPushButton("Edit",gbTC);       Edit->setGeometry(20,25,100,32); Edit->setAutoDefault(false);
    Trigger = new QPushButton("Trigger",gbTC); Trigger->setGeometry(20,55,100,32); Trigger->setAutoDefault(false);
    Abort = new QPushButton("Abort",gbTC);     Abort->setGeometry(20,85,100,32); Abort->setAutoDefault(false);
    // Connect to the event slots, changed from QueuedConnection July 24, 2025
    connect(Edit, &QPushButton::pressed, this, &TimingControl::pbEdit);
    connect(Trigger, &QPushButton::pressed, this, &TimingControl::pbTrigger);
    connect(Abort, &QPushButton::pressed, this, &TimingControl::pbAbort);
    TableDownloaded = false;
    TG = new TimingGenerator(p,Title,MIPSnm);
    TG->comms = comms;
    TG->Trigger = Trigger;
    TG->Abort = Abort;
    TG->Edit = Edit;
    TG->properties = properties;
    TG->FrameCtAdj = FrameCtAdj;
    // Create the AcquireData object and init
    AD = new class AcquireData(p);
    AD->fileName = fileName;
    AD->comms = comms;
    AD->statusBar = statusBar;
    AD->properties = properties;
    connect(AD, &AcquireData::dataAcquired, this, &TimingControl::slotDataAcquired);
    connect(AD, &AcquireData::dataFileDefined, this, &TimingControl::slotDataFileDefined);
    gbTC->installEventFilter(this);
    gbTC->setMouseTracking(true);
}

/*! \brief TimingControl::eventFilter
 * Event filter: passes events through to the base class.
 */
bool TimingControl::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, gbTC, gbTC , event)) return false;
    return false;
}


/*! \brief TimingControl::pbEdit
 * Slot: opens the TimingGenerator dialog for editing.
 */
void TimingControl::pbEdit(void)
{
   Edit->setDown(false);
   TG->show();
   TG->raise();
}

/*! \brief TimingControl::pbTrigger
 * Slot: downloads the timing table if needed and triggers execution on MIPS.
 */
void TimingControl::pbTrigger(void)
{

    Trigger->setDown(false);
    if(AD->TriggerMode == "Software")
    {
        if(AD->isRunning())
        {
            if(properties != nullptr) properties->Log("Timing control trigger pressed while running!");
            return;
        }
        if(properties != nullptr) properties->Log("Timing control trigger pressed.");
    }
    else
    {
        // Here if in external trigger mode so test if we are in table mode, if
        // so ask if user would like to force a trigger.
        if(AD->isRunning())
        {
            if(properties != nullptr) properties->Log("Timing control trigger pressed while running!");
            // Now ask user about forcing a trigger
            QMessageBox msgBox;
            QString msg = "The system is in external trigger mode waiting for a trigger event.\n";
            msg += "Select Yes if you would like to force a trigger or No to exit and keep waiting.\n";
            msgBox.setText(msg);
            msgBox.setInformativeText("Select Yes to force a trigger?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int ret = msgBox.exec();
            if(ret == QMessageBox::Yes)
            {
                if(properties != nullptr) properties->Log("Timing control trigger forced.");
                if(comms->SendCommand("TBLSTRT\n")) {if(statusBar != nullptr) statusBar->showMessage("Table trigger command accepted!", 5000);}
                else if(statusBar != nullptr) statusBar->showMessage("Table trigger command rejected!", 5000);
                return;
            }
            else return;
        }
        if(properties != nullptr) properties->Log("Timing control trigger pressed, external triggers enabled.");
    }
    // If the table line edit box is empty, generate table
    AD->fileName = fileName;
    AD->TriggerMode = TG->ui->comboTriggerSource->currentText();
    if(properties != nullptr) properties->Log("Trigger mode: " + AD->TriggerMode);
    if(AlwaysGenerate) TG->ui->leTable->setText("");
    if(TG->ui->leTable->text() == "") TG->slotGenerate();
    Downloading = true;
    // Set the external clock frequency
    if(comms != nullptr) comms->SendCommand("SEXTFREQ," + TG->ui->leExtClkFreq->text() + "\n");
    // Send table
    TableDownloaded = DownloadTable(comms, TG->ui->leTable->text(),TG->ui->comboClockSource->currentText(), TG->ui->comboTriggerSource->currentText());
    TG->UpdateEvents();
    Downloading = false;
    if(properties != nullptr)
    {
        if((properties->DataFilePath != "") && (properties->FileName != "")) AcquireData(properties->DataFilePath + "/" + properties->FileName);
        else AcquireData("");
    }
    else AcquireData("");
}

/*! \brief TimingControl::slotDataAcquired
 * Slot: forwards the dataAcquired signal from AcquireData.
 */
void TimingControl::slotDataAcquired(QString filepath)
{
    emit dataAcquired(filepath);
}

/*! \brief TimingControl::slotDataFileDefined
 * Slot: forwards the dataFileDefined signal from AcquireData.
 */
void TimingControl::slotDataFileDefined(QString filepath)
{
    emit dataFileDefined(filepath);
}

/*! \brief TimingControl::slotEventChanged
 * Slot: sends an updated event value to MIPS.
 */
void TimingControl::slotEventChanged(QString ECname, QString Val)
{
    QStringList resList;

    resList = ECname.split(".");
    if(resList.count() != 2) return;
    foreach(Event *e, TG->Events)
    {
        if(e->Name.toUpper() == resList[0].toUpper())
        {
            if(resList[1].toUpper() == "VALUE")
            {
                e->Value = Val.toFloat();
                if(TG->ui->leEventName->text() == resList[0]) TG->ui->leEventValue->setText(Val);
            }
            if(resList[1].toUpper() == "VALUEOFF")
            {
                e->ValueOff = Val.toFloat();
                if(TG->ui->leEventName->text() == resList[0]) TG->ui->leEventValueOff->setText(Val);
            }
            if(resList[1].toUpper() == "START")
            {
                e->Start = Val.trimmed();
                if(TG->ui->leEventName->text() == resList[0]) TG->ui->leEventStart->setText(Val);
            }
            if(resList[1].toUpper() == "WIDTH")
            {
                e->Width = Val.trimmed();
                if(TG->ui->leEventName->text() == resList[0]) TG->ui->leEventWidth->setText(Val);
            }
            if(resList[1].toUpper() == "SIGNAL")
            {
                int i = TG->ui->comboEventSignal->findText(Val.trimmed());
                e->Channel = TG->ui->comboEventSignal->itemData(i).toString();
                if(TG->ui->leEventName->text() == resList[0]) TG->ui->comboEventSignal->setCurrentIndex(TG->ui->comboEventSignal->findData(e->Channel));
            }
        }
    }

}

/*! \brief TimingControl::AcquireData
 * Starts the AcquireData helper with the given file path.
 */
void TimingControl::AcquireData(QString path)
{
    if(TableDownloaded == false)
    {
        TableDownloaded = DownloadTable(comms, TG->ui->leTable->text(),TG->ui->comboClockSource->currentText(), TG->ui->comboTriggerSource->currentText());
    }
    AD->TableDownloaded = TableDownloaded;
    AD->StartAcquire(path, TG->ConvertToCount(TG->ui->leFrameWidth->text()), TG->ui->leAccumulations->text().toInt());
}

/*! \brief TimingControl::pbAbort
 * Slot: aborts the active table sequence on MIPS.
 */
void TimingControl::pbAbort(void)
{
    Abort->setDown(false);
    if(comms == nullptr) return;
    // Send table abort command
    if(comms->SendCommand("TBLABRT\n")) { if(statusBar != nullptr) statusBar->showMessage("Table mode aborted!", 5000); }
    else if(statusBar != nullptr) statusBar->showMessage("Table mode abort error!", 5000);
}

// *************************************************************************************************
// EventControl implementation     *****************************************************************
// *************************************************************************************************
