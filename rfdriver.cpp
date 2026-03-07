// =============================================================================
// rfdriver.cpp
//
// RF driver module classes for the MIPS host application.
//
// Depends on:  rfdriver.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "rfdriver.h"
#include "Utilities.h"
#include <QElapsedTimer>

// *************************************************************************************************
// RFdriver  ***************************************************************************************
// *************************************************************************************************

// RFdriver — constructor. Sets up the two-channel combo box, wires the
// frequency and drive line edits, auto-tune buttons, and the manual Update
// push-button.
RFdriver::RFdriver(Ui::MIPS *w, Comms *c)
{
    rui   = w;
    comms = c;

    SetNumberOfChannels(2);
    connect(rui->pbUpdateRF,   &QPushButton::pressed,        this, &RFdriver::UpdateRFdriver);
    connect(rui->leSRFFRQ,     &QLineEdit::returnPressed,    this, &RFdriver::leSRFFRQ_editingFinished);
    connect(rui->leSRFDRV,     &QLineEdit::returnPressed,    this, &RFdriver::leSRFDRV_editingFinished);
    connect(rui->pbAutoTune,   &QPushButton::pressed,        this, &RFdriver::AutoTune);
    connect(rui->pbAutoRetune, &QPushButton::pressed,        this, &RFdriver::AutoRetune);
    rui->leSRFFRQ->setValidator(new QIntValidator);
    rui->leSRFDRV->setValidator(new QDoubleValidator);
}

// SetNumberOfChannels — rebuilds the channel selector combo to show num
// channels (1-based labels). Reconnects currentTextChanged afterwards.
void RFdriver::SetNumberOfChannels(int num)
{
    disconnect(rui->comboRFchan, SIGNAL(currentTextChanged(QString)), 0, 0);
    NumChannels = num;
    rui->comboRFchan->clear();
    for(int i = 0; i < NumChannels; i++)
        rui->comboRFchan->addItem(QString::number(i + 1));
    connect(rui->comboRFchan, &QComboBox::currentTextChanged, this, &RFdriver::UpdateRFdriver);
}

// Update — polls frequency, drive, positive/negative peak voltage, and power
// readbacks for the currently selected channel and refreshes the UI.
void RFdriver::Update(void)
{
    rui->tabMIPS->setEnabled(false);
    rui->statusBar->showMessage(tr("Updating RF driver controls..."));
    rui->leSRFFRQ->setText(comms->SendMess("GRFFRQ,"  + rui->comboRFchan->currentText() + "\n"));
    rui->leSRFDRV->setText(comms->SendMess("GRFDRV,"  + rui->comboRFchan->currentText() + "\n"));
    rui->leGRFPPVP->setText(comms->SendMess("GRFPPVP," + rui->comboRFchan->currentText() + "\n"));
    rui->leGRFPPVN->setText(comms->SendMess("GRFPPVN," + rui->comboRFchan->currentText() + "\n"));
    rui->leGRFPWR->setText(comms->SendMess("GRFPWR,"  + rui->comboRFchan->currentText() + "\n"));
    rui->tabMIPS->setEnabled(true);
    rui->statusBar->showMessage(tr(""));
}

// Save — iterates through all channels selecting each in turn, writes every
// leS* line edit name/channel/value triplet to Filename as CSV.
void RFdriver::Save(QString Filename)
{
    if(NumChannels == 0) return;
    if(Filename == "") return;

    int SelectedChannel = rui->comboRFchan->currentIndex();
    QFile file(Filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QDateTime dateTime = QDateTime::currentDateTime();
        stream << "# RFdriver settings, " + dateTime.toString() + "\n";
        QObjectList widgetList = rui->gbRFparms->children();
        for(int i = 0; i < NumChannels; i++)
        {
            rui->comboRFchan->setCurrentIndex(i);
            foreach(QObject *w, widgetList)
            {
                if(w->objectName().mid(0, 3) == "leS")
                {
                    QString res = w->objectName() + "," + rui->comboRFchan->currentText() + ","
                                  + ((QLineEdit *)w)->text() + "\n";
                    stream << res;
                }
            }
        }
        rui->comboRFchan->setCurrentIndex(SelectedChannel);
        file.close();
        rui->statusBar->showMessage("Settings saved to " + Filename, 2000);
    }
}

// Load — reads CSV triplets from Filename and applies them to the matching
// leS* widget for the correct channel, triggering editingFinished to push each value to MIPS.
void RFdriver::Load(QString Filename)
{
    if(NumChannels == 0) return;
    if(Filename == "") return;

    int SelectedChannel = rui->comboRFchan->currentIndex();
    QStringList resList;
    QFile file(Filename);
    QObjectList widgetList = rui->gbRFparms->children();
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            resList = line.split(",");
            if(resList.count() == 3)
            {
                if(rui->comboRFchan->currentIndex() != (resList[1].toInt() - 1))
                    rui->comboRFchan->setCurrentIndex(resList[1].toInt() - 1);
                foreach(QObject *w, widgetList)
                {
                    if(w->objectName().mid(0, 3) == "leS")
                    {
                        if(resList[2] != "" && w->objectName() == resList[0])
                        {
                            ((QLineEdit *)w)->setText(resList[2]);
                            ((QLineEdit *)w)->setModified(true);
                            QMetaObject::invokeMethod(w, "editingFinished");
                        }
                    }
                }
            }
        } while(!line.isNull());
        rui->comboRFchan->setCurrentIndex(SelectedChannel);
        file.close();
        rui->statusBar->showMessage("Settings loaded from " + Filename, 2000);
    }
}

// UpdateRFdriver — slot for the manual Update push-button and channel change.
// Delegates to Update().
void RFdriver::UpdateRFdriver(void)
{
    Update();
}

// leSRFFRQ_editingFinished — slot for frequency line edit returnPressed.
// Sends SRFFRQ,channel,value to MIPS.
void RFdriver::leSRFFRQ_editingFinished()
{
    if(!rui->leSRFFRQ->isModified()) return;
    comms->SendCommand("SRFFRQ," + rui->comboRFchan->currentText() + "," + rui->leSRFFRQ->text() + "\n");
    rui->leSRFFRQ->setModified(false);
}

// leSRFDRV_editingFinished — slot for drive line edit returnPressed.
// Sends SRFDRV,channel,value to MIPS.
void RFdriver::leSRFDRV_editingFinished()
{
    if(!rui->leSRFDRV->isModified()) return;
    comms->SendCommand("SRFDRV," + rui->comboRFchan->currentText() + "," + rui->leSRFDRV->text() + "\n");
    rui->leSRFDRV->setModified(false);
}

// AutoTune — prompts the user and sends TUNERFCH,channel. Only one channel
// can be tuning at a time; MIPS returns an error if another tune is active.
void RFdriver::AutoTune(void)
{
    QMessageBox msgBox;

    rui->pbAutoTune->setDown(false);
    QString msg = "This function will tune the RF head attached to channel " + rui->comboRFchan->currentText() + ". ";
    msg += "Make sure the RF head is attached and connected to your system as needed. ";
    msg += "This process can take up to 3 minutes.\n";
    msgBox.setText(msg);
    msgBox.setInformativeText("Are you sure you want to continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::No) return;
    if(!comms->SendCommand("TUNERFCH," + rui->comboRFchan->currentText() + "\n"))
    {
        msgBox.setText("Request failed! Only one channel can be tuned or retuned at a time.");
        msgBox.setInformativeText("");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
}

// AutoRetune — prompts the user and sends RETUNERFCH,channel to refine the
// existing tune without a full frequency sweep.
void RFdriver::AutoRetune(void)
{
    QMessageBox msgBox;

    rui->pbAutoRetune->setDown(false);
    QString msg = "This function will retune the RF head attached to channel " + rui->comboRFchan->currentText() + ". ";
    msg += "Make sure the RF head is attached and connected to your system as needed. ";
    msg += "This process can take up to 1 minute.\n";
    msgBox.setText(msg);
    msgBox.setInformativeText("Are you sure you want to continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::No) return;
    if(!comms->SendCommand("RETUNERFCH," + rui->comboRFchan->currentText() + "\n"))
    {
        msgBox.setText("Request failed! Only one channel can be tuned or retuned at a time.");
        msgBox.setInformativeText("");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
}

// *************************************************************************************************
// RFchannel  **************************************************************************************
// *************************************************************************************************

// RFchannel — constructor. Records position and identity. Call Show() to
// create the group box with Drive/Freq/readback line edits and Tune/Retune buttons.
RFchannel::RFchannel(QWidget *parent, QString name, QString MIPSname, int x, int y) : QWidget(parent)
{
    p          = parent;
    Title      = name;
    MIPSnm     = MIPSname;
    X          = x;
    Y          = y;
    comms      = NULL;
    isShutdown = false;
    Updating   = false;
    UpdateOff  = false;
}

// Show — creates and lays out the RF channel group box with all line edits,
// labels, and buttons, installs drag-to-move and value-scroll event filters.
void RFchannel::Show(void)
{
    gbRF = new QGroupBox(Title, p);
    gbRF->setGeometry(X, Y, 200, 180);
    gbRF->setToolTip(MIPSnm + " RF channel " + QString::number(Channel));

    labels[0] = new QLabel("Drive", gbRF); labels[0]->setGeometry(10,  26, 59, 16);
    labels[1] = new QLabel("Freq",  gbRF); labels[1]->setGeometry(10,  48, 59, 16);
    labels[2] = new QLabel("RF+",   gbRF); labels[2]->setGeometry(10,  73, 59, 16);
    labels[3] = new QLabel("RF-",   gbRF); labels[3]->setGeometry(10,  96, 59, 16);
    labels[4] = new QLabel("Power", gbRF); labels[4]->setGeometry(10, 118, 59, 16);
    labels[5] = new QLabel("%",     gbRF); labels[5]->setGeometry(159,  26, 31, 21);
    labels[6] = new QLabel("Hz",    gbRF); labels[6]->setGeometry(159,  48, 31, 21);
    labels[7] = new QLabel("Vp-p",  gbRF); labels[7]->setGeometry(159,  73, 31, 21);
    labels[8] = new QLabel("Vp-p",  gbRF); labels[8]->setGeometry(159,  96, 31, 21);
    labels[9] = new QLabel("W",     gbRF); labels[9]->setGeometry(159, 118, 31, 21);

    Drive = new QLineEdit(gbRF); Drive->setGeometry(60,  22, 91, 21); Drive->setValidator(new QDoubleValidator);
    Freq  = new QLineEdit(gbRF); Freq->setGeometry( 60,  46, 91, 21); Freq->setValidator(new QIntValidator);
    RFP   = new QLineEdit(gbRF); RFP->setGeometry(  60,  70, 91, 21); RFP->setReadOnly(true);
    RFN   = new QLineEdit(gbRF); RFN->setGeometry(  60,  94, 91, 21); RFN->setReadOnly(true);
    Power = new QLineEdit(gbRF); Power->setGeometry(60, 118, 91, 21); Power->setReadOnly(true);

    Tune   = new QPushButton("Tune",   gbRF); Tune->setGeometry(  10, 147, 81, 32); Tune->setAutoDefault(false);
    Retune = new QPushButton("Retune", gbRF); Retune->setGeometry(102, 147, 81, 32); Retune->setAutoDefault(false);

    connect(Drive,  &QLineEdit::returnPressed,   this, &RFchannel::DriveChange);
    connect(Freq,   &QLineEdit::returnPressed,   this, &RFchannel::FreqChange);
    connect(Tune,   &QPushButton::pressed,       this, &RFchannel::TunePressed);
    connect(Retune, &QPushButton::pressed,       this, &RFchannel::RetunePressed);

    gbRF->installEventFilter(this);
    gbRF->setMouseTracking(true);
    Drive->installEventFilter(this);
    Drive->setMouseTracking(true);
    Freq->installEventFilter(this);
    Freq->setMouseTracking(true);
}

// eventFilter — handles drag-to-move via moveWidget() and mouse-wheel value
// adjustment on Drive (step 1) and Freq (step 1000 Hz).
bool RFchannel::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, gbRF, gbRF, event)) return true;
    if(Updating) return true;
    UpdateOff = true;
    if(adjustValue(obj, Drive, event, 1))    { UpdateOff = false; return true; }
    UpdateOff = false;
    if(adjustValue(obj, Freq,  event, 1000)) { UpdateOff = false; return true; }
    UpdateOff = false;
    return QObject::eventFilter(obj, event);
}

// Returns a comma-separated string: Name,RFdrive,RFfreq,RFvolt+,RFvolt-,Power
QString RFchannel::Report(void)
{
    QString title;
    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;

    QString res = title + "," + (isShutdown ? activeDrive : Drive->text());
    res += "," + Freq->text() + "," + RFP->text() + "," + RFN->text() + "," + Power->text();
    return res;
}

// Sets RFchannel parameters if the name matches and at least 3 comma-separated values are present.
bool RFchannel::SetValues(QString strVals)
{
    QString title;
    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!strVals.startsWith(title)) return false;

    QStringList resList = strVals.split(",");
    if(resList.count() < 3) return false;

    Freq->setText(resList[2]);
    Freq->setModified(true);
    emit Freq->editingFinished();
    if(isShutdown)
    {
        activeDrive = resList[1];
    }
    else
    {
        Drive->setText(resList[1]);
        Drive->setModified(true);
        emit Drive->editingFinished();
    }
    return true;
}

// Processes scripting commands for this channel:
//   title.Drive, title.Freq          — read or set
//   title.RF+, title.RF-, title.Power — read only
//   title.color=<css>                 — set group box background colour
// Returns "?" if the command was not handled.
QString RFchannel::ProcessCommand(QString cmd)
{
    QString title;
    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";

    if(cmd == title + ".Drive") return Drive->text();
    if(cmd == title + ".Freq")  return Freq->text();
    if(cmd == title + ".RF+")   return RFP->text();
    if(cmd == title + ".RF-")   return RFN->text();
    if(cmd == title + ".Power") return Power->text();

    QStringList resList = cmd.split("=");
    if(resList.count() == 2)
    {
        if(cmd.startsWith(title + ".Drive"))
        {
            Drive->setText(resList[1]);
            Drive->setModified(true);
            emit Drive->editingFinished();
            return "";
        }
        if(cmd.startsWith(title + ".Freq"))
        {
            Freq->setText(resList[1]);
            Freq->setModified(true);
            emit Freq->editingFinished();
            return "";
        }
        if(cmd.startsWith(title + ".color"))
        {
            gbRF->setStyleSheet("background-color: " + resList[1].trimmed());
            return "";
        }
    }
    return "?";
}

// Updates display fields from MIPS or from a pre-fetched comma-separated value string.
// sVals format: freq,drive,RF+,RF-,Power  (all optional — missing values are read from MIPS).
void RFchannel::Update(QString sVals)
{
    QString     res;
    QStringList sValsList;
    bool        ok;

    sValsList = (sVals == "") ? QStringList() : sVals.split(",");
    if(comms == NULL) return;
    if(UpdateOff) return;
    Updating = true;
    comms->rb.clear();

    if(sValsList.count() < 2) res = comms->SendMess("GRFDRV,"  + QString::number(Channel) + "\n");
    else                      res = sValsList[1];
    res.toFloat(&ok);
    if(!Drive->hasFocus() && res != "" && ok) Drive->setText(res);

    if(sValsList.count() < 1) res = comms->SendMess("GRFFRQ,"  + QString::number(Channel) + "\n");
    else                      res = sValsList[0];
    res.toInt(&ok);
    if(!Freq->hasFocus() && res != "" && ok) Freq->setText(res);

    if(sValsList.count() < 3) res = comms->SendMess("GRFPPVP," + QString::number(Channel) + "\n");
    else                      res = sValsList[2];
    res.toFloat(&ok);
    if(res != "" && ok) RFP->setText(res);

    if(sValsList.count() < 4) res = comms->SendMess("GRFPPVN," + QString::number(Channel) + "\n");
    else                      res = sValsList[3];
    res.toFloat(&ok);
    if(res != "" && ok) RFN->setText(res);

    if(sValsList.count() < 5) res = comms->SendMess("GRFPWR,"  + QString::number(Channel) + "\n");
    else                      res = sValsList[4];
    res.toFloat(&ok);
    if(res != "" && ok) Power->setText(res);

    Updating = false;
    gbRF->repaint();
}

// DriveChange — slot for Drive returnPressed. Sends SRFDRV,channel,value.
void RFchannel::DriveChange(void)
{
    if(comms == NULL) return;
    if(!Drive->isModified()) return;
    comms->SendCommand("SRFDRV," + QString::number(Channel) + "," + Drive->text() + "\n");
    Drive->setModified(false);
}

// FreqChange — slot for Freq returnPressed. Sends SRFFRQ,channel,value.
void RFchannel::FreqChange(void)
{
    if(comms == NULL) return;
    if(!Freq->isModified()) return;
    comms->SendCommand("SRFFRQ," + QString::number(Channel) + "," + Freq->text() + "\n");
    Freq->setModified(false);
}

// TunePressed — prompts the user and initiates a full RF head tune sequence
// (zeros drive, waits 1 s, sends TUNERFCH,channel).
void RFchannel::TunePressed(void)
{
    QMessageBox msgBox;

    Tune->setDown(false);
    QString msg = "This function will tune the RF head attached to " + MIPSnm + " channel " + QString::number(Channel) + ". ";
    msg += "Make sure the RF head is attached and connected to your system as needed. ";
    msg += "This process can take up to 3 minutes.\n";
    msgBox.setText(msg);
    msgBox.setInformativeText("Are you sure you want to continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::No) return;

    // Zero the drive and wait 1 s before tuning
    if(comms != NULL) comms->SendCommand("SRFDRV," + QString::number(Channel) + ",0\n");
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() < 1000) QApplication::processEvents();

    if(comms != NULL) comms->SendCommand("TUNERFCH," + QString::number(Channel) + "\n");
}

// RetunePressed — prompts the user and sends RETUNERFCH,channel for a fast
// retune of the existing resonance without a full frequency sweep.
void RFchannel::RetunePressed(void)
{
    QMessageBox msgBox;

    Retune->setDown(false);
    QString msg = "This function will retune the RF head attached to " + MIPSnm + " channel " + QString::number(Channel) + ". ";
    msg += "Make sure the RF head is attached and connected to your system as needed. ";
    msg += "This process can take up to 1 minute.\n";
    msgBox.setText(msg);
    msgBox.setInformativeText("Are you sure you want to continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::No) return;
    if(comms != NULL) comms->SendCommand("RETUNERFCH," + QString::number(Channel) + "\n");
}

// Shutdown — saves the drive setpoint and zeroes it for safe power-down.
// Restore — returns the drive to the saved setpoint.
void RFchannel::Shutdown(void)
{
    if(isShutdown) return;
    isShutdown  = true;
    activeDrive = Drive->text();
    Drive->setText("0");
    Drive->setModified(true);
    emit Drive->editingFinished();
}

void RFchannel::Restore(void)
{
    if(!isShutdown) return;
    isShutdown = false;
    Drive->setText(activeDrive);
    Drive->setModified(true);
    emit Drive->editingFinished();
}

// *************************************************************************************************
// RFCchannel — closed-loop RF driver channel  *****************************************************
// *************************************************************************************************

// RFCchannel — constructor. Records position and identity. Call Show() to
// create the group box with Drive/Setpoint/Freq/readback line edits, open/closed-loop
// radio buttons, and Tune/Retune buttons.
RFCchannel::RFCchannel(QWidget *parent, QString name, QString MIPSname, int x, int y) : QWidget(parent)
{
    p          = parent;
    Title      = name;
    MIPSnm     = MIPSname;
    X          = x;
    Y          = y;
    comms      = NULL;
    isShutdown = false;
    Updating   = false;
    UpdateOff  = false;
}

// Show — creates and lays out the closed-loop RF channel group box with all line
// edits, labels, radio buttons, and Tune/Retune buttons. Installs event filters for
// drag-to-move and mouse-wheel adjustment on Drive, Setpoint, and Freq.
void RFCchannel::Show(void)
{
    gbRF = new QGroupBox(Title, p);
    gbRF->setGeometry(X, Y, 200, 225);
    gbRF->setToolTip(MIPSnm + " RF channel " + QString::number(Channel));

    labels[0]  = new QLabel("Drive",    gbRF); labels[0]->setGeometry( 8,  26, 59, 16);
    labels[1]  = new QLabel("Setpoint", gbRF); labels[1]->setGeometry( 8,  48, 59, 16);
    labels[2]  = new QLabel("Freq",     gbRF); labels[2]->setGeometry( 8,  73, 59, 16);
    labels[3]  = new QLabel("RF+",      gbRF); labels[3]->setGeometry( 8,  96, 59, 16);
    labels[4]  = new QLabel("RF-",      gbRF); labels[4]->setGeometry( 8, 118, 59, 16);
    labels[5]  = new QLabel("Power",    gbRF); labels[5]->setGeometry( 8, 142, 59, 16);
    labels[6]  = new QLabel("%",        gbRF); labels[6]->setGeometry(159,  26, 31, 21);
    labels[7]  = new QLabel("V",        gbRF); labels[7]->setGeometry(159,  48, 31, 21);
    labels[8]  = new QLabel("Hz",       gbRF); labels[8]->setGeometry(159,  73, 31, 21);
    labels[9]  = new QLabel("Vp-p",     gbRF); labels[9]->setGeometry(159,  96, 31, 21);
    labels[10] = new QLabel("Vp-p",     gbRF); labels[10]->setGeometry(159, 118, 31, 21);
    labels[11] = new QLabel("W",        gbRF); labels[11]->setGeometry(159, 142, 31, 21);

    Drive    = new QLineEdit(gbRF); Drive->setGeometry(   60,  22, 91, 21); Drive->setValidator(new QDoubleValidator);
    Setpoint = new QLineEdit(gbRF); Setpoint->setGeometry(60,  46, 91, 21); Setpoint->setValidator(new QDoubleValidator);
    Freq     = new QLineEdit(gbRF); Freq->setGeometry(    60,  70, 91, 21); Freq->setValidator(new QIntValidator);
    RFP      = new QLineEdit(gbRF); RFP->setGeometry(     60,  94, 91, 21); RFP->setReadOnly(true);
    RFN      = new QLineEdit(gbRF); RFN->setGeometry(     60, 118, 91, 21); RFN->setReadOnly(true);
    Power    = new QLineEdit(gbRF); Power->setGeometry(   60, 142, 91, 21); Power->setReadOnly(true);

    Open_Loop   = new QRadioButton("Open loop",  gbRF); Open_Loop->setGeometry(  10, 166, 91, 21);
    Closed_Loop = new QRadioButton("Close loop", gbRF); Closed_Loop->setGeometry(100, 166, 91, 21);

    Tune   = new QPushButton("Tune",   gbRF); Tune->setGeometry(  10, 190, 81, 32); Tune->setAutoDefault(false);
    Retune = new QPushButton("Retune", gbRF); Retune->setGeometry(102, 190, 81, 32); Retune->setAutoDefault(false);

    connect(Drive,       &QLineEdit::returnPressed,    this, &RFCchannel::DriveChange);
    connect(Setpoint,    &QLineEdit::returnPressed,    this, &RFCchannel::SetpointChange);
    connect(Freq,        &QLineEdit::returnPressed,    this, &RFCchannel::FreqChange);
    connect(Tune,        &QPushButton::pressed,        this, &RFCchannel::TunePressed);
    connect(Retune,      &QPushButton::pressed,        this, &RFCchannel::RetunePressed);
    connect(Open_Loop,   &QRadioButton::clicked,       this, &RFCchannel::rbChange);
    connect(Closed_Loop, &QRadioButton::clicked,       this, &RFCchannel::rbChange);

    gbRF->installEventFilter(this);
    gbRF->setMouseTracking(true);
    Drive->installEventFilter(this);
    Drive->setMouseTracking(true);
    Setpoint->installEventFilter(this);
    Setpoint->setMouseTracking(true);
    Freq->installEventFilter(this);
    Freq->setMouseTracking(true);
}

// eventFilter — handles drag-to-move via moveWidget() and mouse-wheel value
// adjustment on Drive (step 1), Setpoint (step 1), and Freq (step 1000 Hz).
bool RFCchannel::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, gbRF, gbRF, event)) return true;
    if(Updating) return true;
    UpdateOff = true;
    if(adjustValue(obj, Drive,    event, 1))    { UpdateOff = false; return true; }
    if(adjustValue(obj, Setpoint, event, 1))    { UpdateOff = false; return true; }
    UpdateOff = false;
    if(adjustValue(obj, Freq,     event, 1000)) { UpdateOff = false; return true; }
    UpdateOff = false;
    return QObject::eventFilter(obj, event);
}

// Returns a comma-separated string: Name,RFdrive,RFsetpoint,RFfreq,RFvolt+,RFvolt-,Power
QString RFCchannel::Report(void)
{
    QString title;
    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;

    QString res = title + "," + (isShutdown ? activeDrive : Drive->text());
    res += "," + Freq->text() + "," + Setpoint->text() + "," + RFP->text() + "," + RFN->text() + "," + Power->text();
    return res;
}

// Sets RFCchannel parameters if the name matches and at least 4 comma-separated values are present.
bool RFCchannel::SetValues(QString strVals)
{
    QString title;
    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!strVals.startsWith(title)) return false;

    QStringList resList = strVals.split(",");
    if(resList.count() < 4) return false;

    Freq->setText(resList[2]);
    Freq->setModified(true);
    emit Freq->editingFinished();
    if(isShutdown)
    {
        activeDrive    = resList[1];
        activeSetpoint = resList[3];
    }
    else
    {
        Drive->setText(resList[1]);
        Drive->setModified(true);
        emit Drive->editingFinished();
        Setpoint->setText(resList[3]);
        Setpoint->setModified(true);
        emit Setpoint->editingFinished();
    }
    return true;
}

// Processes scripting commands for this channel:
//   title.Drive, title.Setpoint, title.Freq  — read or set
//   title.RF+, title.RF-, title.Power         — read only
//   title.color=<css>                          — set group box background colour
// Returns "?" if the command was not handled.
QString RFCchannel::ProcessCommand(QString cmd)
{
    QString title;
    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";

    if(cmd == title + ".Drive")    return Drive->text();
    if(cmd == title + ".Setpoint") return Setpoint->text();
    if(cmd == title + ".Freq")     return Freq->text();
    if(cmd == title + ".RF+")      return RFP->text();
    if(cmd == title + ".RF-")      return RFN->text();
    if(cmd == title + ".Power")    return Power->text();

    QStringList resList = cmd.split("=");
    if(resList.count() == 2)
    {
        if(cmd.startsWith(title + ".Drive"))
        {
            Drive->setText(resList[1]);
            Drive->setModified(true);
            emit Drive->editingFinished();
            return "";
        }
        if(cmd.startsWith(title + ".Setpoint"))
        {
            Setpoint->setText(resList[1]);
            Setpoint->setModified(true);
            emit Setpoint->editingFinished();
            return "";
        }
        if(cmd.startsWith(title + ".Freq"))
        {
            Freq->setText(resList[1]);
            Freq->setModified(true);
            emit Freq->editingFinished();
            return "";
        }
        if(cmd.startsWith(title + ".color"))
        {
            gbRF->setStyleSheet("background-color: " + resList[1].trimmed());
            return "";
        }
    }
    return "?";
}

// Updates display fields from MIPS or from a pre-fetched comma-separated value string.
// sVals format: freq,drive,RF+,RF-,Power  (all optional — missing values are read from MIPS).
// Setpoint and open/closed loop mode are always read from MIPS; they are never passed in sVals.
void RFCchannel::Update(QString sVals)
{
    QString     res;
    QStringList sValsList;
    bool        ok;

    sValsList = (sVals == "") ? QStringList() : sVals.split(",");
    if(comms == NULL) return;
    if(UpdateOff) return;
    Updating = true;
    comms->rb.clear();

    if(sValsList.count() < 2) res = comms->SendMess("GRFDRV,"  + QString::number(Channel) + "\n");
    else                      res = sValsList[1];
    res.toFloat(&ok);
    if(!Drive->hasFocus() && res != "" && ok) Drive->setText(res);

    if(sValsList.count() < 1) res = comms->SendMess("GRFFRQ,"  + QString::number(Channel) + "\n");
    else                      res = sValsList[0];
    res.toInt(&ok);
    if(!Freq->hasFocus() && res != "" && ok) Freq->setText(res);

    if(sValsList.count() < 3) res = comms->SendMess("GRFPPVP," + QString::number(Channel) + "\n");
    else                      res = sValsList[2];
    res.toFloat(&ok);
    if(res != "" && ok) RFP->setText(res);

    if(sValsList.count() < 4) res = comms->SendMess("GRFPPVN," + QString::number(Channel) + "\n");
    else                      res = sValsList[3];
    res.toFloat(&ok);
    if(res != "" && ok) RFN->setText(res);

    if(sValsList.count() < 5) res = comms->SendMess("GRFPWR,"  + QString::number(Channel) + "\n");
    else                      res = sValsList[4];
    res.toFloat(&ok);
    if(res != "" && ok) Power->setText(res);

    // Setpoint and loop mode are always read from MIPS
    res = comms->SendMess("GRFVLT,"  + QString::number(Channel) + "\n");
    res.toFloat(&ok);
    if(!Setpoint->hasFocus() && res != "" && ok) Setpoint->setText(res);

    res = comms->SendMess("GRFMODE," + QString::number(Channel) + "\n");
    if(res == "AUTO")
    {
        Closed_Loop->setChecked(true);
        Drive->setReadOnly(true);
        Setpoint->setReadOnly(false);
        Tune->setDisabled(true);
        Retune->setDisabled(true);
    }
    if(res == "MANUAL")
    {
        Open_Loop->setChecked(true);
        Drive->setReadOnly(false);
        Setpoint->setReadOnly(true);
        Tune->setDisabled(false);
        Retune->setDisabled(false);
    }
    Updating = false;
    gbRF->repaint();
}

// DriveChange — slot for Drive returnPressed. Sends SRFDRV,channel,value.
void RFCchannel::DriveChange(void)
{
    if(comms == NULL) return;
    if(!Drive->isModified()) return;
    comms->SendCommand("SRFDRV," + QString::number(Channel) + "," + Drive->text() + "\n");
    Drive->setModified(false);
}

// SetpointChange — slot for Setpoint returnPressed. Sends SRFVLT,channel,value.
void RFCchannel::SetpointChange(void)
{
    if(comms == NULL) return;
    if(!Setpoint->isModified()) return;
    comms->SendCommand("SRFVLT," + QString::number(Channel) + "," + Setpoint->text() + "\n");
    Setpoint->setModified(false);
}

// FreqChange — slot for Freq returnPressed. Sends SRFFRQ,channel,value.
void RFCchannel::FreqChange(void)
{
    if(comms == NULL) return;
    if(!Freq->isModified()) return;
    comms->SendCommand("SRFFRQ," + QString::number(Channel) + "," + Freq->text() + "\n");
    Freq->setModified(false);
}

// TunePressed — prompts the user and initiates a full RF head tune sequence
// (zeros drive, waits 1 s, sends TUNERFCH,channel).
void RFCchannel::TunePressed(void)
{
    QMessageBox msgBox;

    Tune->setDown(false);
    QString msg = "This function will tune the RF head attached to " + MIPSnm + " channel " + QString::number(Channel) + ". ";
    msg += "Make sure the RF head is attached and connected to your system as needed. ";
    msg += "This process can take up to 3 minutes.\n";
    msgBox.setText(msg);
    msgBox.setInformativeText("Are you sure you want to continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::No) return;

    // Zero the drive and wait 1 s before tuning
    if(comms != NULL) comms->SendCommand("SRFDRV," + QString::number(Channel) + ",0\n");
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() < 1000) QApplication::processEvents();

    if(comms != NULL) comms->SendCommand("TUNERFCH," + QString::number(Channel) + "\n");
}

// RetunePressed — prompts the user and sends RETUNERFCH,channel for a fast
// retune of the existing resonance without a full frequency sweep.
void RFCchannel::RetunePressed(void)
{
    QMessageBox msgBox;

    Retune->setDown(false);
    QString msg = "This function will retune the RF head attached to " + MIPSnm + " channel " + QString::number(Channel) + ". ";
    msg += "Make sure the RF head is attached and connected to your system as needed. ";
    msg += "This process can take up to 1 minute.\n";
    msgBox.setText(msg);
    msgBox.setInformativeText("Are you sure you want to continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::No) return;
    if(comms != NULL) comms->SendCommand("RETUNERFCH," + QString::number(Channel) + "\n");
}

// rbChange — slot for the open/closed-loop radio buttons. Sends SRFMODE,channel,AUTO|MANUAL
// and updates widget read-only states to match the new mode.
void RFCchannel::rbChange(void)
{
    QString res = "SRFMODE,";
    if(Closed_Loop->isChecked())
    {
        res += QString::number(Channel) + ",AUTO\n";
        Drive->setReadOnly(true);
        Setpoint->setReadOnly(false);
        Tune->setDisabled(true);
        Retune->setDisabled(true);
    }
    if(Open_Loop->isChecked())
    {
        res += QString::number(Channel) + ",MANUAL\n";
        Drive->setReadOnly(false);
        Setpoint->setReadOnly(true);
        Tune->setDisabled(false);
        Retune->setDisabled(false);
    }
    if(comms == NULL) return;
    comms->SendCommand(res);
}

// Shutdown — saves Drive and Setpoint, then zeroes both for safe power-down.
// Restore — returns Drive and Setpoint to their saved values.
void RFCchannel::Shutdown(void)
{
    if(isShutdown) return;
    isShutdown     = true;
    activeDrive    = Drive->text();
    activeSetpoint = Setpoint->text();
    Drive->setText("0");
    Drive->setModified(true);
    emit Drive->editingFinished();
    Setpoint->setText("0");
    Setpoint->setModified(true);
    emit Setpoint->editingFinished();
}

void RFCchannel::Restore(void)
{
    if(!isShutdown) return;
    isShutdown = false;
    Drive->setText(activeDrive);
    Drive->setModified(true);
    emit Drive->editingFinished();
    Setpoint->setText(activeSetpoint);
    Setpoint->setModified(true);
    emit Setpoint->editingFinished();
}
