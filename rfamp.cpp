// =============================================================================
// rfamp.cpp
//
// RF amplifier control panel widget for the MIPS host application.
//
// Depends on:  rfamp.h, comms.h, Utilities.h, ui_rfamp.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "rfamp.h"
#include "ui_rfamp.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Constructor — connects dynamically-named widgets and installs event filters
// on the line edits that support arrow-key value adjustment.
// -----------------------------------------------------------------------------
RFamp::RFamp(QWidget *parent, QString name, QString MIPSname, int Module) :
    QDialog(parent),
    ui(new Ui::RFamp)
{
    p         = parent;
    ui->setupUi(this);
    Updating   = false;
    UpdateOff  = false;
    isShutdown = false;
    comms      = NULL;
    Title      = name;
    MIPSnm     = MIPSname;
    Channel    = Module;

    ui->gbRFamp->setTitle(Title);
    ui->gbRFamp->setToolTip(MIPSnm + " Module: " + QString::number(Module));

    QObjectList widgetList = ui->tabRFsettings->children();
    widgetList += ui->tabMassFilter->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("leS"))
        {
            if(QLineEdit *le = qobject_cast<QLineEdit *>(w))
                connect(le, &QLineEdit::returnPressed, this, &RFamp::Updated);
        }
        else if(w->objectName().contains("chk"))
            connect(((QCheckBox *)w), &QCheckBox::toggled, this, &RFamp::Updated);
        else if(w->objectName().contains("rb"))
            connect(((QRadioButton *)w), &QRadioButton::clicked, this, &RFamp::Updated);
    }
    connect(ui->pbRFAupdate, &QPushButton::pressed, this, &RFamp::slotUpdate);

    // Install event filters for arrow-key value nudging
    ui->leSRFAFREQ->installEventFilter(this); ui->leSRFAFREQ->setMouseTracking(true);
    ui->leSRFARES->installEventFilter(this);  ui->leSRFARES->setMouseTracking(true);
    ui->leSRFAR0->installEventFilter(this);   ui->leSRFAR0->setMouseTracking(true);
    ui->leSRFAK->installEventFilter(this);    ui->leSRFAK->setMouseTracking(true);
    ui->gbRFamp->setMouseTracking(true);
}

// ~RFamp — destructor. Releases the UI form.
RFamp::~RFamp()
{
    delete ui;
}

// -----------------------------------------------------------------------------
// eventFilter — handles arrow-key value nudging on FREQ, RES, R0, and K fields.
// R0 and K use a finer multiplier (mult / 1000) than FREQ and RES.
// -----------------------------------------------------------------------------
bool RFamp::eventFilter(QObject *obj, QEvent *event)
{
    if((obj != ui->leSRFAFREQ) && (obj != ui->leSRFARES) &&
        (obj != ui->leSRFAR0)   && (obj != ui->leSRFAK))
        return QObject::eventFilter(obj, event);

    if(Updating) return true;
    UpdateOff = true;
    float mult = 10;
    if(obj == ui->leSRFAR0) mult /= 1000;
    if(obj == ui->leSRFAK)  mult /= 1000;
    if(adjustValue(obj, (QLineEdit *)obj, event, mult))
    {
        UpdateOff = false;
        return true;
    }
    UpdateOff = false;
    return QObject::eventFilter(obj, event);
}

// -----------------------------------------------------------------------------
// Update — polls all RF amp parameters from MIPS and refreshes the UI.
// On first call reads all tabs; thereafter reads only the active tab.
// -----------------------------------------------------------------------------
void RFamp::Update(void)
{
    QStringList resList;
    QString res;
    QString CurrentList;
    int i;
    static bool inited = false;
    QObjectList widgetList;

    if(comms == NULL) return;
    if(UpdateOff) return;
    Updating = true;

    if(inited) widgetList = ui->tabRFquad->currentWidget()->children();
    else
    {
        widgetList  = ui->tabRFsettings->children();
        widgetList += ui->tabMassFilter->children();
    }

    foreach(QObject *w, widgetList)
    {
        comms->rb.clear();
        if(w->objectName().contains("leS") || w->objectName().contains("leG"))
        {
            if(!((QLineEdit *)w)->hasFocus())
            {
                res = "G" + w->objectName().mid(3) + "," + QString::number(Channel) + "\n";
                ((QLineEdit *)w)->setText(comms->SendMess(res));
            }
        }
        else if(w->objectName().contains("chk"))
        {
            // Checkbox names encode command and on/off response values: chk<CMD>_<ON>_<OFF>
            // DCPWR is a global command (no channel number); all others include channel.
            resList = w->objectName().split("_");
            if(resList.count() == 3)
            {
                if(resList[0].mid(4) == "DCPWR") res = "G" + resList[0].mid(4) + "\n";
                else res = "G" + resList[0].mid(4) + "," + QString::number(Channel) + "\n";
                res = comms->SendMess(res);
                if(res == resList[1]) ((QCheckBox *)w)->setChecked(true);
                if(res == resList[2]) ((QCheckBox *)w)->setChecked(false);
                if(res.contains("?")) comms->waitforline(100);
            }
        }
        else if(w->objectName().contains("rb"))
        {
            resList = w->objectName().split("_");
            if(resList.count() == 2)
            {
                res = "G" + resList[0].mid(3) + "," + QString::number(Channel) + "\n";
                res = comms->SendMess(res);
                if(res == resList[1]) ((QRadioButton *)w)->setChecked(true);
            }
        }
        else if(w->objectName().contains("lel"))
        {
            // Multi-value list line edits: lel<CMD>_<INDEX>
            resList = w->objectName().split("_");
            if(resList.count() == 2)
            {
                i   = resList[1].toInt();
                res = resList[0].mid(3);
                if(!CurrentList.startsWith(res)) CurrentList = res + "," + comms->SendMess(res + "," + QString::number(Channel) + "\n");
                resList = CurrentList.split(",");
                if(resList.count() > i) ((QLineEdit *)w)->setText(resList[i]);
            }
        }
    }
    Updating = false;
    inited   = true;
}

// -----------------------------------------------------------------------------
// ProcessCommand — scripting interface for reading and writing RF amp parameters
// by human-readable name.
// -----------------------------------------------------------------------------
QString RFamp::ProcessCommand(QString cmd)
{
    QString      title;
    QLineEdit    *le = NULL;
    QCheckBox    *cb = NULL;
    QRadioButton *rb = NULL;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";

    // Read-only readback fields
    if(cmd == title + ".RF settings.Enable")
        return ui->chkSRFAENA_ON_OFF->checkState() ? "TRUE" : "FALSE";
    if(cmd == title + ".RF settings.Open loop")
        return ui->rbSRFAMOD_OPEN->isChecked() ? "TRUE" : "FALSE";
    if(cmd == title + ".RF settings.Closed loop")
        return ui->rbSRFAMOD_CLOSED->isChecked() ? "TRUE" : "FALSE";
    if(cmd == title + ".RF settings.Frequency") return ui->leSRFAFREQ->text();
    if(cmd == title + ".RF settings.Drive")     return ui->leSRFADRV->text();
    if(cmd == title + ".RF settings.Setpoint")  return ui->leSRFALEV->text();
    if(cmd == title + ".RF settings.RF+")       return ui->leGRFAVPPA->text();
    if(cmd == title + ".RF settings.RF-")       return ui->leGFRAVPPB->text();
    if(cmd == title + ".RF settings.RF pwr")    return ui->leGRFAPWR->text();

    if(cmd == title + ".Mass filter.DC power supply enable")
        return ui->chkSDCPWR_ON_OFF->checkState() ? "TRUE" : "FALSE";
    if(cmd == title + ".Mass filter.Ro")           return ui->leSRFAR0->text();
    if(cmd == title + ".Mass filter.k")            return ui->leSRFAK->text();
    if(cmd == title + ".Mass filter.Resolving DC") return ui->leSRFARDC->text();
    if(cmd == title + ".Mass filter.Pole Bias")    return ui->leSRFAPB->text();
    if(cmd == title + ".Mass filter.Resolution")   return ui->leSRFARES->text();
    if(cmd == title + ".Mass filter.m/z")          return ui->leSRFAMZ->text();
    if(cmd == title + ".Mass filter.Update")       { ui->pbRFAupdate->click(); return ""; }

    if(cmd == title + ".RF amp.DC V")       return ui->lelRRFAAMP_1->text();
    if(cmd == title + ".RF amp.DC I in")    return ui->lelRRFAAMP_2->text();
    if(cmd == title + ".RF amp.DC pwr")     return ui->lelRRFAAMP_3->text();
    if(cmd == title + ".RF amp.Temp")       return ui->lelRRFAAMP_4->text();
    if(cmd == title + ".RF amp.RF Vrms")    return ui->lelRRFAAMP_5->text();
    if(cmd == title + ".RF amp.RF Irms")    return ui->lelRRFAAMP_6->text();
    if(cmd == title + ".RF amp.RF fwd pwr") return ui->lelRRFAAMP_7->text();
    if(cmd == title + ".RF amp.RF rev pwr") return ui->lelRRFAAMP_8->text();
    if(cmd == title + ".RF amp.SWR")        return ui->lelRRFAAMP_9->text();

    // Write path: cmd=value
    QStringList resList = cmd.split("=");
    if(resList.count() == 2)
    {
        if(cmd.startsWith(title + ".RF settings.Enable"))       cb = ui->chkSRFAENA_ON_OFF;
        if(cmd.startsWith(title + ".RF settings.Open loop"))    rb = ui->rbSRFAMOD_OPEN;
        if(cmd.startsWith(title + ".RF settings.Closed loop"))  rb = ui->rbSRFAMOD_CLOSED;
        if(cmd.startsWith(title + ".RF settings.Frequency"))    le = ui->leSRFAFREQ;
        if(cmd.startsWith(title + ".RF settings.Drive"))        le = ui->leSRFADRV;
        if(cmd.startsWith(title + ".RF settings.Setpoint"))     le = ui->leSRFALEV;
        if(cmd.startsWith(title + ".Mass filter.Ro"))           le = ui->leSRFAR0;
        if(cmd.startsWith(title + ".Mass filter.k"))            le = ui->leSRFAK;
        if(cmd.startsWith(title + ".Mass filter.Resolving DC")) le = ui->leSRFARDC;
        if(cmd.startsWith(title + ".Mass filter.Pole Bias"))    le = ui->leGFRAVPPB;
        if(cmd.startsWith(title + ".Mass filter.Resolution"))   le = ui->leSRFARES;
        if(cmd.startsWith(title + ".Mass filter.m/z"))          le = ui->leSRFAMZ;

        if(le != NULL)
        {
            le->setText(resList[1]);
            le->setModified(true);
            emit le->editingFinished();
            return "";
        }
        if(cb != NULL)
        {
            if(resList[1].trimmed() == "TRUE")  cb->setChecked(true);
            if(resList[1].trimmed() == "FALSE") cb->setChecked(false);
            emit cb->clicked();
            return "";
        }
        if(rb != NULL)
        {
            if(resList[1].trimmed() == "TRUE")  rb->setChecked(true);
            if(resList[1].trimmed() == "FALSE") rb->setChecked(false);
            emit rb->clicked();
            return "";
        }
    }
    return "?";
}

// -----------------------------------------------------------------------------
// Updated — slot called when any setpoint widget changes. Sends the new value
// to MIPS using the command encoded in the widget's object name.
// -----------------------------------------------------------------------------
void RFamp::Updated(void)
{
    QObject     *obj = sender();
    QString      res;
    QStringList  resList;

    if(comms == NULL) return;
    if(Updating) return;

    if(obj->objectName().startsWith("leS"))
    {
        if(!((QLineEdit *)obj)->isModified()) return;
        res = obj->objectName().mid(2) + "," + QString::number(Channel) + "," + ((QLineEdit *)obj)->text() + "\n";
        comms->SendCommand(res.toStdString().c_str());
        ((QLineEdit *)obj)->setModified(false);
    }
    if(obj->objectName().startsWith("chkS"))
    {
        resList = obj->objectName().mid(3).split("_");
        if(resList.count() == 3)
        {
            // DCPWR is a global command; all others include channel number
            if(resList[0] == "SDCPWR")
            {
                if(((QCheckBox *)obj)->isChecked()) comms->SendCommand(resList[0] + "," + resList[1] + "\n");
                else comms->SendCommand(resList[0] + "," + resList[2] + "\n");
            }
            else
            {
                if(((QCheckBox *)obj)->isChecked()) comms->SendCommand(resList[0] + "," + QString::number(Channel) + "," + resList[1] + "\n");
                else comms->SendCommand(resList[0] + "," + QString::number(Channel) + "," + resList[2] + "\n");
            }
        }
    }
    if(obj->objectName().startsWith("rbS"))
    {
        resList = obj->objectName().mid(2).split("_");
        if(resList.count() == 2)
        {
            if(((QRadioButton *)obj)->isChecked())
                comms->SendCommand(resList[0] + "," + QString::number(Channel) + "," + resList[1] + "\n");
        }
    }
}

// -----------------------------------------------------------------------------
// Report — serialises all widget values for method file save.
// -----------------------------------------------------------------------------
QString RFamp::Report(void)
{
    QString res;
    QStringList resList;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    res.clear();

    QObjectList widgetList = ui->tabRFsettings->children();
    widgetList += ui->tabMassFilter->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().startsWith("leS"))
        {
            res += title + "," + w->objectName() + "," + ((QLineEdit *)w)->text() + "\n";
        }
        if(w->objectName().startsWith("chkS"))
        {
            resList = w->objectName().split("_");
            if(resList.count() == 3)
            {
                if(((QCheckBox *)w)->isChecked()) res += title + "," + resList[0] + "," + resList[1] + "\n";
                else res += title + "," + resList[0] + "," + resList[2] + "\n";
            }
        }
        if(w->objectName().startsWith("rbS"))
        {
            resList = w->objectName().split("_");
            if(resList.count() == 2)
                if(((QRadioButton *)w)->isChecked()) res += title + "," + resList[0] + "," + resList[1] + "\n";
        }
    }
    return res;
}

// -----------------------------------------------------------------------------
// SetValues — restores a single widget value from a method file line.
// -----------------------------------------------------------------------------
bool RFamp::SetValues(QString strVals)
{
    QStringList resList, ctrlList;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!strVals.startsWith(title)) return false;
    resList = strVals.split(",");
    if(resList.count() < 3) return false;

    QObjectList widgetList = ui->tabRFsettings->children();
    widgetList += ui->tabMassFilter->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().startsWith(resList[1]))
        {
            if(w->objectName().startsWith("leS"))
            {
                ((QLineEdit *)w)->setText(resList[2]);
                ((QLineEdit *)w)->setModified(true);
                emit ((QLineEdit *)w)->editingFinished();
                return true;
            }
            if(w->objectName().startsWith("chk"))
            {
                ctrlList = w->objectName().split("_");
                if(ctrlList.count() >= 3)
                {
                    if((ctrlList[0] == resList[1]) && (ctrlList[1] == resList[2])) { ((QCheckBox *)w)->setChecked(true);  return true; }
                    if((ctrlList[0] == resList[1]) && (ctrlList[2] == resList[2])) { ((QCheckBox *)w)->setChecked(false); return true; }
                    emit ((QCheckBox *)w)->clicked();
                }
            }
            if(w->objectName().startsWith("rb"))
            {
                if(w->objectName() == (resList[1] + "_" + resList[2]))
                {
                    ((QRadioButton *)w)->setChecked(true);
                    emit ((QRadioButton *)w)->clicked();
                    return true;
                }
            }
        }
    }
    return false;
}

// -----------------------------------------------------------------------------
// slotUpdate — sends the RF amp quad update command to MIPS.
// -----------------------------------------------------------------------------
void RFamp::slotUpdate(void)
{
    if(comms == NULL) return;
    comms->SendCommand("RFAQUPDATE," + QString::number(Channel) + "\n");
}

// -----------------------------------------------------------------------------
// Shutdown — disables RF and DC power, saving the current enable state for Restore.
// -----------------------------------------------------------------------------
void RFamp::Shutdown(void)
{
    if(isShutdown) return;
    isShutdown        = true;
    activeEnableState = ui->chkSRFAENA_ON_OFF->checkState();
    ui->chkSRFAENA_ON_OFF->setChecked(false);
    emit ui->chkSRFAENA_ON_OFF->checkStateChanged(Qt::Unchecked);
    ui->chkSDCPWR_ON_OFF->setChecked(false);
    emit ui->chkSDCPWR_ON_OFF->checkStateChanged(Qt::Unchecked);
}

// -----------------------------------------------------------------------------
// Restore — re-enables RF and DC power, restoring the saved enable state.
// -----------------------------------------------------------------------------
void RFamp::Restore(void)
{
    if(!isShutdown) return;
    isShutdown = false;
    ui->chkSRFAENA_ON_OFF->setChecked(activeEnableState);
    emit ui->chkSRFAENA_ON_OFF->checkStateChanged(activeEnableState ? Qt::Checked : Qt::Unchecked);
    ui->chkSDCPWR_ON_OFF->setChecked(true);
    emit ui->chkSDCPWR_ON_OFF->checkStateChanged(Qt::Checked);
}
