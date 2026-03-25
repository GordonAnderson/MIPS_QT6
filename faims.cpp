// =============================================================================
// faims.cpp
//
// FAIMS (Field Asymmetric Ion Mobility Spectrometry) module for the MIPS
// host application.
//
// Depends on:  faims.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "faims.h"
#include <QElapsedTimer>

QElapsedTimer eTimer;

// File-scope sort key used by the startTime comparator passed to std::sort.
// Index into the CSV record for the Retention Time column.
int key;

// FAIMS — constructor. Populates trigger input/output combo boxes, initialises
// state flags, wires all leS* line edits and button/combo signals, and starts
// the elapsed timer used for CV parking.
FAIMS::FAIMS(Ui::MIPS *w, Comms *c)
{
    fui   = w;
    comms = c;

    // Populate trigger input combo boxes
    const QStringList trigInputs  = {"None", "Q", "R", "S", "T"};
    const QStringList trigOutputs = {"None", "B", "C", "D", "B Active low", "C Active low", "D Active low"};

    fui->comboFMtrig->clear();        fui->comboFMtrig->addItems(trigInputs);
    fui->comboFMlinearTrig->clear();  fui->comboFMlinearTrig->addItems(trigInputs);
    fui->comboFMstepTrig->clear();    fui->comboFMstepTrig->addItems(trigInputs);
    fui->comboFMlinearTrigOut->clear(); fui->comboFMlinearTrigOut->addItems(trigOutputs);
    fui->comboFMstepTrigOut->clear();   fui->comboFMstepTrigOut->addItems(trigOutputs);

    CVparkingTriggered       = false;
    WaitingForLinearScanTrig = false;
    WaitingForStepScanTrig   = false;
    LogFileName              = "";

    // Connect all leS* fields in FAIMS group boxes to FAIMSUpdated
    QObjectList widgetList  = fui->gbFAIMS_DC->children();
    widgetList             += fui->gbFAIMS_RF->children();
    widgetList             += fui->gbLinearScan->children();
    widgetList             += fui->gbStepScan->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().startsWith("leS"))
        {
            QLineEdit *le = qobject_cast<QLineEdit *>(w);
            le->setValidator(new QDoubleValidator);
            connect(le, &QLineEdit::returnPressed, this, &FAIMS::FAIMSUpdated);
        }
    }

    connect(fui->chkFMenable,              &QCheckBox::toggled,             this, &FAIMS::FAIMSenable);
    connect(fui->chkCurtianEna,            &QCheckBox::toggled,             this, &FAIMS::slotFAIMSCurtianEna);
    connect(fui->chkCurtianIndEna,         &QCheckBox::toggled,             this, &FAIMS::slotFAIMSCurtianInd);
    connect(fui->chkNegTune,               &QCheckBox::toggled,             this, &FAIMS::slotFAIMSnegTune);
    connect(fui->pbFMstart,                &QPushButton::pressed,           this, &FAIMS::FAIMSscan);
    connect(fui->pbFMloadCSV,              &QPushButton::pressed,           this, &FAIMS::FAIMSloadCSV);
    connect(fui->pbFMstartLinear,          &QPushButton::pressed,           this, &FAIMS::FAIMSstartLinearScan);
    connect(fui->pbFMabortLinear,          &QPushButton::pressed,           this, &FAIMS::FAIMSabortLinearScan);
    connect(fui->pbFMstartStep,            &QPushButton::pressed,           this, &FAIMS::FAIMSstartStepScan);
    connect(fui->pbFMabortStep,            &QPushButton::pressed,           this, &FAIMS::FAIMSabortStepScan);
    connect(fui->pbFAIMSautoTune,          &QPushButton::pressed,           this, &FAIMS::slotFAIMSautoTune);
    connect(fui->pbFAIMSautoTuneAbort,     &QPushButton::pressed,           this, &FAIMS::slotFAIMSautoTuneAbort);
    connect(fui->rbSFMLOCK_FALSE,          &QRadioButton::clicked,          this, &FAIMS::FAIMSlockOff);
    connect(fui->rbSFMLOCK_TRUE,           &QRadioButton::clicked,          this, &FAIMS::FAIMSlockOn);
    connect(fui->pbSelectLogFile,          &QPushButton::pressed,           this, &FAIMS::FAIMSselectLogFile);
    connect(fui->comboFMlinearTrigOut,     &QComboBox::currentTextChanged,  this, &FAIMS::slotLinearTrigOut);
    connect(fui->comboFMstepTrigOut,       &QComboBox::currentTextChanged,  this, &FAIMS::slotStepTrigOut);

    eTimer.start();
}

// Save — placeholder; FAIMS settings are not persisted (no-op).
void FAIMS::Save(QString)
{
}

// Load — placeholder; FAIMS settings are not restored from file (no-op).
void FAIMS::Load(QString)
{
}

// SetVersionOptions — enables or disables version-gated UI controls (auto-tune,
// negative tune, curtain supply) based on firmware version and hardware presence.
void FAIMS::SetVersionOptions(void)
{
    bool state = false;
    QString res;

    if(comms == nullptr) return;
    int maj, min;
    comms->version(maj, min);
    if((maj > 1) || (min >= 201)) state = true;

    fui->pbFAIMSautoTune->setEnabled(state);
    fui->pbFAIMSautoTuneAbort->setEnabled(state);
    fui->chkNegTune->setEnabled(state);
    fui->lblTuneState->setEnabled(state);
    fui->leGFMTSTAT->setEnabled(state);

    // Enable curtain supply controls only if the hardware is present (FMISCUR returns TRUE)
    res   = comms->SendMess("FMISCUR\n");
    state = (res == "TRUE");
    fui->lblCurtianV->setEnabled(state);
    fui->leSHV_1->setEnabled(state);
    fui->leGHVV_1->setEnabled(state);
    fui->chkCurtianEna->setEnabled(state);
    fui->chkCurtianIndEna->setEnabled(state);
}

// Update — calls SetVersionOptions then polls all FAIMS parameters from MIPS and
// refreshes the UI, including enable/lock/curtain/scan state.
void FAIMS::Update(void)
{
    QString res;

    SetVersionOptions();
    QObjectList widgetList = fui->gbFAIMS_RF->children();
    widgetList += fui->gbFAIMS_DC->children();
    if(fui->tabScanMode->tabText(fui->tabScanMode->currentIndex()) == "Linear scan")
        widgetList += fui->gbLinearScan->children();
    if(fui->tabScanMode->tabText(fui->tabScanMode->currentIndex()) == "Step scan")
        widgetList += fui->gbStepScan->children();

    foreach(QObject *w, widgetList)
    {
        if(w->objectName().startsWith("le"))
        {
            QLineEdit *le = qobject_cast<QLineEdit *>(w);
            if(!le->hasFocus() && le->isEnabled())
            {
                res = "G" + w->objectName().mid(3).replace("_", ",");
                if(res.endsWith(",")) res = res.left(res.length() - 1);
                res += "\n";
                le->setText(comms->SendMess(res));
            }
        }
    }

    res = comms->SendMess("GFMENA\n");
    if(res == "TRUE")  fui->chkFMenable->setChecked(true);
    if(res == "FALSE") fui->chkFMenable->setChecked(false);

    if(fui->chkCurtianEna->isEnabled())
    {
        res = comms->SendMess("GHVSTATUS,1\n");
        if(res == "ON")  fui->chkCurtianEna->setChecked(true);
        if(res == "OFF") fui->chkCurtianEna->setChecked(false);
    }
    if(fui->chkCurtianIndEna->isEnabled())
    {
        res = comms->SendMess("GFMCCUR\n");
        if(res == "TRUE")  fui->chkCurtianIndEna->setChecked(false);
        if(res == "FALSE") fui->chkCurtianIndEna->setChecked(true);
    }
    if(fui->chkNegTune->isEnabled())
    {
        res = comms->SendMess("GFMTPOS\n");
        if(res == "TRUE")  fui->chkNegTune->setChecked(false);
        if(res == "FALSE") fui->chkNegTune->setChecked(true);
    }

    res = comms->SendMess("GFMLOCK\n");
    if(res == "TRUE")
    {
        fui->rbSFMLOCK_TRUE->setChecked(true);
        fui->leSFMSP->setEnabled(true);
        fui->leSFMDRV->setEnabled(false);
    }
    else if(res == "FALSE")
    {
        fui->rbSFMLOCK_FALSE->setChecked(true);
        fui->leSFMSP->setEnabled(false);
        fui->leSFMDRV->setEnabled(true);
    }

    if(fui->tabScanMode->tabText(fui->tabScanMode->currentIndex()) == "Linear scan")
    {
        res = comms->SendMess("GFMSTRTLIN\n");
        if(res == "TRUE")
        {
            fui->pbFMabortLinear->setEnabled(true);
            fui->pbFMstartLinear->setEnabled(false);
        }
        else if(res == "FALSE")
        {
            fui->pbFMabortLinear->setEnabled(false);
            fui->pbFMstartLinear->setEnabled(true);
        }
    }
    if(fui->tabScanMode->tabText(fui->tabScanMode->currentIndex()) == "Step scan")
    {
        res = comms->SendMess("GFMSTRTSTP\n");
        if(res == "TRUE")
        {
            fui->pbFMabortStep->setEnabled(true);
            fui->pbFMstartStep->setEnabled(false);
        }
        else if(res == "FALSE")
        {
            fui->pbFMabortStep->setEnabled(false);
            fui->pbFMstartStep->setEnabled(true);
        }
    }
}

// FAIMSUpdated — slot for returnPressed on all leS* fields. Derives the MIPS
// command from the widget name (strips "le", replaces "_" with ",") and sends it.
void FAIMS::FAIMSUpdated(void)
{
    QObject* obj = sender();
    QLineEdit *le = qobject_cast<QLineEdit *>(obj);
    if(!le->isModified()) return;
    QString res = obj->objectName().mid(2).replace("_", ",");
    if(res.endsWith(",")) res = res.left(res.length() - 1);
    res += "," + le->text() + "\n";
    comms->SendCommand(res);
    le->setModified(false);
}

// FAIMSenable — slot for the FAIMS enable checkbox; sends SFMENA,TRUE|FALSE.
void FAIMS::FAIMSenable(void)
{
    comms->SendCommand(fui->chkFMenable->isChecked() ? "SFMENA,TRUE\n" : "SFMENA,FALSE\n");
}

// FAIMSscan — starts or stops the CV parking scan. If stopping, clears state.
// If starting with no trigger, arms immediately; otherwise waits for the
// selected DIO trigger input before beginning the first CV parking target.
void FAIMS::FAIMSscan(void)
{
    fui->leFMcompound->setText("");
    fui->leFMcvVolts->setText("");
    fui->leFMbiasVolts->setText("");

    if(CVparkingTriggered || fui->pbFMstart->text() == "Stop")
    {
        CVparkingTriggered = false;
        fui->pbFMstart->setText("Start");
        fui->leFMstatus->setText("Stopped");
    }
    else
    {
        // Disable all trigger reports before arming
        comms->SendCommand("RPT,Q,OFF\n");
        comms->SendCommand("RPT,R,OFF\n");
        comms->SendCommand("RPT,S,OFF\n");
        comms->SendCommand("RPT,T,OFF\n");
        fui->pbFMstart->setText("Stop");
        if(fui->comboFMtrig->currentText() == "None")
        {
            CVparkingTriggered = true;
            fui->leFMstatus->setText("Running");
            eTimer.restart();
        }
        else
        {
            fui->leFMstatus->setText("Waiting for trigger");
            comms->SendCommand("RPT," + fui->comboFMtrig->currentText() + ",RISING\n");
        }
        CurrentPoint = 0;
        if(!GetNextTarget(0))
        {
            CVparkingTriggered = false;
            fui->pbFMstart->setText("Start");
            fui->leFMstatus->setText("Finished");
        }
    }
}

// getHeaderIndex — returns the 0-based column index of the CSV header field
// whose name starts with Name, or -1 if not found.
int FAIMS::getHeaderIndex(QString Name)
{
    QStringList resList = Header.split(",");
    for(int i = 0; i < resList.count(); i++)
    {
        if(resList.at(i).startsWith(Name)) return i;
    }
    return -1;
}

// getCSVtoken — returns the value of field Name from the CSV record at index,
// or an empty string if the field is not found.
QString FAIMS::getCSVtoken(QString Name, int index)
{
    QStringList resList = Header.split(",");
    for(int i = 0; i < resList.count(); i++)
    {
        if(resList.at(i).startsWith(Name))
        {
            QStringList recordList = Records.at(index).split(",");
            if(recordList.count() > i) return recordList.at(i);
        }
    }
    return "";
}

// startTime — comparator for std::sort. Sorts CSV records by ascending window
// start time (Retention Time − RT Window / 2) using the file-scope 'key' index.
bool startTime(const QString v1, const QString v2)
{
    QStringList v1List = v1.split(",");
    QStringList v2List = v2.split(",");
    return (v1List.at(key).toFloat() - v1List.at(key + 1).toFloat() / 2)
         < (v2List.at(key).toFloat() - v2List.at(key + 1).toFloat() / 2);
}

// FAIMSloadCSV — opens a CSV target file, reads the header and records, sorts
// by window start time, and populates the CV parking list text area.
void FAIMS::FAIMSloadCSV(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load CSV target file"), "", tr("CSV (*.csv);;All files (*.*)"));
    if(fileName == "") return;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        Header = stream.readLine();  // First line is the CSV header
        Records.clear();
        QString line;
        do
        {
            line = stream.readLine();
            if(line != "") Records.append(line);
        } while(!line.isNull());
        file.close();

        // Sort records by start time (RT - RTWindow/2)
        key = getHeaderIndex("Retention Time");
        std::sort(Records.begin(), Records.end(), startTime);

        for(int i = 0; i < Records.count(); i++)
        {
            QString rec  = getCSVtoken("Compound",           i) + ",";
            rec         += getCSVtoken("Retention Time",     i) + ",";
            rec         += getCSVtoken("RT Window",          i) + ",";
            rec         += getCSVtoken("Compensation Voltage",i) + ",";
            rec         += getCSVtoken("Bias",               i);
            fui->textCVparkList->append(rec);
        }
        fui->statusBar->showMessage("CSV target file read " + fileName, 2000);
    }
}

// GetNextTarget — finds the next CV parking entry whose time window has not yet
// ended at elapsed time et (minutes), sets the target fields, and advances
// CurrentPoint. Returns false when all entries are exhausted.
bool FAIMS::GetNextTarget(float et)
{
    QStringList entries;

    if(CurrentPoint == 0)
    {
        QString text = fui->textCVparkList->toPlainText();
        Parks = text.split("\n");
    }
    while(1)
    {
        while(1)
        {
            if((CurrentPoint + 1) > Parks.count()) return false;
            entries = Parks.at(CurrentPoint).split(",");
            if(entries.count() == 5) break;
            CurrentPoint++;
        }
        if((entries.at(1).toFloat() + entries.at(2).toFloat() / 2) > et)
        {
            TargetCompound = entries.at(0);
            TargetRT       = entries.at(1).toFloat();
            TargetWindow   = entries.at(2).toFloat();
            TargetCV       = entries.at(3).toFloat();
            TargetBias     = entries.at(4).toFloat();
            CurrentPoint++;
            State = WAITING;
            return true;
        }
        else CurrentPoint++;
    }
}

// PollLoop — called from the main update timer. Services the active scan mode:
// detects DIO trigger edges for CV parking/linear/step scans, advances CV parking
// state machine, and calls Update/Log on each cycle when no trigger is pending.
void FAIMS::PollLoop(void)
{
    float CurrentMin;
    QString res;
    QStringList reslist;

    if(fui->tabScanMode->tabText(fui->tabScanMode->currentIndex()) == "CV parking")
    {
        if(fui->leFMstatus->text() == "Waiting for trigger")
        {
            // Watching for DIC,<ch>,RISING,<time> from the selected trigger input
            res = comms->getline();
            if(res.startsWith("DIC,"))
            {
                reslist = res.split(",");
                if(reslist.count() == 4)
                {
                    if((reslist.at(2) == "RISING") && (reslist.at(1) == fui->comboFMtrig->currentText()))
                    {
                        CVparkingTriggered = true;
                        fui->leFMstatus->setText("Triggered");
                        eTimer.restart();
                    }
                }
            }
            return;
        }
        if(!CVparkingTriggered)
        {
            if(fui->tabMIPS->tabText(fui->tabMIPS->currentIndex()) == "FAIMS")
            {
                Update();
                Log("");
            }
            return;
        }
        CurrentMin = (float)eTimer.elapsed() / (float)60000;
        fui->leFMelasped->setText(QString::number(CurrentMin, 'f', 1));
        switch(State)
        {
            case WAITING:
                if(CurrentMin >= TargetRT - TargetWindow / 2)
                {
                    fui->leFMcompound->setText(TargetCompound);
                    fui->leFMcvVolts->setText(QString::number(TargetCV));
                    fui->leFMbiasVolts->setText(QString::number(TargetBias));
                    comms->SendCommand("SFMCV,"   + QString::number(TargetCV)   + "\n");
                    comms->SendCommand("SFMBIAS," + QString::number(TargetBias) + "\n");
                    State = SCANNING;
                }
                break;
            case SCANNING:
                if(CurrentMin >= TargetRT + TargetWindow / 2)
                {
                    if(!GetNextTarget(CurrentMin))
                    {
                        CVparkingTriggered = false;
                        fui->pbFMstart->setText("Start");
                        fui->leFMstatus->setText("Finished");
                    }
                    if(CurrentMin < TargetRT - TargetWindow / 2) fui->leFMcompound->setText("");
                }
                break;
            default:
                break;
        }
    }

    if(fui->tabScanMode->tabText(fui->tabScanMode->currentIndex()) == "Linear scan")
    {
        if(WaitingForLinearScanTrig)
        {
            res = comms->getline();
            if(res.startsWith("DIC,"))
            {
                reslist = res.split(",");
                if(reslist.count() == 4)
                {
                    if((reslist.at(2) == "RISING") && (reslist.at(1) == fui->comboFMlinearTrig->currentText()))
                    {
                        WaitingForLinearScanTrig = false;
                        fui->statusBar->showMessage(tr("Scan triggered!"));
                        comms->SendCommand("SFMSTRTLIN,TRUE\n");
                        Log("Linear scan triggered!");
                    }
                }
            }
            return;
        }
    }

    if(fui->tabScanMode->tabText(fui->tabScanMode->currentIndex()) == "Step scan")
    {
        if(WaitingForStepScanTrig)
        {
            res = comms->getline();
            if(res.startsWith("DIC,"))
            {
                reslist = res.split(",");
                if(reslist.count() == 4)
                {
                    if((reslist.at(2) == "RISING") && (reslist.at(1) == fui->comboFMstepTrig->currentText()))
                    {
                        WaitingForStepScanTrig = false;
                        fui->statusBar->showMessage(tr("Scan triggered!"));
                        comms->SendCommand("SFMSTRTSTP,TRUE\n");
                        Log("Step scan triggered!");
                    }
                }
            }
            return;
        }
    }

    if(fui->tabMIPS->tabText(fui->tabMIPS->currentIndex()) == "FAIMS")
    {
        Update();
        Log("");
    }
}

// FAIMSstartLinearScan — starts a CV linear scan. With "None" trigger, asserts
// the selected digital output for 250 ms and sends SFMSTRTLIN,TRUE immediately.
// Otherwise arms a DIO trigger input and waits for the rising edge in PollLoop.
void FAIMS::FAIMSstartLinearScan(void)
{
    QElapsedTimer timer;

    if(comms == NULL) return;

    // Disable all trigger reports before arming
    comms->SendCommand("RPT,Q,OFF\n");
    comms->SendCommand("RPT,R,OFF\n");
    comms->SendCommand("RPT,S,OFF\n");
    comms->SendCommand("RPT,T,OFF\n");

    if(fui->comboFMlinearTrig->currentText() == "None")
    {
        QString trigOut = fui->comboFMlinearTrigOut->currentText();
        // Assert trigger output
        if(trigOut == "B")           comms->SendCommand("SDIO,B,1\n");
        if(trigOut == "C")           comms->SendCommand("SDIO,C,1\n");
        if(trigOut == "D")           comms->SendCommand("SDIO,D,1\n");
        if(trigOut == "B Active low") comms->SendCommand("SDIO,B,0\n");
        if(trigOut == "C Active low") comms->SendCommand("SDIO,C,0\n");
        if(trigOut == "D Active low") comms->SendCommand("SDIO,D,0\n");
        fui->statusBar->showMessage(tr("Scan triggered!"));
        comms->SendCommand("SFMSTRTLIN,TRUE\n");
        Log("Linear scan started,CV start = " + fui->leSFMCVSTART->text()
            + ",CV end = "    + fui->leSFMCVEND->text()
            + ",Duration, S = " + fui->leSFMDUR->text()
            + ",Loops = "     + fui->leSFMLOOPS->text());
        timer.start();
        while(timer.elapsed() < 250) QApplication::processEvents();
        // Deassert trigger output after 250 ms
        if(trigOut == "B")           comms->SendCommand("SDIO,B,0\n");
        if(trigOut == "C")           comms->SendCommand("SDIO,C,0\n");
        if(trigOut == "D")           comms->SendCommand("SDIO,D,0\n");
        if(trigOut == "B Active low") comms->SendCommand("SDIO,B,1\n");
        if(trigOut == "C Active low") comms->SendCommand("SDIO,C,1\n");
        if(trigOut == "D Active low") comms->SendCommand("SDIO,D,1\n");
    }
    else
    {
        comms->SendCommand("RPT," + fui->comboFMlinearTrig->currentText() + ",RISING\n");
        fui->statusBar->showMessage(tr("Waiting for scan trigger!"));
        WaitingForLinearScanTrig = true;
        Log("Linear scan armed,CV start = " + fui->leSFMCVSTART->text()
            + ",CV end = "    + fui->leSFMCVEND->text()
            + ",Duration, S = " + fui->leSFMDUR->text()
            + ",Loops = "     + fui->leSFMLOOPS->text());
    }
    fui->pbFMstartLinear->setEnabled(false);
    fui->pbFMabortLinear->setEnabled(true);
}

// FAIMSabortLinearScan — cancels any pending linear scan trigger and sends
// SFMSTRTLIN,FALSE to halt the scan.
void FAIMS::FAIMSabortLinearScan(void)
{
    WaitingForLinearScanTrig = false;
    fui->statusBar->showMessage(tr("Scan aborted!"));
    comms->SendCommand("SFMSTRTLIN,FALSE\n");
    fui->pbFMstartLinear->setEnabled(true);
    fui->pbFMabortLinear->setEnabled(false);
    Log("Linear scan aborted!");
}

// FAIMSstartStepScan — starts a CV step scan. Same trigger/output logic as
// FAIMSstartLinearScan but uses SFMSTRTSTP and the step scan trigger combo.
void FAIMS::FAIMSstartStepScan(void)
{
    QElapsedTimer timer;

    if(comms == NULL) return;

    // Disable all trigger reports before arming
    comms->SendCommand("RPT,Q,OFF\n");
    comms->SendCommand("RPT,R,OFF\n");
    comms->SendCommand("RPT,S,OFF\n");
    comms->SendCommand("RPT,T,OFF\n");

    if(fui->comboFMstepTrig->currentText() == "None")
    {
        QString trigOut = fui->comboFMstepTrigOut->currentText();
        // Assert trigger output
        if(trigOut == "B")           comms->SendCommand("SDIO,B,1\n");
        if(trigOut == "C")           comms->SendCommand("SDIO,C,1\n");
        if(trigOut == "D")           comms->SendCommand("SDIO,D,1\n");
        if(trigOut == "B Active low") comms->SendCommand("SDIO,B,0\n");
        if(trigOut == "C Active low") comms->SendCommand("SDIO,C,0\n");
        if(trigOut == "D Active low") comms->SendCommand("SDIO,D,0\n");
        fui->statusBar->showMessage(tr("Scan triggered!"));
        comms->SendCommand("SFMSTRTSTP,TRUE\n");
        Log("Step scan started,CV start = " + fui->leSFMCVSTART_->text()
            + ",CV end = "       + fui->leSFMCVEND_->text()
            + ",Step time mS = " + fui->leSFMSTPTM->text()
            + ",Steps = "        + fui->leSFMSTEPS->text()
            + ",Loops = "        + fui->leSFMLOOPS_->text());
        timer.start();
        while(timer.elapsed() < 250) QApplication::processEvents();
        // Deassert trigger output after 250 ms
        if(trigOut == "B")           comms->SendCommand("SDIO,B,0\n");
        if(trigOut == "C")           comms->SendCommand("SDIO,C,0\n");
        if(trigOut == "D")           comms->SendCommand("SDIO,D,0\n");
        if(trigOut == "B Active low") comms->SendCommand("SDIO,B,1\n");
        if(trigOut == "C Active low") comms->SendCommand("SDIO,C,1\n");
        if(trigOut == "D Active low") comms->SendCommand("SDIO,D,1\n");
    }
    else
    {
        comms->SendCommand("RPT," + fui->comboFMstepTrig->currentText() + ",RISING\n");
        fui->statusBar->showMessage(tr("Waiting for scan trigger!"));
        WaitingForStepScanTrig = true;
        Log("Step scan armed,CV start = " + fui->leSFMCVSTART_->text()
            + ",CV end = "       + fui->leSFMCVEND_->text()
            + ",Step time mS = " + fui->leSFMSTPTM->text()
            + ",Steps = "        + fui->leSFMSTEPS->text()
            + ",Loops = "        + fui->leSFMLOOPS_->text());
    }
    fui->pbFMstartLinear->setEnabled(false);
    fui->pbFMabortLinear->setEnabled(true);
}

// FAIMSabortStepScan — cancels any pending step scan trigger and sends
// SFMSTRTSTP,FALSE to halt the scan.
void FAIMS::FAIMSabortStepScan(void)
{
    WaitingForStepScanTrig = false;
    fui->statusBar->showMessage(tr("Scan aborted!"));
    comms->SendCommand("SFMSTRTSTP,FALSE\n");
    fui->pbFMstartStep->setEnabled(true);
    fui->pbFMabortStep->setEnabled(false);
    Log("Step scan aborted!");
}

// FAIMSlockOff — disables closed-loop CV lock mode (SFMLOCK,FALSE).
void FAIMS::FAIMSlockOff(void)
{
    comms->SendCommand("SFMLOCK,FALSE\n");
    Log("Output lock mode disabled requested.");
}

// FAIMSlockOn — enables closed-loop CV lock mode (SFMLOCK,TRUE).
void FAIMS::FAIMSlockOn(void)
{
    comms->SendCommand("SFMLOCK,TRUE\n");
    Log("Output lock mode enable requested.");
}

// Log — appends a timestamped entry to LogFileName. If Message is empty,
// logs the full current operating parameter set; otherwise logs the message string.
void FAIMS::Log(QString Message)
{
    if(LogFileName == "") return;

    QFile file(LogFileName);
    if(file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream stream(&file);
        if(Message == "")
        {
            QString LogString;
            LogString  = QDateTime::currentDateTime().toString() + ",";
            LogString += fui->chkFMenable->isChecked() ? "Enabled," : "Disabled,";
            LogString += fui->leSFMDRV->text() + ",";
            if(fui->rbSFMLOCK_FALSE->isChecked()) LogString += "Manual,";
            if(fui->rbSFMLOCK_TRUE->isChecked())  LogString += "Lock,";
            LogString += fui->leSFMSP->text()      + ",";
            LogString += fui->leGFMPWR->text()     + ",";
            LogString += fui->leGFMPV->text()      + ",";
            LogString += fui->leGFMNV->text()      + ",";
            LogString += fui->leGFMBIASA->text()   + ",";
            LogString += fui->leGFMCVA->text()     + ",";
            LogString += fui->leGFMOFFA->text()    + "\n";
            stream << LogString;
        }
        else
        {
            stream << QDateTime::currentDateTime().toString() + "," << Message + "\n";
        }
        file.close();
    }
}

// FAIMSselectLogFile — opens a save-file dialog to choose a .log path, then
// creates the file and writes the CSV header row.
void FAIMS::FAIMSselectLogFile(void)
{
    LogFileName = QFileDialog::getSaveFileName(this, tr("Log File"), "", tr("Settings (*.log);;All files (*.*)"));
    fui->LogFile->setText(LogFileName);
    QFile file(LogFileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << "# FAIMS log file, " + QDateTime::currentDateTime().toString() + "\n";
        stream << "Time,Enable,Drive,Mode,Voltage SP,Power,Pos KV,Neg KV,DC bias,DC CV,DC offset\n";
        file.close();
    }
}

// slotLinearTrigOut — slot for linear scan trigger output combo. Sets the selected
// DIO line to its idle (deasserted) state when the selection changes.
void FAIMS::slotLinearTrigOut(void)
{
    if(comms == NULL) return;
    QString trigOut = fui->comboFMlinearTrigOut->currentText();
    if(trigOut == "B")           comms->SendCommand("SDIO,B,0\n");
    if(trigOut == "C")           comms->SendCommand("SDIO,C,0\n");
    if(trigOut == "D")           comms->SendCommand("SDIO,D,0\n");
    if(trigOut == "B Active low") comms->SendCommand("SDIO,B,1\n");
    if(trigOut == "C Active low") comms->SendCommand("SDIO,C,1\n");
    if(trigOut == "D Active low") comms->SendCommand("SDIO,D,1\n");
}

// slotStepTrigOut — slot for step scan trigger output combo. Sets the selected
// DIO line to its idle (deasserted) state when the selection changes.
void FAIMS::slotStepTrigOut(void)
{
    if(comms == NULL) return;
    QString trigOut = fui->comboFMstepTrigOut->currentText();
    if(trigOut == "B")           comms->SendCommand("SDIO,B,0\n");
    if(trigOut == "C")           comms->SendCommand("SDIO,C,0\n");
    if(trigOut == "D")           comms->SendCommand("SDIO,D,0\n");
    if(trigOut == "B Active low") comms->SendCommand("SDIO,B,1\n");
    if(trigOut == "C Active low") comms->SendCommand("SDIO,C,1\n");
    if(trigOut == "D Active low") comms->SendCommand("SDIO,D,1\n");
}

// slotFAIMSautoTune — sends SFMTUNE to start the RF auto-tune routine.
// slotFAIMSautoTuneAbort — sends SFMTABRT to abort an in-progress auto-tune.
void FAIMS::slotFAIMSautoTune(void)      { if(comms) comms->SendCommand("SFMTUNE\n"); }
void FAIMS::slotFAIMSautoTuneAbort(void) { if(comms) comms->SendCommand("SFMTABRT\n"); }

// slotFAIMSCurtianEna — enables or disables the curtain supply HV via SHVENA/SHVDIS.
void FAIMS::slotFAIMSCurtianEna(void)
{
    comms->SendCommand(fui->chkCurtianEna->isChecked() ? "SHVENA,1\n" : "SHVDIS,1\n");
}

// slotFAIMSCurtianInd — toggles the curtain independent control mode (SFMCCUR).
void FAIMS::slotFAIMSCurtianInd(void)
{
    comms->SendCommand(fui->chkCurtianIndEna->isChecked() ? "SFMCCUR,FALSE\n" : "SFMCCUR,TRUE\n");
}

// slotFAIMSnegTune — toggles negative-polarity tuning mode (SFMTPOS).
void FAIMS::slotFAIMSnegTune(void)
{
    comms->SendCommand(fui->chkNegTune->isChecked() ? "SFMTPOS,FALSE\n" : "SFMTPOS,TRUE\n");
}
