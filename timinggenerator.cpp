// =============================================================================
// timinggenerator.cpp
//
// TimingGenerator — pulse/table sequence generator for MIPS hardware.
// Implements AcquireData, EventControl, TimingControl, and TimingGenerator,
// plus free functions isTblMode(), DownloadTable(), and MakePathUnique().
//
// Depends on:  timinggenerator.h, Utilities.h, controlpanel.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 1+2+3 refactoring
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "timinggenerator.h"
#include "ui_timinggenerator.h"
#include "Utilities.h"
#include "controlpanel.h"

// This function tests if the system is in table mode by sending the trigger source
// option. If its in table mode then a NAK will be received from MIPS
/*! \brief isTblMode
 * Tests whether the MIPS system is currently in table mode by probing the trigger-source command.
 */
bool isTblMode(Comms *comms, QString TriggerSource)
{
   if(comms == nullptr) return false;
   QString res = TriggerSource.toUpper();
   if(res == "SOFTWARE") res = "SW";
   comms->rb.clear();
   comms->SendString("STBLTRG," + res + "\n");
   comms->waitforline(1000);
   if(comms->rb.numLines() >= 1)
   {
       res = comms->rb.getline();
       if(res == "?") return true;
       if(res == "") return false;
   }
   return false;
}

// Download table parameters to MIPS.
// If the system is in table mode it will be placed in local mode.
// The table is downloaded and the system will remain in local mode.
// Returns false if an error is detected
/*! \brief DownloadTable
 * Downloads the timing table string to MIPS with the specified clock and trigger source settings.
 */
bool DownloadTable(Comms *comms, QString Table, QString ClockSource, QString TriggerSource)
{
   if(comms == nullptr) return false;
   comms->SendCommand("SMOD,LOC\n");        // Make sure the system is in local mode
   comms->SendCommand("STBLREPLY,FALSE\n"); // Turn off any table messages from MIPS
   // Make sure a table has been generated
   if(Table == "")
   {
       QMessageBox msgBox;
       msgBox.setText("There is no Pulse Sequence to download to MIPS!");
       msgBox.exec();
       return false;
   }
   // Set clock
   comms->SendCommand("STBLCLK," + ClockSource.toUpper() + "\n");
   // Set trigger
   QString res = TriggerSource.toUpper();
   if(res == "SOFTWARE") res = "SW";
   comms->SendCommand("STBLTRG," + res + "\n");
   // Send table
   comms->SendCommand(Table.trimmed());
   return true;
}

/*! \brief MakePathUnique
 * Returns a unique file path by appending or incrementing a numeric suffix if the path already exists.
 */
QString MakePathUnique(QString path)
{
    int i, num;

    // Check if path exists, if it does then see if it ends in a number, if
    // it does then increment this number, if it does not append -0001 to the
    // name
    while(true)
    {
       if(QDir(path).exists())
       {
           for(i=0;i<path.length();i++) if(!path[path.length()-1-i].isDigit()) break;
           if(i == 0) path += "-0001";
           else
           {
               num = QStringView{path}.right(i).toInt();
               num++;
               path = path.left(path.length()-i);
               path += QString("%1").arg(num,4,10,QLatin1Char('0'));
           }
       }
       else break;
    }
    // At this point path is unique
    return path;
}

// **********************************************************************************************
// AcquireData implementation *******************************************************************
// **********************************************************************************************
/*! \brief TimingGenerator::TimingGenerator
 * Constructor. Sets up the UI, populates combo boxes, and connects all control signals.
 */
TimingGenerator::TimingGenerator(QWidget *parent, QString name, QString MIPSname) :
    QDialog(parent),
    ui(new Ui::TimingGenerator)
{
    ui->setupUi(this);

    p         = parent;
    Title     = name;
    MIPSnm    = MIPSname;
    comms     = nullptr;
    statusBar = nullptr;
    Events.clear();
    EC.clear();
    selectedEvent = nullptr;

    QWidget::setWindowTitle(Title + " editor");
    ui->comboClockSource->clear();
    ui->comboClockSource->addItem("Ext","Ext");
    ui->comboClockSource->addItem("ExtN","ExtN");
    ui->comboClockSource->addItem("ExtS","ExtS");
    ui->comboClockSource->addItem("42000000","42000000");
    ui->comboClockSource->addItem("10500000","10500000");
    ui->comboClockSource->addItem("2625000","2625000");
    ui->comboClockSource->addItem("656250","656250");
    ui->comboTriggerSource->clear();
    ui->comboTriggerSource->addItem("Software","Software");
    ui->comboTriggerSource->addItem("Edge","Edge");
    ui->comboTriggerSource->addItem("Pos","Pos");
    ui->comboTriggerSource->addItem("Neg","Neg");
    ui->comboEnable->clear();
    ui->comboEnable->addItem("","");
    ui->comboEnable->addItem("A","A");
    ui->comboEnable->addItem("B","B");
    ui->comboEnable->addItem("C","C");
    ui->comboEnable->addItem("D","D");
    ui->comboSelectEvent->clear();
    ui->comboSelectEvent->addItem("");
    ui->comboSelectEvent->addItem("New event");
    ui->comboSelectEvent->addItem("Rename event");
    ui->comboSelectEvent->addItem("Delete current");
    ui->comboEventSignal->clear();
    ui->comboEventSignal->addItem("","");

    connect(ui->comboSelectEvent, &QComboBox::currentIndexChanged, this, [this](int){ slotEventChange(); });
    connect(ui->leEventStart, &QLineEdit::editingFinished, this, &TimingGenerator::slotEventUpdated);
    connect(ui->leEventWidth, &QLineEdit::editingFinished, this, &TimingGenerator::slotEventUpdated);
    connect(ui->leEventValue, &QLineEdit::editingFinished, this, &TimingGenerator::slotEventUpdated);
    connect(ui->leEventValueOff, &QLineEdit::editingFinished, this, &TimingGenerator::slotEventUpdated);
    connect(ui->comboEventSignal, &QComboBox::currentIndexChanged, this, [this](int){ slotEventUpdated(); });
    connect(ui->pbGenerate, &QPushButton::pressed, this, &TimingGenerator::slotGenerate);
    connect(ui->pbClearEvents, &QPushButton::pressed, this, &TimingGenerator::slotClearEvents);
    connect(ui->pbLoad, &QPushButton::pressed, this, &TimingGenerator::slotLoad);
    connect(ui->pbSave, &QPushButton::pressed, this, &TimingGenerator::slotSave);

    // The following events will signal a new table needs to be generated so the slot
    // will clear the table field
    connect(ui->comboMuxOrder, &QComboBox::currentIndexChanged, this, [this](int){ slotClearTable(); });
    connect(ui->comboClockSource, &QComboBox::currentIndexChanged, this, [this](int){ slotClearTable(); });
    connect(ui->comboTriggerSource, &QComboBox::currentIndexChanged, this, [this](int){ slotClearTable(); });
    connect(ui->chkTimeMode, &QCheckBox::clicked, this, &TimingGenerator::slotClearTable);
}

/*! \brief TimingGenerator::~TimingGenerator
 * Destructor. Sends the table-abort command and deletes all Event and EventControl objects.
 */
TimingGenerator::~TimingGenerator()
{
    delete ui;
}

// Called after table is downloaded to MIPS. This function will
// update all the event control parameters and connect to the
// pulse sequence events.
/*! \brief TimingGenerator::UpdateEvents
 * Rebuilds the event-control widgets from the Events list and wires their change signals.
 */
void TimingGenerator::UpdateEvents(void)
{
    QStringList resList;

    foreach(EventControl *ec, EC)
    {
        resList = ec->ECname.split(".");
        ec->comms = comms;
        if(resList.count() == 2) foreach(Event *evt, Events)
        {
            if(resList[0].toUpper()==evt->Name.toUpper())
            {
                if((resList[1].toUpper()=="VALUE") && (!evt->Channel.isEmpty()))
                {
                    if((evt->Width.toUpper() != "INIT") && (evt->Width.toUpper() != "RAMP"))
                    {
                        ec->Scommand = "STBLVLT,"+QString::number((int)evt->StartT)+","+evt->Channel;
                        ec->Gcommand = "GTBLVLT,"+QString::number((int)evt->StartT)+","+evt->Channel;
                                        }
                    ec->EventValue->setText(QString::number(evt->Value));
                }
                else if((resList[1].toUpper()=="VALUEOFF") && (!evt->Channel.isEmpty()))
                {
                    if((evt->Width.toUpper() != "INIT") && (evt->Width.toUpper() != "RAMP"))
                    {
                        ec->Scommand = "STBLVLT,"+QString::number((int)(evt->StartT+evt->WidthT))+","+evt->Channel;
                        ec->Gcommand = "GTBLVLT,"+QString::number((int)(evt->StartT+evt->WidthT))+","+evt->Channel;
                    }
                    ec->EventValue->setText(QString::number(evt->ValueOff));
                }
                else
                {
                    ec->Scommand.clear();
                    ec->Gcommand.clear();
                }
            }
        }
        else
        {
            ec->Scommand.clear();
            ec->Gcommand.clear();
        }
    }
}

// Reports all the setting to be saved in methods file or sequence file
/*! \brief TimingGenerator::Report
 * Serialises all timing parameters and events to a newline-separated string for save/restore.
 */
QString TimingGenerator::Report(void)
{
    QString res;

    res.clear();
    // Report all of the events;
    foreach(Event *evt, Events)
    {
        res += "TGevent," + Title + "," + evt->Name + "," + evt->Channel + ",";
        res += evt->Start + "," + evt->Width + ",";
        res += QString::number(evt->Value) + "," + QString::number(evt->ValueOff) + "\n";
    }
    // Frame parameters
    res += "TGframe," + Title + "," + ui->leFrameStart->text() + "," + ui->leFrameWidth->text() + ",";
    res += ui->leAccumulations->text() + "," + ui->comboEnable->currentText() + "\n";
    // Clock and trigger settings
    res += "TGclock," + Title + "," + ui->comboClockSource->currentText() + "\n";
    res += "TGtrigger," + Title + "," + ui->comboTriggerSource->currentText() + "\n";
    // Table
    res += "TGmux," + Title + "," + ui->comboMuxOrder->currentText() + "\n";
    res += "TGtable," + Title + "," + ui->leTable->text() + "\n";
    // External clock
    res += "ExtClock," + Title + "," + ui->leExtClkFreq->text() + "\n";
    // Time mode
    if(ui->chkTimeMode->isChecked()) res += "TimeMode," + Title + ",TRUE\n";
    else res += "TimeMode," + Title + ",FALSE\n";
    return res;
}

/*! \brief TimingGenerator::SetValues
 * Restores timing parameters and events from a save-file string.
 */
bool TimingGenerator::SetValues(QString strVals)
{
    QStringList resList;

    if(strVals.startsWith("TGevent," + Title + ","))
    {
       resList = strVals.split(",");
       if(resList.count() != 8) return false;
       Event *event = new Event();
       event->Name = resList[2];
       event->Channel = resList[3];
       event->Start = resList[4];
       event->Width = resList[5];
       event->Value = resList[6].toFloat();
       event->ValueOff = resList[7].toFloat();
       Events.append(event);
       if(Events.count()==1)
       {
           disconnect(ui->comboSelectEvent, &QComboBox::currentIndexChanged, nullptr, nullptr);
           ui->comboSelectEvent->clear();
           ui->comboSelectEvent->addItem("");
           ui->comboSelectEvent->addItem("New event");
           ui->comboSelectEvent->addItem("Rename event");
           ui->comboSelectEvent->addItem("Delete current");
           connect(ui->comboSelectEvent, &QComboBox::currentIndexChanged, this, [this](int){ slotEventChange(); });
       }
       ui->comboSelectEvent->addItem(resList[2]);
       return true;
    }
    if(strVals.startsWith("TGframe," + Title + ","))
    {
        resList = strVals.split(",");
        if(resList.count() != 6) return false;
        ui->leFrameStart->setText(resList[2]);
        ui->leFrameWidth->setText(resList[3]);
        ui->leAccumulations->setText(resList[4]);
        ui->comboEnable->setCurrentIndex(ui->comboEnable->findText(resList[5]));
        return true;
    }
    if(strVals.startsWith("TGclock," + Title + ","))
    {
        resList = strVals.split(",");
        if(resList.count() != 3) return false;
        ui->comboClockSource->setCurrentIndex(ui->comboClockSource->findText(resList[2]));
        return true;
    }
    if(strVals.startsWith("TGtrigger," + Title + ","))
    {
        resList = strVals.split(",");
        if(resList.count() != 3) return false;
        ui->comboTriggerSource->setCurrentIndex(ui->comboTriggerSource->findText(resList[2]));
        return true;
    }
    if(strVals.startsWith("TGmux," + Title + ","))
    {
        resList = strVals.split(",");
        if(resList.count() != 3) return false;
        ui->comboMuxOrder->setCurrentIndex(ui->comboMuxOrder->findText(resList[2]));
        return true;
    }
    if(strVals.startsWith("TGtable," + Title + ","))
    {
        int i = strVals.indexOf("STBLDAT");
        if(i > 0) ui->leTable->setText(strVals.mid(i));
        return true;
    }
    if(strVals.startsWith("ExtClock," + Title + ","))
    {
        resList = strVals.split(",");
        if(resList.count() != 3) return false;
        ui->leExtClkFreq->setText(resList[2]);
        return true;
    }
    if(strVals.startsWith("TimeMode," + Title + ","))
    {
        resList = strVals.split(",");
        if(resList.count() != 3) return false;
        if(resList[2] == "TRUE") ui->chkTimeMode->setChecked(true);
        if(resList[2] == "FALSE") ui->chkTimeMode->setChecked(false);
        return true;
    }
    return false;
}

/*! \brief TimingGenerator::isTableMode
 * Returns true if the MIPS system is currently in table mode.
 */
bool TimingGenerator::isTableMode(void)
{
    return isTblMode(comms,ui->comboTriggerSource->currentText());
}

/*! \brief TimingGenerator::AddSignal
 * Adds a signal name/channel pair to the signal combo box.
 */
void TimingGenerator::AddSignal(QString title, QString chan)
{
    ui->comboEventSignal->addItem(title,chan);
}

/*! \brief TimingGenerator::Split
 * Splits a string by a delimiter, respecting quoted sub-strings.
 */
QStringList TimingGenerator::Split(QString str, QString del)
{
    QString     s;
    QStringList reslist;

    reslist.clear();
    s.clear();
    for(int i= 0;i<str.length();i++)
    {
        if(str.mid(i,1) == " ") continue;
        if(del.contains(str.mid(i,1)))
        {
            if(!s.isEmpty()) reslist.append(s);
            s.clear();
            reslist.append(str.mid(i,1));
        }
        else s += QStringView{str}.mid(i,1);
    }
    if(!s.isEmpty()) reslist.append(s);
    return(reslist);
}

/*! \brief TimingGenerator::ConvertToCount
 * Converts a time string (µs or ms) to a MIPS clock-count integer.
 */
int TimingGenerator::ConvertToCount(QString val)
{
    bool  ok;
    float clock  = 0;
    float result;

    result = CalculateTime(val);
    if(ui->chkTimeMode->isChecked())
    {
        clock = ui->comboClockSource->currentText().toInt(&ok);
        if(ok) result = result * (clock/1000.0);
        else
        {
            clock = ui->leExtClkFreq->text().toInt(&ok);
            if(ok) result = result * (clock/1000.0);
        }
    }
    return result;
}

/*! \brief TimingGenerator::CalculateTime
 * Converts a time string to a float value in seconds.
 */
float TimingGenerator::CalculateTime(QString val)
{
    QStringList reslist;
    bool  ok;
    float result = 0;
    float j      = 0;
    int   sign   = 1;

    reslist = Split(val,"+-");
    for(int i=0;i<reslist.count();i++)
    {
        j = reslist[i].toFloat(&ok);
        if(ok) result += sign * j;
        else if(reslist[i] == "+") sign =  1;
        else if(reslist[i] == "-") sign = -1;
        else
        {
            foreach(Event *evt, Events)
            {
                QString evtName = evt->Name;
                evtName.replace(" ", "");
                if(evtName == reslist[i])
                {
                    result += sign * CalculateTime(evt->Start);
                    break;
                }
                else if((evtName + ".Start") == reslist[i])
                {
                    result += sign * CalculateTime(evt->Start);
                    break;
                }
                else if((evtName + ".Width") == reslist[i])
                {
                    result += sign * CalculateTime(evt->Width);
                    break;
                }
            }
        }
    }
    return(result);
}

/*! \brief TimingGenerator::GenerateMuxSeq
 * Generates the MUX sequencer command string from the event table.
 */
QString TimingGenerator::GenerateMuxSeq(QString Seq)
{
    int maxCount = ConvertToCount(ui->leFrameWidth->text()) + ConvertToCount(ui->leFrameStart->text());
    bool timeFlag;
    QString table;
    QList<int> InjectionPoints;

    if(properties != nullptr) properties->Log("Mux seq: " + Seq);
    int l = Seq.length();
    if(l <= 0) return "";
    // Look for 1s in the sequence and build a list of
    // TOF counts for injections
    for(int i=0;i<l;i++) if(Seq[i] == '1') InjectionPoints.append((i * ConvertToCount(ui->leFrameWidth->text()) / (l))  + ConvertToCount(ui->leFrameStart->text()));
    slotEventUpdated();
    table.clear();
    if(ui->comboTriggerSource->currentText() == "Software") table = "STBLDAT;0:[A:" + QString::number(ui->leAccumulations->text().toInt() + FrameCtAdj);
    else table = "STBLDAT;0:[A:" + QString::number(ui->leAccumulations->text().toInt());
    foreach(Event *evt, Events)
    {
        if(evt->Channel.isEmpty()) continue;
        evt->StartT = ConvertToCount(evt->Start);
        evt->WidthT = ConvertToCount(evt->Width);
    }
    // Do some validation tests, make sure the required events are defined, also make sure the sum of
    // fill time, trap time, and release time will fit between the two closest injection points
    int events_found=0;
    int TotalInjectEventTime = 0;
    int InjectTime = 0;
    foreach(Event *evt, Events)
    {
        if(evt->Name == "Fill time") {events_found++; TotalInjectEventTime += ConvertToCount(evt->Width);}
        if(evt->Name == "Trap time") {events_found++; TotalInjectEventTime += ConvertToCount(evt->Width);}
        if(evt->Name == "Release time") {events_found++; TotalInjectEventTime += ConvertToCount(evt->Width);}
        if(evt->Name == "Inject time") {events_found++; InjectTime = ConvertToCount(evt->Start);}
    }
    if((events_found != 4) || (maxCount/l < TotalInjectEventTime) || (InjectTime != 0))
    {
        QMessageBox msgBox;
        QString msg = "Invalid pulse sequence! The following requirements\n";
        msg +=        "must be met:\n";
        msg +=        "  The following events must be defined:\n";
        msg +=        "       Fill Time, \n";
        msg +=        "       Trap time, \n";
        msg +=        "       Release time, \n";
        msg +=        "       Inject time, \n";
        msg +=        "  Inject time Start must equal 0.\n";
        msg +=        "  Fill time, Trap time, and Release time are relative\n";
        msg +=        "  to Inject time.\n";
        msg +=        "  The sum of Fill time, Trap time, and release time\n";
        msg +=        "  widths must be less than\n";
        msg +=        "  frame width / 2^(mux order -1).\n";
        msgBox.setText(msg);
        msgBox.exec();
        return "";
    }
    // End of validation testing
    int FrameStartT = ConvertToCount(ui->leFrameStart->text());
    for(int i=0;i <= maxCount;i++)
    {
        timeFlag = false;
        // Search for event at this clock cycle
        foreach(Event *evt, Events)
        {
            if(evt->Channel.isEmpty()) continue;
            int et = evt->StartT;
            bool Matched = false;
            if((evt->Name == "Fill time") || (evt->Name == "Trap time") || (evt->Name == "Release time"))
            {
                foreach(int it, InjectionPoints) if((et + it) == i) Matched = true;
            }
            else if(et == i) Matched = true;
            if(Matched)
            {
                if(!timeFlag) { table += "," + QString::number(i); timeFlag=true; }
                table += ":" + evt->Channel + ":" + QString::number(evt->Value);
            }
            et = evt->StartT + evt->WidthT;
            Matched = false;
            if((evt->Name == "Fill time") || (evt->Name == "Trap time") || (evt->Name == "Release time"))
            {
                foreach(int it, InjectionPoints) if((et + it) == i) Matched = true;
            }
            else if(et == i) Matched = true;
            if(Matched)
            {
                if(evt->WidthT > 0)
                {
                   if(!timeFlag) { table += "," + QString::number(i); timeFlag=true; }
                   table += ":" + evt->Channel + ":" + QString::number(evt->ValueOff);
                }
            }
        }
        if(FrameStartT == i)
        {
            if(ui->comboEnable->currentText() != "")
            {
                if(!timeFlag) { table += "," + QString::number(i); timeFlag=true; }
                table += ":" + ui->comboEnable->currentText() + ":1";
            }
        }
        if(maxCount == i)
        {
            if(!timeFlag) { table += "," + QString::number(i); }  // timeFlag=true; }
            if(ui->comboEnable->currentText() != "")
            {
               table += ":" +  ui->comboEnable->currentText() + ":0";
            }
            table += ":];";
        }
    }
    return table;
}

/*! \brief TimingGenerator::slotGenerate
 * Slot: builds the timing table from the event list and downloads it to MIPS.
 */
void  TimingGenerator::slotGenerate(void)
{
    int maxCount = ConvertToCount(ui->leFrameWidth->text()) + ConvertToCount(ui->leFrameStart->text());
    int maxEventCount=0;
    bool timeFlag;
    QString table;
    Event revt;

    revt.Name.clear();
    if(ui->comboMuxOrder->currentText() == "4 Bit") ui->leTable->setText(GenerateMuxSeq("000100110101111"));
    else if(ui->comboMuxOrder->currentText() == "5 Bit") ui->leTable->setText(GenerateMuxSeq("0000100101100111110001101110101"));
    else if(ui->comboMuxOrder->currentText() == "6 Bit") ui->leTable->setText(GenerateMuxSeq("000001000011000101001111010001110010010110111011001101010111111"));
    else if(ui->comboMuxOrder->currentText() == "7 Bit") ui->leTable->setText(GenerateMuxSeq("0000001000001100001010001111001000101100111010100111110100001110001001001101101011011110110001101001011101110011001010101111111"));
    else if(ui->comboMuxOrder->currentText() == "8 Bit") ui->leTable->setText(GenerateMuxSeq("000000010111000111011110001011001101100001111001110000101011111111001011110100101000011011101101111101011101000001100101010100011010110001100000100101101101010011010011111101110011001111011001000010000001110010010011000100111010101101000100010100100011111"));
    else if(ui->comboMuxOrder->currentText() == "9 Bit") ui->leTable->setText(GenerateMuxSeq("0000000010000100011000010011100101010110000110111101001101110010001010000101011010011111101100100100101101111110010011010100110011000000011000110010100011010010111111101000101100011101011001011001111000111110111010000011010110110111011000001011010111110101010100000010100101011110010111011100000011100111010010011110101110101000100100001100111000010111101101100110100001110111100001111111110000011110111110001011100110010000010010100111011010001111001111100110110001010100100011100011011010101110001001100010001"));
    if(ui->comboMuxOrder->currentText() != "None") return;
    slotEventUpdated();
    table.clear();
    if(ui->comboTriggerSource->currentText() == "Software") table = "STBLDAT;0:[A:" + QString::number(ui->leAccumulations->text().toInt() + FrameCtAdj);
    else table = "STBLDAT;0:[A:" + QString::number(ui->leAccumulations->text().toInt());
    foreach(Event *evt, Events)
    {
        // If this is a Repeat event then make a copy
        if(evt->Name == "Repeat")
        {
            revt = *evt;
            evt->StartT = evt->Start.toInt();
            evt->WidthT = evt->Width.toInt();
        }
        if(evt->Channel.isEmpty()) continue;
        evt->StartT = ConvertToCount(evt->Start);
        evt->WidthT = ConvertToCount(evt->Width);
        if((abs(evt->StartT) + abs(evt->WidthT)) > maxEventCount) maxEventCount = abs(evt->StartT) + abs(evt->WidthT);
    }
    int FrameStartT = ConvertToCount(ui->leFrameStart->text());
    for(int i=0;i <= abs(maxCount);i++)
    {

        if((revt.Name == "Repeat") && (i>0) && (revt.Width.toInt()>0))
        {
            if((i % int(ConvertToCount(revt.Width))) == 0)
            {
                foreach(Event *evt, Events) if(evt->Name == "Repeat")
                {
                    if(ConvertToCount(QString::number(evt->StartT + evt->WidthT)) < abs(maxCount))
                    {
                        evt->StartT += evt->WidthT;
                        evt->Start = QString::number(evt->StartT);
                    }
                }
                foreach(Event *evt, Events)
                {
                    if(evt->Channel.isEmpty()) continue;
                    evt->StartT = ConvertToCount(evt->Start);
                    evt->WidthT = ConvertToCount(evt->Width);
                }
            }
        }

        timeFlag = false;
        // Search for event at this clock cycle
        foreach(Event *evt, Events)
        {
            if(evt->Channel.isEmpty()) continue;
            QString Chan = evt->Channel;
            if(evt->Width.toUpper() == "INIT")
            {
                Chan = QString::number(Chan.toInt() + 128 + 64);
            }
            if(evt->Width.toUpper() == "RAMP")
            {
                Chan = QString::number(Chan.toInt() + 128);
            }
            if((int)abs(evt->StartT) == i)
            {
                if(!timeFlag) { table += "," + QString::number((int)evt->StartT); timeFlag=true; }
                if(Chan == "c")
                {
                    if(evt->Signal == "ARBct") table += ":" + Chan + ":A";
                    else table += ":" + Chan + ":T";
                }
                else table += ":" + Chan + ":" + QString::number(evt->Value);
            }
            if((int)abs(evt->StartT + evt->WidthT) == i)
            {
                if(abs(evt->WidthT) > 0)
                {
                   if(!timeFlag) { table += "," + QString::number((int)(evt->StartT + evt->WidthT)); timeFlag=true; }
                   table += ":" + Chan + ":" + QString::number(evt->ValueOff);
                }
            }
        }
        if((int)abs(FrameStartT) == i)
        {
            if(ui->comboEnable->currentText() != "")
            {
                if(!timeFlag) { table += "," + QString::number((int)FrameStartT); timeFlag=true; }
                table += ":" + ui->comboEnable->currentText() + ":1";
            }
        }
        if((abs(maxCount) == i) || ((maxEventCount < i) && (revt.Name != "Repeat")))
        {
            if(!timeFlag) { table += "," + QString::number((int)maxCount); } //timeFlag=true; }
            if(ui->comboEnable->currentText() != "")
            {
               table += ":" +  ui->comboEnable->currentText() + ":0";
            }
            table += ":];";
            break;
        }
    }
    ui->leTable->setText(table);
    if(revt.Name == "Repeat")
    {
        foreach(Event *evt, Events) if(evt->Name == "Repeat") *evt = revt;
    }
}

/*! \brief TimingGenerator::slotEventUpdated
 * Slot: updates the selected event from the UI fields and refreshes the event list.
 */
void TimingGenerator::slotEventUpdated(void)
{
    if(selectedEvent == nullptr) return;
    selectedEvent->Name = ui->leEventName->text();
    selectedEvent->Signal = ui->comboEventSignal->currentText();
    selectedEvent->Channel = ui->comboEventSignal->currentData().toString();
    selectedEvent->Start = ui->leEventStart->text();
    selectedEvent->Width = ui->leEventWidth->text();
    selectedEvent->Value = ui->leEventValue->text().toFloat();
    selectedEvent->ValueOff = ui->leEventValueOff->text().toFloat();
    if(!scriptName.isEmpty()) emit change(scriptName);
}

/*! \brief TimingGenerator::slotEventChange
 * Slot: loads the selected event's parameters into the UI fields.
 */
void TimingGenerator::slotEventChange(void)
{
    bool ok;
    QString text;

    if(ui->comboSelectEvent->currentText() == "New event")
    {
        while(true)
        {
            text = QInputDialog::getText(0, "New event", "Enter event name, must be unique:", QLineEdit::Normal,"", &ok );
            if (ok && !text.isEmpty() )
            {
                if(ui->comboSelectEvent->findText(text) >= 0)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Name must be unique, try again!");
                    msgBox.exec();
                }
                else break;
            }
            else
            {
                selectedEvent = nullptr;
                ui->comboSelectEvent->setCurrentIndex(0);
                return;
            }
        }
        selectedEvent = nullptr;
        Event *event = new Event();
        event->Name = text;
        ui->comboEventSignal->setCurrentIndex(0);
        event->Channel = ui->comboEventSignal->currentData().toString();
        event->Start = "0";
        event->Width = "10";
        event->Value = 0;
        ui->leEventName->setText(event->Name);
        ui->leEventStart->setText(event->Start);
        ui->leEventWidth->setText(event->Width);
        ui->leEventValue->setText(QString::number(event->Value));
        ui->leEventValueOff->setText(QString::number(event->ValueOff));
        Events.append(event);
        ui->comboSelectEvent->addItem(text);
        int i = (ui->comboSelectEvent->findText(text));
        ui->comboSelectEvent->setCurrentIndex(i);
        selectedEvent = event;
    }
    else if(ui->comboSelectEvent->currentText() == "Rename event")
    {
       if(!ui->leEventName->text().isEmpty())
       {
           while(true)
           {
               text = QInputDialog::getText(0, "Rename event", "Enter new event name, must be unique:", QLineEdit::Normal,"", &ok );
               if (ok && !text.isEmpty() )
               {
                   if(ui->comboSelectEvent->findText(text) >= 0)
                   {
                       QMessageBox msgBox;
                       msgBox.setText("Name must be unique, try again!");
                       msgBox.exec();
                   }
                   else break;
               }
               else return;
           }
           // Rename the event if here
           ui->comboSelectEvent->removeItem(ui->comboSelectEvent->findText(ui->leEventName->text()));
                          ui->comboSelectEvent->addItem(text);
           ui->leEventName->setText(text);
           slotEventUpdated();
           ui->comboSelectEvent->setCurrentIndex(0);
           ui->leEventName->setText("");
           ui->comboEventSignal->setCurrentIndex(0);
           ui->leEventStart->setText("");
           ui->leEventWidth->setText("");
           ui->leEventValue->setText("");
           ui->leEventValueOff->setText("");
           selectedEvent = nullptr;
       }
    }
    else if(ui->comboSelectEvent->currentText() == "Delete current")
    {
        int   i;

        if((i=ui->comboSelectEvent->findText(ui->leEventName->text())) >= 3)
        {
            // Find in Event list and remove
            foreach(Event *evt, Events) if(evt->Name == ui->leEventName->text()) Events.removeOne(evt);
            ui->comboSelectEvent->removeItem(i);
            ui->comboSelectEvent->setCurrentIndex(0);
            ui->leEventName->setText("");
            ui->comboEventSignal->setCurrentIndex(0);
            ui->leEventStart->setText("");
            ui->leEventWidth->setText("");
            ui->leEventValue->setText("");
            ui->leEventValueOff->setText("");
            selectedEvent = nullptr;
        }
    }
    else
    {
        // Find the event entry in list and update the controls
        selectedEvent = nullptr;
        foreach(Event *evt, Events) if(evt->Name == ui->comboSelectEvent->currentText()) selectedEvent = evt;
        if(selectedEvent == nullptr)
        {
            ui->leEventName->clear();
            ui->leEventStart->clear();
            ui->leEventWidth->clear();
            ui->leEventValue->clear();
            ui->leEventValueOff->clear();
            ui->comboEventSignal->setCurrentIndex(0);
            return;
        }
        ui->leEventName->setText(selectedEvent->Name);
        ui->leEventStart->setText(selectedEvent->Start);
        ui->leEventWidth->setText(selectedEvent->Width);
        ui->leEventValue->setText(QString::number(selectedEvent->Value));
        ui->leEventValueOff->setText(QString::number(selectedEvent->ValueOff));
        int i = ui->comboEventSignal->findData(selectedEvent->Channel);
            ui->comboEventSignal->setCurrentIndex(i);
    }
}

/*! \brief TimingGenerator::ProcessCommand
 * Reads or writes timing parameters in response to a command string from the scripting system.
 */
QString TimingGenerator::ProcessCommand(QString cmd)
{
    QLineEdit    *le    = nullptr;
    QRadioButton *rb    = nullptr;
    QComboBox    *combo = nullptr;
    QPushButton  *pb    = nullptr;
    QCheckBox    *chk   = nullptr;

    if(!cmd.startsWith(Title)) return "?";
    if(cmd.trimmed() == Title + ".isTblMode")
    {
        if(isTableMode()) return("TRUE");
        else return("FALSE");
    }
    // Handle the Value,off case, replace Value,off with Value off
    cmd.replace("Value,off", "Value off");
    // Handle the Time mode, in mS case, replace Time mode, in mS with Time mode in mS
    cmd.replace("Time mode, in mS", "Time mode in mS");
    //QRegularExpression rx("(\,|\=)"); //RegEx for ',' or '='
    static QRegularExpression rx(QStringLiteral(",|=")); //RegEx for ',' or '='
    QStringList resList = cmd.split(rx);
    resList[0].replace("Value off", "Value,off");
    resList[0].replace("Time mode in mS", "Time mode, in mS");
    if(resList[0].trimmed() == Title + ".script")
    {
        if(resList.count() == 1) return scriptName;
        scriptName = resList[1].trimmed();
        return "";
    }
    if(resList[0].trimmed() == Title + ".Event.StartT")
    {
        for(int i=0;i<Events.count();i++)
        {
            if(Events[i]->Name == resList[1].trimmed()) return(QString::number(Events[i]->StartT));
        }
        return("?");
    }
    if(resList[0].trimmed() == Title + ".Event.WidthT")
    {
        for(int i=0;i<Events.count();i++)
        {
            if(Events[i]->Name == resList[1].trimmed()) return(QString::number(Events[i]->WidthT));
        }
        return("?");
    }
    if(resList[0].trimmed() == Title + ".Event.Channel")
    {
        for(int i=0;i<Events.count();i++) if(Events[i]->Name == resList[1].trimmed()) return(Events[i]->Channel);
        return("?");
    }
    if(resList[0].trimmed() == Title + ".Frame.Start") le = ui->leFrameStart;
    else if(resList[0].trimmed() == Title + ".Frame.Width") le = ui->leFrameWidth;
    else if(resList[0].trimmed() == Title + ".Frame.Accumulations") le = ui->leAccumulations;
    else if(resList[0].trimmed() == Title + ".Table") le = ui->leTable;
    else if(resList[0].trimmed() == Title + ".Event.Start") le = ui->leEventStart;
    else if(resList[0].trimmed() == Title + ".Event.Width") le = ui->leEventWidth;
    else if(resList[0].trimmed() == Title + ".Event.Value") le = ui->leEventValue;
    else if(resList[0].trimmed() == Title + ".Event.Value,off") le = ui->leEventValueOff;
    else if(resList[0].trimmed() == Title + ".Clock source") combo = ui->comboClockSource;
    else if(resList[0].trimmed() == Title + ".Trigger source") combo = ui->comboTriggerSource;
    else if(resList[0].trimmed() == Title + ".Mux order") combo = ui->comboMuxOrder;
    else if(resList[0].trimmed() == Title + ".Ext Clock Freq") le = ui->leExtClkFreq;
    else if(resList[0].trimmed() == Title + ".Time mode, in mS") chk = ui->chkTimeMode;
    else if(resList[0].trimmed() == Title + ".Frame.Enable") combo = ui->comboEnable;
    else if(resList[0].trimmed() == Title + ".Event.Signal") combo = ui->comboEventSignal;
    else if(resList[0].trimmed() == Title + ".Event.Select event") combo = ui->comboSelectEvent;
    else if(resList[0].trimmed() == Title + ".Generate") pb = ui->pbGenerate;
    else if(resList[0].trimmed() == Title + ".Trigger") pb = Trigger;
    else if(resList[0].trimmed() == Title + ".Abort") pb = Abort;
    else if(resList[0].trimmed() == Title + ".Edit") pb = Edit;
    if(le != nullptr)
    {
       if(resList.count() == 1) return le->text();
       le->setText(resList[1]);
       le->setModified(true);
       emit le->editingFinished();
       return "";
    }
    if(rb != nullptr)
    {
        if(resList.count() == 1) { if(rb->isChecked()) return "TRUE"; else return "FALSE"; }
        if(resList[1].trimmed() == "TRUE") rb->setChecked(true);
        if(resList[1].trimmed() == "FALSE") rb->setChecked(false);
        emit rb->clicked();
        return "";
    }
    if(chk != nullptr)
    {
        if(resList.count() == 1) { if(chk->isChecked()) return "TRUE"; else return "FALSE"; }
        if(resList[1].trimmed() == "TRUE") chk->setChecked(true);
        if(resList[1].trimmed() == "FALSE") chk->setChecked(false);
        emit chk->clicked();
        return "";
    }
    if(combo != nullptr)
    {
       if(resList.count() == 1) return combo->currentText();
       int i = combo->findText(resList[1].trimmed());
       if(i<0) return "?";
       combo->setCurrentIndex(i);
       return "";
    }
    if(pb != nullptr)
    {
        emit pb->pressed();
        return "";
    }
    return "?";
}

/*! \brief TimingGenerator::slotClearEvents
 * Slot: clears all events from the event list.
 */
void TimingGenerator::slotClearEvents(void)
{
   ui->pbClearEvents->setDown(false);
   Events.clear();
   ui->comboSelectEvent->clear();
   ui->comboSelectEvent->addItem("");
   ui->comboSelectEvent->addItem("New event");
   ui->comboSelectEvent->addItem("Rename event");
   ui->comboSelectEvent->addItem("Delete current");
}

/*! \brief TimingGenerator::slotLoad
 * Slot: loads timing parameters and events from a user-selected file.
 */
void TimingGenerator::slotLoad(void)
{
    ui->pbLoad->setDown(false);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Sequence from File"),"",tr("Sequence (*.seq);;All files (*.*)"));
    if(fileName == "") return;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        // We're going to streaming the file
        // to a QString
        QTextStream stream(&file);
        QString line;
        Events.clear();
        do
        {
            line = stream.readLine();
            SetValues(line);
        } while(!line.isNull());
    }
    file.close();
    ui->leEventName->clear();
    ui->leEventStart->clear();
    ui->leEventWidth->clear();
    ui->leEventValue->clear();
    ui->leEventValueOff->clear();
    ui->comboEventSignal->setCurrentIndex(0);
}

/*! \brief TimingGenerator::slotSave
 * Slot: saves timing parameters and events to a user-selected file.
 */
void TimingGenerator::slotSave(void)
{
    ui->pbSave->setDown(false);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save to Sequence File"),"",tr("Sequence (*.seq);;All files (*.*)"));
    if(fileName == "") return;
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // We're going to streaming text to the file
        QTextStream stream(&file);
        QDateTime dateTime = QDateTime::currentDateTime();
        stream << "# Timing sequence, " + dateTime.toString() + "\n";
        stream << Report() + "\n";
        file.close();
    }
}

/*! \brief TimingGenerator::slotClearTable
 * Slot: marks the downloaded table as stale when a source/mode combo changes.
 */
void TimingGenerator::slotClearTable(void)
{
    ui->leTable->clear();
}
