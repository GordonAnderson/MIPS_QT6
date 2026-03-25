// =============================================================================
// dio.cpp
//
// Implements two classes for Digital I/O interaction:
//
//   DIO         — controls the Digital IO tab. Manages digital output
//                 checkboxes, trigger output, frequency generator, and
//                 remote navigation of the MIPS front panel display.
//
//   DIOchannel  — control panel widget for a single DIO channel. Renders
//                 as a checkbox; channels A–P are read/write, channels above
//                 P are read-only inputs.
//
// Hardware:    MIPS DIO module (commands: GDIO, SDIO, TRIGOUT, BURST,
//              SSERIALNAV)
// Depends on:  comms.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "dio.h"
#include "Utilities.h"

// ASCII control codes sent to MIPS for front-panel remote navigation
static const char NAV_SMALL_UP   = 30;
static const char NAV_LARGE_UP   = 31;
static const char NAV_SMALL_DOWN = 28;
static const char NAV_LARGE_DOWN = 29;
static const char NAV_SELECT     =  9;

// Unicode arrow characters for the navigation push buttons
static const QChar ARROW_UP_SMALL(0x25B4);
static const QChar ARROW_UP_LARGE(0x25B2);
static const QChar ARROW_DOWN_SMALL(0x25BE);
static const QChar ARROW_DOWN_LARGE(0x25BC);

// -----------------------------------------------------------------------------
// DIO constructor — sets navigation button labels and connects all UI signals.
// -----------------------------------------------------------------------------
DIO::DIO(Ui::MIPS *w, Comms *c)
{
    dui = w;
    comms = c;

    dui->pbUPsmall->setText(ARROW_UP_SMALL);
    dui->pbUPlarge->setText(ARROW_UP_LARGE);
    dui->pbDOWNsmall->setText(ARROW_DOWN_SMALL);
    dui->pbDOWNlarge->setText(ARROW_DOWN_LARGE);

    QObjectList widgetList = dui->gbDigitalOut->children();
    widgetList += dui->gbDigitalIn->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("chk"))
        {
            connect(((QCheckBox *)w),SIGNAL(stateChanged(int)),this,SLOT(DOUpdated()));
        }
    }
    connect(dui->pbDIOupdate,   SIGNAL(pressed()),                        this, SLOT(UpdateDIO()));
    connect(dui->pbTrigHigh,    SIGNAL(pressed()),                        this, SLOT(TrigHigh()));
    connect(dui->pbTrigLow,     SIGNAL(pressed()),                        this, SLOT(TrigLow()));
    connect(dui->pbTrigPulse,   SIGNAL(pressed()),                        this, SLOT(TrigPulse()));
    connect(dui->pbRFgenerate,  SIGNAL(pressed()),                        this, SLOT(RFgenerate()));
    connect(dui->leSFREQ,       SIGNAL(editingFinished()),                this, SLOT(SetFreq()));
    connect(dui->leSWIDTH,      SIGNAL(editingFinished()),                this, SLOT(SetWidth()));
    connect(dui->chkRemoteNav,  SIGNAL(checkStateChanged(Qt::CheckState)),this, SLOT(RemoteNavigation()));
    connect(dui->pbUPsmall,     SIGNAL(pressed()),                        this, SLOT(RemoteNavSmallUP()));
    connect(dui->pbUPlarge,     SIGNAL(pressed()),                        this, SLOT(RemoteNavLargeUP()));
    connect(dui->pbDOWNsmall,   SIGNAL(pressed()),                        this, SLOT(RemoteNavSmallDown()));
    connect(dui->pbDOWNlarge,   SIGNAL(pressed()),                        this, SLOT(RemoteNavLargeDown()));
    connect(dui->pbSelect,      SIGNAL(pressed()),                        this, SLOT(RemoteNavSelect()));
}

// -----------------------------------------------------------------------------
// Update — polls all digital output checkboxes and frequency generator fields
// from hardware. Blocks checkbox signals during update to prevent feedback.
// -----------------------------------------------------------------------------
void DIO::Update(void)
{
    QString res;

    dui->tabMIPS->setEnabled(false);
    dui->statusBar->showMessage(tr("Updating Digital IO controls..."));
    QObjectList widgetList = dui->gbDigitalOut->children();
    widgetList += dui->gbFreqGen->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("chk"))
        {
            res = "G" + w->objectName().mid(4).replace("_",",") + "\n";
            bool oldState = ((QCheckBox *)w)->blockSignals(true);
            if(comms->SendMess(res).toInt()==1) ((QCheckBox *)w)->setChecked(true);
            else ((QCheckBox *)w)->setChecked(false);
            ((QCheckBox *)w)->blockSignals(oldState);
        }
        if(w->objectName().contains("le"))
        {
            res = "G" + w->objectName().mid(3).replace("_",",") + "\n";
            ((QLineEdit *)w)->setText(comms->SendMess(res));
        }
    }
    widgetList = dui->gbDigitalIn->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("chk"))
        {
            res = "G" + w->objectName().mid(4).replace("_",",") + "\n";
            if(comms->SendMess(res).toInt()==1) ((QCheckBox *)w)->setChecked(true);
            else ((QCheckBox *)w)->setChecked(false);
        }
    }
    dui->tabMIPS->setEnabled(true);
    dui->statusBar->clearMessage();
}

// UpdateDIO — slot for the manual Update push-button. Delegates to Update().
void DIO::UpdateDIO(void)
{
    Update();
}

// Slot for digital output checkbox state change — derives the MIPS set command
// from the widget object name and sends the new 0/1 state.
void DIO::DOUpdated(void)
{
    QObject* obj = sender();
    QString res;

    if(obj->objectName().startsWith("chkS"))
    {
        res = obj->objectName().mid(3).replace("_",",") + ",";
        if(((QCheckBox *)obj)->checkState()) res += "1\n";
        else res+= "0\n";
        comms->SendCommand(res.toStdString().c_str());
    }
}

// TrigHigh / TrigLow / TrigPulse — send the corresponding TRIGOUT command
// to the connected MIPS unit.
void DIO::TrigHigh(void)
{
    comms->SendCommand("TRIGOUT,HIGH\n");
}

void DIO::TrigLow(void)
{
    comms->SendCommand("TRIGOUT,LOW\n");
}

void DIO::TrigPulse(void)
{
    comms->SendCommand("TRIGOUT,PULSE\n");
}

// RFgenerate — sends BURST,<count> using the value from the Burst field.
void DIO::RFgenerate(void)
{
    QString res;

    res = "BURST," + dui->Burst->text() + "\n";
    comms->SendCommand(res.toStdString().c_str());
}

// SetFreq — slot for leSFREQ editingFinished. Sends the new frequency to MIPS.
void DIO::SetFreq(void)
{
    QString res;

    if(!(dui->leSFREQ->isModified())) return;
    res = dui->leSFREQ->objectName().mid(2).replace("_",",") + "," + dui->leSFREQ->text() + "\n";
    comms->SendCommand(res.toStdString().c_str());
    dui->leSFREQ->setModified(false);
}

// SetWidth — slot for leSWIDTH editingFinished. Sends the new pulse width to MIPS.
void DIO::SetWidth(void)
{
    QString res;

    if(!(dui->leSWIDTH->isModified())) return;
    res = dui->leSWIDTH->objectName().mid(2).replace("_",",") + "," + dui->leSWIDTH->text() + "\n";
    comms->SendCommand(res.toStdString().c_str());
    dui->leSWIDTH->setModified(false);
}

// Save — writes the state of all chkS* digital output checkboxes to a
// timestamped CSV settings file.
void DIO::Save(QString Filename)
{
    QString res;

    if(Filename == "") return;
    QFile file(Filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QDateTime dateTime = QDateTime::currentDateTime();
        stream << "# DIO settings, " + dateTime.toString() + "\n";
        QObjectList widgetList = dui->gbDigitalOut->children();
        foreach(QObject *w, widgetList)
        {
            if(w->objectName().mid(0,4) == "chkS")
            {
                res = w->objectName() + ",";
                if(((QCheckBox *)w)->checkState()) res += "true\n";
                else res += "false\n";
                stream << res;
            }
        }
        file.close();
        dui->statusBar->showMessage("Settings saved to " + Filename, 2000);
    }
}

// Load — reads a settings file and restores the digital output checkbox states.
void DIO::Load(QString Filename)
{
    QStringList resList;

    if(Filename == "") return;
    QObjectList widgetList = dui->gbDigitalOut->children();
    QFile file(Filename);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            resList = line.split(",");
            if(resList.count() == 2)
            {
                foreach(QObject *w, widgetList)
                {
                    if(w->objectName().mid(0,4) == "chkS")
                    {
                        if(w->objectName() == resList[0])
                        {
                            if(resList[1] == "true") ((QCheckBox *)w)->setChecked(true);
                            else                     ((QCheckBox *)w)->setChecked(false);
                        }
                    }
                }
            }
        } while(!line.isNull());
        file.close();
        dui->statusBar->showMessage("Settings loaded from " + Filename, 2000);
    }
}

// -----------------------------------------------------------------------------
// RemoteNavigation — enables or disables the front-panel navigation buttons
// based on the Remote Nav checkbox. Sends SSERIALNAV,TRUE to activate serial
// navigation mode on the MIPS controller.
// -----------------------------------------------------------------------------
void DIO::RemoteNavigation(void)
{
    if(dui->chkRemoteNav->isChecked())
    {
        if(comms->SendCommand("SSERIALNAV,TRUE\n"))
        {
            dui->pbUPsmall->setEnabled(true);
            dui->pbUPlarge->setEnabled(true);
            dui->pbDOWNsmall->setEnabled(true);
            dui->pbDOWNlarge->setEnabled(true);
            dui->pbSelect->setEnabled(true);
            return;
        }
    }
    dui->pbUPsmall->setEnabled(false);
    dui->pbUPlarge->setEnabled(false);
    dui->pbDOWNsmall->setEnabled(false);
    dui->pbDOWNlarge->setEnabled(false);
    dui->pbSelect->setEnabled(false);
}

// RemoteNavSmallUP / LargeUP / SmallDown / LargeDown / Select — send the
// corresponding ASCII control code to the MIPS front-panel serial navigator.
void DIO::RemoteNavSmallUP(void)
{
    char str[2] = { NAV_SMALL_UP, 0 };
    comms->SendString(QString::fromStdString(str));
}

void DIO::RemoteNavLargeUP(void)
{
    char str[2] = { NAV_LARGE_UP, 0 };
    comms->SendString(QString::fromStdString(str));
}

void DIO::RemoteNavSmallDown(void)
{
    char str[2] = { NAV_SMALL_DOWN, 0 };
    comms->SendString(QString::fromStdString(str));
}

void DIO::RemoteNavLargeDown(void)
{
    char str[2] = { NAV_LARGE_DOWN, 0 };
    comms->SendString(QString::fromStdString(str));
}

void DIO::RemoteNavSelect(void)
{
    char str[2] = { NAV_SELECT, 0 };
    comms->SendString(QString::fromStdString(str));
}

// =============================================================================
// DIOchannel — control panel widget for a single DIO channel
// =============================================================================

// DIOchannel — constructor. Records position and identity information;
// call Show() to create the visible checkbox widget.
DIOchannel::DIOchannel(QWidget *parent, QString name, QString MIPSname, int x, int y) : QWidget(parent)
{
    p       = parent;
    Title   = name;
    MIPSnm  = MIPSname;
    X       = x;
    Y       = y;
    Channel = "A";
    comms   = NULL;
    ReadOnly = false;
}

// Show — creates the checkbox widget on the parent control panel.
// Channels above "P" are automatically set read-only (input-only channels).
void DIOchannel::Show(void)
{
    frmDIO = new QFrame(p); frmDIO->setGeometry(X, Y, 170, 21);
    DIO    = new QCheckBox(frmDIO); DIO->setGeometry(0, 0, 170, 21);
    DIO->setText(Title);
    DIO->setToolTip(MIPSnm + " DIO channel " + Channel);
    if(Channel > "P") ReadOnly = true;
    connect(DIO, SIGNAL(clicked(bool)), this, SLOT(DIOChange(bool)));
    frmDIO->installEventFilter(this);
    frmDIO->setMouseTracking(true);
    DIO->installEventFilter(this);
    DIO->setMouseTracking(true);
}

// eventFilter — delegates drag-to-move to moveWidget().
bool DIOchannel::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, frmDIO, DIO, event)) return true;
    return false;
}

// Report — returns a "title,1|0" CSV string with the current checkbox state.
QString DIOchannel::Report(void)
{
    QString res;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    res = title + ",";
    if(DIO->isChecked()) res += "1";
    else                 res += "0";
    return(res);
}

// SetValues — parses a "title,1|0" CSV string and applies the state to the
// checkbox. Returns false if the title does not match.
bool DIOchannel::SetValues(QString strVals)
{
    QStringList resList;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!strVals.startsWith(title)) return false;
    resList = strVals.split(",");
    if(resList.count() < 2) return false;
    if(resList[1].contains("1")) DIO->setChecked(true);
    else                         DIO->setChecked(false);
    if(resList[1].contains("1")) emit DIO->checkStateChanged(Qt::Checked);
    else                         emit DIO->checkStateChanged(Qt::Unchecked);
    return true;
}

// ProcessCommand — handles scripting API commands for this widget.
// Supported commands:
//   title      — returns "1" if checked, "0" if unchecked
//   title=1    — sets the channel high
//   title=0    — sets the channel low
// Returns "?" if the command is not recognised.
QString DIOchannel::ProcessCommand(QString cmd)
{
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";
    if(cmd == title)
    {
        if(DIO->isChecked()) return "1";
        return "0";
    }
    QStringList resList = cmd.split("=");
    if(resList.count() == 2)
    {
        if(resList[1] == "1")      DIO->setChecked(true);
        else if(resList[1] == "0") DIO->setChecked(false);
        else return "?";
        if(resList[1] == "1") DIOChange(1);
        else                  DIOChange(0);
        return "";
    }
    return "?";
}

// Update — queries GDIO,channel from MIPS and syncs the checkbox. Signals
// are blocked during the update to prevent DIOChange from firing.
void DIOchannel::Update(void)
{
    QString res;

    if(comms == NULL) return;
    comms->rb.clear();
    res = comms->SendMess("GDIO," + Channel + "\n");
    bool oldState = DIO->blockSignals(true);
    if(res.contains("1"))      DIO->setChecked(true);
    else if(res.contains("0")) DIO->setChecked(false);
    DIO->blockSignals(oldState);
}

// DIOChange — sends the new channel state to hardware. If the channel is
// read-only, reverts the checkbox to its previous state without sending.
void DIOchannel::DIOChange(bool state)
{
    QString res;

    DIO->setFocus();
    if(ReadOnly)
    {
        bool oldState = DIO->blockSignals(true);
        if(state) DIO->setChecked(false);
        else      DIO->setChecked(true);
        DIO->blockSignals(oldState);
        return;
    }
    if(comms == NULL) return;
    if(DIO->checkState()) res = "SDIO," + Channel + ",1\n";
    else                  res = "SDIO," + Channel + ",0\n";
    comms->SendCommand(res.toStdString().c_str());
}
