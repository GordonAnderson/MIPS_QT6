// =============================================================================
// twave.cpp
//
// Twave tab controller for the MIPS host application.
//
// Depends on:  twave.h, comms.h, ui_mips.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "twave.h"

// Twave parameter limits used for range clamping in Changed()
static const float TWAVE_PULSE_VOLTAGE_MIN  =       7.0f;
static const float TWAVE_PULSE_VOLTAGE_MAX  =     100.0f;
static const float TWAVE_GUARD_VOLTAGE_MIN  =       0.0f;
static const float TWAVE_GUARD_VOLTAGE_MAX  =     100.0f;
static const float TWAVE_FREQUENCY_MIN      =    5000.0f;
static const float TWAVE_FREQUENCY_MAX      = 2000000.0f;
static const float TWAVE_ORDER_MIN          =       0.0f;
static const float TWAVE_ORDER_MAX          =     255.0f;

// -----------------------------------------------------------------------------
// Constructor — connects all widget signals to slots.
// -----------------------------------------------------------------------------
Twave::Twave(Ui::MIPS *w, Comms *c)
{
    tui   = w;
    comms = c;

    Updating    = false;
    UpdateOff   = false;
    NumChannels = 2;
    Compressor  = true;

    QObjectList widgetList = tui->gbTwaveCH1->children();
    widgetList += tui->gbTwaveCH2->children();
    widgetList += tui->tabCompressor->children();
    widgetList += tui->gbTiming->children();
    widgetList += tui->gbTWsweepCH1->children();
    widgetList += tui->gbTWsweepCH2->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("le"))
            connect(((QLineEdit *)w), &QLineEdit::editingFinished, this, &Twave::Changed);
    }

    connect(tui->rbSTWCMODE_COMPRESS,   &QRadioButton::clicked, this, &Twave::rbModeCompress);
    connect(tui->rbSTWCMODE_NORMAL,     &QRadioButton::clicked, this, &Twave::rbModeNormal);
    connect(tui->rbSTWCSW_CLOSE,        &QRadioButton::clicked, this, &Twave::rbSwitchClose);
    connect(tui->rbSTWCSW_OPEN,         &QRadioButton::clicked, this, &Twave::rbSwitchOpen);
    connect(tui->pbTwaveUpdate,         &QPushButton::pressed,  this, &Twave::pbUpdate);
    connect(tui->pbTwaveForceTrigger,   &QPushButton::pressed,  this, &Twave::pbForceTrigger);
    connect(tui->rbSTWDIR_1_FWD,        &QRadioButton::clicked, this, &Twave::rbTW1fwd);
    connect(tui->rbSTWDIR_1_REV,        &QRadioButton::clicked, this, &Twave::rbTW1rev);
    connect(tui->rbSTWDIR_2_FWD,        &QRadioButton::clicked, this, &Twave::rbTW2fwd);
    connect(tui->rbSTWDIR_2_REV,        &QRadioButton::clicked, this, &Twave::rbTW2rev);
    connect(tui->pbTWsweepCH1start,     &QPushButton::pressed,  this, &Twave::pbTWsweepStart);
    connect(tui->pbTWsweepCH2start,     &QPushButton::pressed,  this, &Twave::pbTWsweepStart);
    connect(tui->pbTWsweepStart,        &QPushButton::pressed,  this, &Twave::pbTWsweepStart);
    connect(tui->pbTWsweepCH1stop,      &QPushButton::pressed,  this, &Twave::pbTWsweepStop);
    connect(tui->pbTWsweepCH2stop,      &QPushButton::pressed,  this, &Twave::pbTWsweepStop);
    connect(tui->pbTWsweepStop,         &QPushButton::pressed,  this, &Twave::pbTWsweepStop);
    connect(tui->chkSweepExtTrig,       &QCheckBox::clicked,    this, &Twave::SweepExtTrigger);
}

// -----------------------------------------------------------------------------
// myEventFilter — handles up/down arrow key value nudging on leSTW* widgets.
// Modifier keys scale the delta: Alt=0.1x, Ctrl=10x, Shift=100x.
// Frequency fields use a base delta of 10000; all others use 10.
// -----------------------------------------------------------------------------
bool Twave::myEventFilter(QObject *obj, QEvent *event)
{
    QLineEdit *le;
    float delta = 0;

    if(obj->objectName().startsWith("leSTW") && (event->type() == QEvent::KeyPress))
    {
        if(obj->objectName().startsWith("leSTWSEQ")) return QObject::eventFilter(obj, event);
        if(Updating) return true;
        UpdateOff = true;
        le = (QLineEdit *)obj;
        QKeyEvent *key = static_cast<QKeyEvent *>(event);
        if(key->key() == Qt::Key_Up)   delta =  0.1f;
        if(key->key() == Qt::Key_Down) delta = -0.1f;
        if((QApplication::queryKeyboardModifiers() & 0xA000000) != 0) delta *= 0.1f;
        else if((QApplication::queryKeyboardModifiers() & 0x2000000) != 0) delta *= 10.0f;
        else if((QApplication::queryKeyboardModifiers() & 0x8000000) != 0) delta *= 100.0f;
        if(obj->objectName().startsWith("leSTWF")) delta *= 10000.0f;
        else delta *= 10.0f;
        if(delta != 0)
        {
            QString myString;
            if((obj->objectName().startsWith("leSTWPV")) || (obj->objectName().startsWith("leSTWG")))
                myString = QString::asprintf("%3.2f", le->text().toFloat() + delta);
            else
                myString = QString::asprintf("%1.0f", le->text().toFloat() + delta);
            le->setText(myString);
            le->setModified(true);
            emit le->editingFinished();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

// -----------------------------------------------------------------------------
// SweepExtTrigger — enables or disables external trigger for frequency sweep.
// -----------------------------------------------------------------------------
void Twave::SweepExtTrigger(void)
{
    if(tui->chkSweepExtTrig->isChecked())
    {
        comms->SendCommand("SDTRIGDLY,0\n");
        comms->SendCommand("SDTRIGPRD,10000\n");
        comms->SendCommand("SDTRIGRPT,1\n");
        comms->SendCommand("SDTRIGMOD,SWEEP\n");
        comms->SendCommand("SDTRIGINP,R,POS\n");
        comms->SendCommand("SDTRIGENA,TRUE\n");
    }
    else
    {
        comms->SendCommand("SDTRIGENA,FALSE\n");
    }
}

// -----------------------------------------------------------------------------
// Save — writes all leS* and rbS* widget values to a text settings file.
// -----------------------------------------------------------------------------
void Twave::Save(QString Filename)
{
    QString res;

    if(NumChannels <= 0) return;
    if(Filename == "") return;
    QFile file(Filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << "# DCbias settings, " + QDateTime::currentDateTime().toString() + "\n";

        QObjectList widgetList = tui->gbTwaveCH1->children();
        if(NumChannels > 1) widgetList += tui->gbTwaveCH2->children();
        if(Compressor)
        {
            widgetList += tui->tabCompressor->children();
            widgetList += tui->gbTWsweepCH1->children();
            widgetList += tui->gbTWsweepCH2->children();
            widgetList += tui->gbTiming->children();
            widgetList += tui->gbMode->children();
            widgetList += tui->gbSwitch->children();
        }
        foreach(QObject *w, widgetList)
        {
            if(w->objectName().mid(0, 3) == "leS")
            {
                stream << w->objectName() + "," + ((QLineEdit *)w)->text() + "\n";
            }
            if(w->objectName().mid(0, 3) == "rbS")
            {
                res = w->objectName() + ",";
                res += ((QRadioButton *)w)->isChecked() ? "true\n" : "false\n";
                stream << res;
            }
        }
        file.close();
        tui->statusBar->showMessage("Settings saved to " + Filename, 2000);
    }
}

// -----------------------------------------------------------------------------
// Load — reads a settings file and restores leS* and rbS* widget values.
// -----------------------------------------------------------------------------
void Twave::Load(QString Filename)
{
    QStringList resList;

    if(NumChannels <= 0) return;
    if(Filename == "") return;

    QObjectList widgetList = tui->gbTwaveCH1->children();
    if(NumChannels > 1) widgetList += tui->gbTwaveCH2->children();
    if(Compressor)
    {
        widgetList += tui->tabCompressor->children();
        widgetList += tui->gbTWsweepCH1->children();
        widgetList += tui->gbTWsweepCH2->children();
        widgetList += tui->gbTiming->children();
        widgetList += tui->gbMode->children();
        widgetList += tui->gbSwitch->children();
    }
    QFile file(Filename);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line    = stream.readLine();
            resList = line.split(",");
            if(resList.count() == 2)
            {
                foreach(QObject *w, widgetList)
                {
                    if(w->objectName().mid(0, 3) == "leS")
                    {
                        if(resList[1] != "" && w->objectName() == resList[0])
                        {
                            ((QLineEdit *)w)->setText(resList[1]);
                            ((QLineEdit *)w)->setModified(true);
                            QMetaObject::invokeMethod(w, "editingFinished");
                        }
                    }
                    if(w->objectName().mid(0, 3) == "rbS")
                    {
                        if(w->objectName() == resList[0])
                        {
                            if(resList[1] == "true")
                            {
                                ((QRadioButton *)w)->setChecked(true);
                                QMetaObject::invokeMethod(w, "clicked");
                            }
                            else ((QRadioButton *)w)->setChecked(false);
                        }
                    }
                }
            }
        } while(!line.isNull());
        file.close();
        tui->statusBar->showMessage("Settings loaded from " + Filename, 2000);
    }
}

// -----------------------------------------------------------------------------
// Update — polls all Twave parameters from MIPS and refreshes the UI.
// Enables/disables UI groups based on NumChannels and Compressor flag.
// -----------------------------------------------------------------------------
void Twave::Update(void)
{
    QString res;

    if(UpdateOff) return;
    Updating = true;
    switch(NumChannels)
    {
    case 0:
        tui->gbTwaveCH1->setEnabled(false);
        tui->gbTwaveCH2->setEnabled(false);
        tui->tabCompressor->setEnabled(false);
        tui->tabSweep->setEnabled(false);
        return;
    case 1:
        tui->gbTwaveCH1->setEnabled(true);
        tui->gbTwaveCH2->setEnabled(false);
        tui->tabCompressor->setEnabled(false);
        tui->tabSweep->setEnabled(true);
        // fall through
    case 2:
        tui->gbTwaveCH1->setEnabled(true);
        tui->gbTwaveCH2->setEnabled(true);
        if(Compressor) tui->tabCompressor->setEnabled(true);
        tui->tabSweep->setEnabled(true);
        // fall through
    default:
        break;
    }

    tui->tabMIPS->setEnabled(false);
    tui->statusBar->showMessage(tr("Updating Twave IO controls..."));

    QObjectList widgetList = tui->gbTwaveCH1->children();
    if(NumChannels > 1) widgetList += tui->gbTwaveCH2->children();
    widgetList += tui->gbTWsweepCH1->children();
    widgetList += tui->gbTWsweepCH2->children();
    if(Compressor)
    {
        widgetList += tui->tabCompressor->children();
        widgetList += tui->gbTiming->children();
    }
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("le"))
        {
            res = "G" + w->objectName().mid(3).replace("_", ",") + "\n";
            ((QLineEdit *)w)->setText(comms->SendMess(res));
            comms->waitforline(1);
            res = comms->rb.getline();
        }
    }

    res = comms->SendMess("GTWDIR,1\n");
    if(res == "FWD") tui->rbSTWDIR_1_FWD->setChecked(true);
    if(res == "REV") tui->rbSTWDIR_1_REV->setChecked(true);
    res = comms->SendMess("GTWDIR,2\n");
    if(res == "FWD") tui->rbSTWDIR_2_FWD->setChecked(true);
    if(res == "REV") tui->rbSTWDIR_2_REV->setChecked(true);

    if(Compressor)
    {
        res = comms->SendMess("GTWCMODE\n");
        if(res == "Normal")   tui->rbSTWCMODE_NORMAL->setChecked(true);
        if(res == "Compress") tui->rbSTWCMODE_COMPRESS->setChecked(true);
        res = comms->SendMess("GTWCSW\n");
        if(res == "Open")  tui->rbSTWCSW_OPEN->setChecked(true);
        if(res == "Close") tui->rbSTWCSW_CLOSE->setChecked(true);
    }

    tui->tabMIPS->setEnabled(true);
    tui->statusBar->showMessage(tr(""));
    Updating = false;
}

// -----------------------------------------------------------------------------
// Changed — slot called when any leSTW* line edit finishes editing.
// Clamps to valid range then sends the updated value to MIPS.
// -----------------------------------------------------------------------------
void Twave::Changed(void)
{
    QObject *obj = sender();
    QString res;

    if(Updating) return;
    if(!((QLineEdit *)obj)->isModified()) return;

    if(obj->objectName().startsWith("leSTWPV"))
    {
        float v = ((QLineEdit *)obj)->text().toFloat();
        if(v < TWAVE_PULSE_VOLTAGE_MIN) ((QLineEdit *)obj)->setText(QString::number(TWAVE_PULSE_VOLTAGE_MIN));
        if(v > TWAVE_PULSE_VOLTAGE_MAX) ((QLineEdit *)obj)->setText(QString::number(TWAVE_PULSE_VOLTAGE_MAX));
    }
    if(obj->objectName().startsWith("leSTWG"))
    {
        float v = ((QLineEdit *)obj)->text().toFloat();
        if(v < TWAVE_GUARD_VOLTAGE_MIN) ((QLineEdit *)obj)->setText(QString::number(TWAVE_GUARD_VOLTAGE_MIN));
        if(v > TWAVE_GUARD_VOLTAGE_MAX) ((QLineEdit *)obj)->setText(QString::number(TWAVE_GUARD_VOLTAGE_MAX));
    }
    if(obj->objectName().startsWith("leSTWF"))
    {
        float v = ((QLineEdit *)obj)->text().toFloat();
        if(v < TWAVE_FREQUENCY_MIN) ((QLineEdit *)obj)->setText(QString::number(TWAVE_FREQUENCY_MIN));
        if(v > TWAVE_FREQUENCY_MAX) ((QLineEdit *)obj)->setText(QString::number(TWAVE_FREQUENCY_MAX));
    }
    if(obj->objectName().startsWith("leSTWCORDER"))
    {
        float v = ((QLineEdit *)obj)->text().toFloat();
        if(v < TWAVE_ORDER_MIN) ((QLineEdit *)obj)->setText(QString::number(TWAVE_ORDER_MIN));
        if(v > TWAVE_ORDER_MAX) ((QLineEdit *)obj)->setText(QString::number(TWAVE_ORDER_MAX));
    }

    res = obj->objectName().mid(2).replace("_", ",") + "," + ((QLineEdit *)obj)->text() + "\n";
    comms->SendCommand(res.toStdString().c_str());
    ((QLineEdit *)obj)->setModified(false);
    UpdateOff = false;
}

// -----------------------------------------------------------------------------
// Compressor mode and switch slot handlers
// -----------------------------------------------------------------------------
void Twave::rbModeCompress(void) { comms->SendCommand("STWCMODE,Compress\n"); }
void Twave::rbModeNormal(void)   { comms->SendCommand("STWCMODE,Normal\n"); }
void Twave::rbSwitchClose(void)  { comms->SendCommand("STWCSW,Close\n"); }
void Twave::rbSwitchOpen(void)   { comms->SendCommand("STWCSW,Open\n"); }

// -----------------------------------------------------------------------------
// Direction slot handlers
// -----------------------------------------------------------------------------
void Twave::rbTW1fwd(void) { comms->SendCommand("STWDIR,1,FWD\n"); }
void Twave::rbTW1rev(void) { comms->SendCommand("STWDIR,1,REV\n"); }
void Twave::rbTW2fwd(void) { comms->SendCommand("STWDIR,2,FWD\n"); }
void Twave::rbTW2rev(void) { comms->SendCommand("STWDIR,2,REV\n"); }

void Twave::pbUpdate(void)       { Update(); }
void Twave::pbForceTrigger(void) { comms->SendCommand("TWCTRG\n"); }

// -----------------------------------------------------------------------------
// pbTWsweepStart — starts the sweep for CH1, CH2, or both depending on sender.
// -----------------------------------------------------------------------------
void Twave::pbTWsweepStart(void)
{
    QString name = sender()->objectName();
    if(name == "pbTWsweepCH1start") comms->SendCommand("STWSGO,1\n");
    if(name == "pbTWsweepCH2start") comms->SendCommand("STWSGO,2\n");
    if(name == "pbTWsweepStart")    comms->SendCommand("STWSGO,3\n");
}

// -----------------------------------------------------------------------------
// pbTWsweepStop — stops the sweep for CH1, CH2, or both depending on sender.
// -----------------------------------------------------------------------------
void Twave::pbTWsweepStop(void)
{
    QString name = sender()->objectName();
    if(name == "pbTWsweepCH1stop") comms->SendCommand("STWSHLT,1\n");
    if(name == "pbTWsweepCH2stop") comms->SendCommand("STWSHLT,2\n");
    if(name == "pbTWsweepStop")    comms->SendCommand("STWSHLT,3\n");
}
