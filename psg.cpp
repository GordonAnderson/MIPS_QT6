// =============================================================================
// psg.cpp
//
// Pulse sequence generator (PSG) tab controller for the MIPS host application.
//
// Depends on:  psg.h, pse.h, psviewer.h, comms.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "psg.h"

static const int PSG_TABLE_WRAP_WIDTH = 60; // Characters per line when displaying table command in a message box

// -----------------------------------------------------------------------------
// Constructor — populates clock/trigger combos, sets validators, connects slots.
// -----------------------------------------------------------------------------
PSG::PSG(Ui::MIPS *w, Comms *c)
{
    pui   = w;
    comms = c;

    // Clock source options
    pui->comboClock->clear();
    pui->comboClock->addItem("Ext");
    pui->comboClock->addItem("ExtN");
    pui->comboClock->addItem("ExtS");
    pui->comboClock->addItem("42000000");
    pui->comboClock->addItem("10500000");
    pui->comboClock->addItem("2625000");
    pui->comboClock->addItem("656250");
    pui->comboClock->setCurrentIndex(6);

    // Trigger mode options
    pui->comboTrigger->clear();
    pui->comboTrigger->addItem("Software");
    pui->comboTrigger->addItem("Edge");
    pui->comboTrigger->addItem("Pos");
    pui->comboTrigger->addItem("Neg");

    pui->leSequenceNumber->setValidator(new QIntValidator);
    pui->leExternClock->setValidator(new QIntValidator);
    pui->leTimePoint->setValidator(new QIntValidator);
    pui->leValue->setValidator(new QIntValidator);

    connect(pui->pbDownload,          &QPushButton::pressed,          this, &PSG::on_pbDownload_pressed);
    connect(pui->pbViewTable,         &QPushButton::pressed,          this, &PSG::on_pbViewTable_pressed);
    connect(pui->pbLoadFromFile,      &QPushButton::pressed,          this, &PSG::on_pbLoadFromFile_pressed);
    connect(pui->pbSaveToFile,        &QPushButton::pressed,          this, &PSG::on_pbSaveToFile_pressed);
    connect(pui->pbEditCurrent,       &QPushButton::pressed,          this, &PSG::on_pbEditCurrent_pressed);
    connect(pui->pbCreateNew,         &QPushButton::pressed,          this, &PSG::on_pbCreateNew_pressed);
    connect(pui->pbTrigger,           &QPushButton::pressed,          this, &PSG::on_pbTrigger_pressed);
    connect(pui->pbExitTableMode,     &QPushButton::pressed,          this, &PSG::on_pbExitTableMode_pressed);
    connect(pui->pbRead,              &QPushButton::pressed,          this, &PSG::on_pbRead_pressed);
    connect(pui->pbWrite,             &QPushButton::pressed,          this, &PSG::on_pbWrite_pressed);
    connect(pui->leSequenceNumber,    &QLineEdit::editingFinished,    this, &PSG::on_leSequenceNumber_textEdited);
    connect(pui->chkAutoAdvance,      &QCheckBox::clicked,            this, &PSG::on_chkAutoAdvance_clicked);
    connect(pui->pbVisPulseSequence,  &QPushButton::pressed,          this, &PSG::on_pbVisPulseSequence_pressed);
}

void PSG::Load(void)
{
    on_pbLoadFromFile_pressed();
}

void PSG::Save(void)
{
    on_pbSaveToFile_pressed();
}

// -----------------------------------------------------------------------------
// Referenced — returns the loop count if point i is the target of a loop in P,
// or -1 if it is not referenced by any loop.
// -----------------------------------------------------------------------------
int PSG::Referenced(QList<psgPoint*> P, int i)
{
    for(int j = 0; j < P.size(); j++)
    {
        if(P[j]->Loop && P[j]->LoopName == P[i]->Name) return P[j]->LoopCount;
    }
    return -1;
}

// -----------------------------------------------------------------------------
// BuildTableCommand — converts a psgPoint list into a MIPS STBLDAT command
// string. Only changed values are encoded at each time point; unchanged
// channels are omitted to minimise command length.
// -----------------------------------------------------------------------------
QString PSG::BuildTableCommand(QList<psgPoint*> P)
{
    QString sTable;
    QString TableName;
    int i, j, Count, TimePointOffset = 0;
    bool NoChange;

    sTable    = "";
    TableName = "A";
    for(i = 0; i < P.size(); i++)
    {
        NoChange = true;
        if(i == 0)
        {
            Count = Referenced(P, i);
            if(Count >= 0)
            {
                sTable += "0:[" + TableName;
                sTable += ":" + QString::number(Count) + "," + QString::number(P[i]->TimePoint - TimePointOffset);
                TableName = TableName[0].unicode()++;
            }
            else sTable += QString::number(P[i]->TimePoint - TimePointOffset);

            for(j = 0; j < PSG_CHANNELS; j++)
            {
                if(P[0]->DCbias[j] != 0)
                {
                    sTable  += ":" + QString::number(j + 1) + ":" + QString::number(P[0]->DCbias[j]);
                    NoChange = false;
                }
            }
            for(j = 0; j < PSG_CHANNELS; j++)
            {
                if(P[0]->DigitalO[j])
                {
                    sTable  += ":" + QString(((QChar)('A')).unicode() += j) + ":1";
                    NoChange = false;
                }
            }
            // At least one channel must be defined at every time point
            if(NoChange)
            {
                sTable += P[i]->DigitalO[0] ? ":A:1" : ":A:0";
            }
        }
        else
        {
            NoChange = true;
            Count    = Referenced(P, i);
            if(Count >= 0)
            {
                sTable += "," + QString::number(P[i]->TimePoint - TimePointOffset) + ":[" + TableName + ":";
                sTable += QString::number(Count) + ",0";
                TableName       = TableName[0].unicode()++;
                TimePointOffset = P[i]->TimePoint;
            }
            else sTable += "," + QString::number(P[i]->TimePoint - TimePointOffset);

            for(j = 0; j < PSG_CHANNELS; j++)
            {
                if(P[i]->DCbias[j] != P[i - 1]->DCbias[j])
                {
                    sTable  += ":" + QString::number(j + 1) + ":" + QString::asprintf("%.3f", P[i]->DCbias[j]);
                    NoChange = false;
                }
            }
            for(j = 0; j < PSG_CHANNELS; j++)
            {
                if(P[i]->DigitalO[j] != P[i - 1]->DigitalO[j])
                {
                    sTable  += ":" + QString(((QChar)'A').unicode() += j) + (P[i]->DigitalO[j] ? ":1" : ":0");
                    NoChange = false;
                }
            }
            // At least one channel must be defined at every time point
            if(NoChange)
            {
                sTable += P[i]->DigitalO[0] ? ":A:1" : ":A:0";
            }
            if(P[i]->Loop)
            {
                sTable += "]";
                if(i > 0) TimePointOffset = P[i]->TimePoint;
            }
        }
    }
    sTable = "STBLDAT;" + sTable + ";";
    return sTable;
}

// -----------------------------------------------------------------------------
// Update — polls PSG state from MIPS and refreshes the UI controls.
// -----------------------------------------------------------------------------
void PSG::Update(void)
{
    pui->tabMIPS->setEnabled(false);
    pui->statusBar->showMessage(tr("Updating Pulse Sequence Generation controls..."));

    if(comms->SendMess("GTBLADV\n").contains("ON")) pui->chkAutoAdvance->setChecked(true);
    else pui->chkAutoAdvance->setChecked(false);
    pui->leSequenceNumber->setText(comms->SendMess("GTBLNUM\n"));

    pui->tabMIPS->setEnabled(true);
    pui->statusBar->showMessage(tr(""));
}

// -----------------------------------------------------------------------------
// on_pbDownload_pressed — sets clock/trigger, sends the table, enters table mode.
// -----------------------------------------------------------------------------
void PSG::on_pbDownload_pressed(void)
{
    QString res;

    pui->pbDownload->setDown(false);
    if(psg.size() == 0)
    {
        QMessageBox::information(this, "Error", "There is no Pulse Sequence to download to MIPS!");
        return;
    }
    comms->SendCommand("SMOD,LOC\n");
    comms->SendCommand("STBLCLK," + pui->comboClock->currentText().toUpper() + "\n");
    res = pui->comboTrigger->currentText().toUpper();
    if(res == "SOFTWARE") res = "SW";
    comms->SendCommand("STBLTRG," + res + "\n");
    comms->SendCommand(BuildTableCommand(psg));
    comms->SendCommand("SMOD,TBL\n");
    comms->waitforline(100);
    pui->statusBar->showMessage(comms->getline());
}

// -----------------------------------------------------------------------------
// on_pbViewTable_pressed — displays the STBLDAT command string in a message box,
// wrapped at PSG_TABLE_WRAP_WIDTH characters per line for readability.
// -----------------------------------------------------------------------------
void PSG::on_pbViewTable_pressed()
{
    pui->pbViewTable->setDown(false);
    if(psg.size() == 0)
    {
        QMessageBox::information(this, "Error", "There is no Pulse Sequence to view!");
        return;
    }
    QString table  = BuildTableCommand(psg);
    QString dtable = "";
    if(table.length() > PSG_TABLE_WRAP_WIDTH)
    {
        for(int i = 0; i < table.length(); i += PSG_TABLE_WRAP_WIDTH)
        {
            dtable += QStringView{table}.mid(i, PSG_TABLE_WRAP_WIDTH);
            dtable += "\n";
        }
        table = dtable;
    }
    QMessageBox::information(this, "Table Command", table);
}

// -----------------------------------------------------------------------------
// on_pbLoadFromFile_pressed — loads a pulse sequence from a .psg binary file.
// -----------------------------------------------------------------------------
void PSG::on_pbLoadFromFile_pressed()
{
    psgPoint *P;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Pulse Sequence File"), "", tr("Files (*.psg *.*)"));

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    psg.clear();
    while(!in.atEnd())
    {
        P = new psgPoint;
        in >> *P;
        psg.push_back(P);
    }
    file.close();
    pui->pbLoadFromFile->setDown(false);
}

// -----------------------------------------------------------------------------
// on_pbCreateNew_pressed — creates a single-point sequence and opens the editor.
// -----------------------------------------------------------------------------
void PSG::on_pbCreateNew_pressed()
{
    psgPoint *point = new psgPoint;

    pui->pbCreateNew->setDown(false);
    if(psg.size() > 0)
    {
        QMessageBox msgBox;
        msgBox.setText("This will overwrite the current Pulse Sequence Table.");
        msgBox.setInformativeText("Do you want to continue?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        if(msgBox.exec() == QMessageBox::No)
        {
            delete point;
            return;
        }
    }
    point->Name = "TP_1";
    psg.clear();
    psg.push_back(point);
    pse = new pseDialog(&psg);
    pse->exec();
}

// -----------------------------------------------------------------------------
// on_pbSaveToFile_pressed — saves the current pulse sequence to a .psg file.
// -----------------------------------------------------------------------------
void PSG::on_pbSaveToFile_pressed()
{
    pui->pbSaveToFile->setDown(false);
    if(psg.size() == 0)
    {
        QMessageBox::information(this, "Error", "There is no Pulse Sequence to save!");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save to Pulse Sequence File"), "", tr("Files (*.psg)"));
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    for(auto it = psg.begin(); it != psg.end(); ++it) out << **it;
    file.close();
    pui->pbSaveToFile->setDown(false);
}

// -----------------------------------------------------------------------------
// on_pbEditCurrent_pressed — opens the sequence editor on the current sequence.
// -----------------------------------------------------------------------------
void PSG::on_pbEditCurrent_pressed()
{
    pui->pbEditCurrent->setDown(false);
    if(psg.size() == 0)
    {
        QMessageBox::information(this, "Error", "There is no Pulse Sequence to edit!");
        return;
    }
    pse = new pseDialog(&psg);
    pse->exec();
}

void PSG::on_leSequenceNumber_textEdited()
{
    comms->SendCommand("STBLNUM," + pui->leSequenceNumber->text() + "\n");
}

void PSG::on_chkAutoAdvance_clicked()
{
    comms->SendCommand(pui->chkAutoAdvance->isChecked() ? "STBLADV,ON\n" : "STBLADV,OFF\n");
}

void PSG::on_pbTrigger_pressed()
{
    comms->SendCommand("TBLSTRT\n");
    comms->waitforline(100);
    pui->statusBar->showMessage(comms->getline());
    comms->waitforline(500);
    pui->statusBar->showMessage(pui->statusBar->currentMessage() + " " + comms->getline());
    comms->waitforline(100);
    pui->statusBar->showMessage(pui->statusBar->currentMessage() + " " + comms->getline());
}

void PSG::on_pbExitTableMode_pressed()
{
    comms->SendCommand("TBLABORT\n");
    comms->SendCommand("SMOD,LOC\n");
}

void PSG::on_pbRead_pressed()
{
    pui->leValue->setText(comms->SendMess("GTBLVLT," + pui->leTimePoint->text() + "," + pui->leChannel->text() + "\n"));
}

void PSG::on_pbWrite_pressed()
{
    comms->SendCommand("STBLVLT," + pui->leTimePoint->text() + "," + pui->leChannel->text() + "," + pui->leValue->text() + "\n");
}

// -----------------------------------------------------------------------------
// on_pbVisPulseSequence_pressed — opens the pulse sequence visualiser.
// -----------------------------------------------------------------------------
void PSG::on_pbVisPulseSequence_pressed()
{
    pui->pbVisPulseSequence->setDown(false);
    if(psg.size() == 0)
    {
        QMessageBox::information(this, "Error", "There is no Pulse Sequence to visualize!");
        return;
    }
    psv = new psviewer(&psg);
    psv->show();
}
