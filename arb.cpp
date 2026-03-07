// =============================================================================
// arb.cpp
//
// ARB (Arbitrary Waveform Generator) module classes for the MIPS host app.
//
// Depends on:  arb.h, help.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "arb.h"
#include "help.h"
#include "Utilities.h"

#include <QProcess>
#include <qthread.h>

// *************************************************************************************************
// ARB  ********************************************************************************************
// *************************************************************************************************

ARB::ARB(Ui::MIPS *w, Comms *c)
{
    aui   = w;
    comms = c;

    LogedData  = new Help();
    Compressor = true;
    PPP        = 32;

    // Populate the module selection combo box (up to 4 modules of 8 channels each)
    aui->comboARBmodule->clear();
    for(int i = 0; i < 4; i++)
    {
        if(((i + 1) * 8) <= NumChannels) aui->comboARBmodule->addItem(QString::number(i + 1));
    }

    // Populate the waveform type combo box
    aui->comboSWFTYP->clear();
    aui->comboSWFTYP->addItem("SIN",   "SIN");
    aui->comboSWFTYP->addItem("RAMP",  "RAMP");
    aui->comboSWFTYP->addItem("TRI",   "TRI");
    aui->comboSWFTYP->addItem("PULSE", "PULSE");
    aui->comboSWFTYP->addItem("ARB",   "ARB");

    // Connect all leS* fields in the module and compressor group boxes to ARBUpdated
    QObjectList widgetList  = aui->gbARBmodule1->children();
    widgetList             += aui->gbARBmodule2->children();
    widgetList             += aui->gbARBcompressor->children();
    widgetList             += aui->gbARBtiming->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("leS"))
            connect(qobject_cast<QLineEdit *>(w), &QLineEdit::returnPressed, this, &ARB::ARBUpdated);
    }

    // Connect all leS* fields in the Twave and dual-output group boxes to ARBUpdatedParms
    widgetList  = aui->gbARBtwaveParms->children();
    widgetList += aui->gbDualOutput->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("leS"))
            connect(qobject_cast<QLineEdit *>(w), &QLineEdit::returnPressed, this, &ARB::ARBUpdatedParms);
    }

    connect(aui->pbSetChannel,          &QPushButton::pressed,              this, &ARB::SetARBchannel);
    connect(aui->pbSetRangeChannel,     &QPushButton::pressed,              this, &ARB::SetARBchannelRange);
    connect(aui->pbARBtrigger,          &QPushButton::pressed,              this, &ARB::ARBtrigger);
    connect(aui->pbSetChannel_2,        &QPushButton::pressed,              this, &ARB::SetARBchannel_2);
    connect(aui->pbSetRangeChannel_2,   &QPushButton::pressed,              this, &ARB::SetARBchannelRange_2);
    connect(aui->pbARBtrigger_2,        &QPushButton::pressed,              this, &ARB::ARBtrigger_2);
    connect(aui->pbARBviewLog,          &QPushButton::pressed,              this, &ARB::ARBviewLog);
    connect(aui->pbARBclearLog,         &QPushButton::pressed,              this, &ARB::ARBclearLog);
    connect(aui->pbARBupdate,           &QPushButton::pressed,              this, &ARB::ARBupdate);
    connect(aui->pbARBtwaveUpdate,      &QPushButton::pressed,              this, &ARB::ARBupdate);
    connect(aui->pbEditARBwf,           &QPushButton::pressed,              this, &ARB::EditARBwaveform);
    connect(aui->tabARB,                &QTabWidget::currentChanged,        this, &ARB::ARBtabSelected);
    connect(aui->comboSWFTYP,           &QComboBox::currentIndexChanged,    this, &ARB::ARBtypeSelected);
    connect(aui->comboARBmodule,        &QComboBox::currentIndexChanged,    this, &ARB::ARBmoduleSelected);
    connect(aui->rbSWFDIR_FWD,         &QRadioButton::clicked,             this, &ARB::rbTWfwd);
    connect(aui->rbSWFDIR_REV,         &QRadioButton::clicked,             this, &ARB::rbTWrev);
    // Compressor controls
    connect(aui->rbSARBCMODE_COMPRESS, &QRadioButton::clicked,             this, &ARB::rbModeCompress);
    connect(aui->rbSARBCMODE_NORMAL,   &QRadioButton::clicked,             this, &ARB::rbModeNormal);
    connect(aui->rbSARBCSW_CLOSE,      &QRadioButton::clicked,             this, &ARB::rbSwitchClose);
    connect(aui->rbSARBCSW_OPEN,       &QRadioButton::clicked,             this, &ARB::rbSwitchOpen);
    connect(aui->pbARBforceTrigger,    &QPushButton::pressed,              this, &ARB::pbForceTrigger);
}

void ARB::SetNumberOfChannels(int num)
{
    NumChannels = num;
    aui->comboARBmodule->clear();
    for(int i = 0; i < 4; i++)
    {
        if(((i + 1) * 8) <= NumChannels) aui->comboARBmodule->addItem(QString::number(i + 1));
    }
}

void ARB::pbForceTrigger(void)   { comms->SendCommand("TARBTRG\n"); }
void ARB::rbModeCompress(void)   { comms->SendCommand("SARBCMODE,Compress\n"); }
void ARB::rbModeNormal(void)     { comms->SendCommand("SARBCMODE,Normal\n"); }
void ARB::rbSwitchClose(void)    { comms->SendCommand("SARBCSW,Close\n"); }
void ARB::rbSwitchOpen(void)     { comms->SendCommand("SARBCSW,Open\n"); }
void ARB::rbTWfwd(void)          { comms->SendCommand("SWFDIR,1,FWD\n"); }
void ARB::rbTWrev(void)          { comms->SendCommand("SWFDIR,1,REV\n"); }

void ARB::ARBtypeSelected(void)
{
    comms->SendCommand("SWFTYP," + aui->comboARBmodule->currentText() + "," + aui->comboSWFTYP->currentText() + "\n");
}

void ARB::ARBtrigger(void)
{
    comms->SendCommand("SWFDIS,1\n");
    comms->SendCommand("SWFENA,1\n");
}

void ARB::ARBtrigger_2(void)
{
    comms->SendCommand("SWFDIS,2\n");
    comms->SendCommand("SWFENA,2\n");
}

void ARB::ARBtabSelected(void)
{
    if(aui->tabARB->tabText(aui->tabARB->currentIndex()) == "ARB mode")
    {
        if(NumChannels > 0) comms->SendCommand("SARBMODE,1,ARB\n");
        if(NumChannels > 8) comms->SendCommand("SARBMODE,2,ARB\n");
        QThread::msleep(2000);
    }
    if(aui->tabARB->tabText(aui->tabARB->currentIndex()) == "Twave mode")
    {
        if(NumChannels > 0) comms->SendCommand("SARBMODE,1,TWAVE\n");
        if(NumChannels > 8) comms->SendCommand("SARBMODE,2,TWAVE\n");
        QThread::msleep(2000);
    }
    Update();
}

void ARB::SetARBchannel(void)
{
    QString res = aui->leChan->text();
    if(res.toInt() == 0)
    {
        res = "SARBCHS,1," + aui->leLevel->text() + "\n";
        LogString += res;
        comms->SendCommand(res);
        return;
    }
    res = "SARBCH,1," + aui->leChan->text() + "," + aui->leLevel->text() + "\n";
    LogString += res;
    comms->SendCommand(res);
}

void ARB::SetARBchannel_2(void)
{
    QString res = aui->leChan->text();
    if(res.toInt() == 0)
    {
        res = "SARBCHS,2," + aui->leLevel_2->text() + "\n";
        LogString += res;
        comms->SendCommand(res);
        return;
    }
    res = "SARBCH,2," + aui->leChan_2->text() + "," + aui->leLevel_2->text() + "\n";
    LogString += res;
    comms->SendCommand(res);
}

void ARB::SetARBchannelRange(void)
{
    QString res = "SACHRNG,1," + aui->leRangeChan->text() + "," + aui->leRangeStart->text()
                  + "," + aui->leRangeStop->text() + "," + aui->leRangeLevel->text() + "\n";
    LogString += res;
    comms->SendCommand(res);
}

void ARB::SetARBchannelRange_2(void)
{
    QString res = "SACHRNG,2," + aui->leRangeChan_2->text() + "," + aui->leRangeStart_2->text()
                  + "," + aui->leRangeStop_2->text() + "," + aui->leRangeLevel_2->text() + "\n";
    LogString += res;
    comms->SendCommand(res);
}

void ARB::ARBmoduleSelected(void)
{
    QString res;
    if(aui->tabMIPS->tabText(aui->tabMIPS->currentIndex()) != "ARB") return;
    QObjectList widgetList = aui->gbARBtwaveParms->children();
    QString chan = aui->comboARBmodule->currentText();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().startsWith("le"))
        {
            res = "G" + w->objectName().mid(3) + "," + chan;
            qobject_cast<QLineEdit *>(w)->setText(comms->SendMess(res + "\n"));
        }
    }
    res = comms->SendMess("GWFDIR," + chan + "\n");
    if(res == "FWD") aui->rbSWFDIR_FWD->setChecked(true);
    if(res == "REV") aui->rbSWFDIR_REV->setChecked(true);
    res = comms->SendMess("GWFTYP," + chan + "\n");
    aui->comboSWFTYP->setCurrentIndex(aui->comboSWFTYP->findData(res));
}

// Slot connected to leS* fields in the module/compressor/timing group boxes.
// Derives the MIPS command name from the widget's objectName (strips "le" prefix,
// replaces underscores with commas) and sends the new value.
void ARB::ARBUpdated(void)
{
    QObject* obj = sender();
    if(!qobject_cast<QLineEdit *>(obj)->isModified()) return;
    QString res = obj->objectName().mid(2).replace("_", ",");
    if(res.right(1) == ",") res = res.left(res.length() - 1);
    res += "," + qobject_cast<QLineEdit *>(obj)->text() + "\n";
    comms->SendCommand(res);
    qobject_cast<QLineEdit *>(obj)->setModified(false);
}

// Slot connected to leS* fields in the Twave/dual-output group boxes.
// Prepends the current module number to the command.
void ARB::ARBUpdatedParms(void)
{
    QString chan = aui->comboARBmodule->currentText();
    QObject* obj = sender();
    if(!qobject_cast<QLineEdit *>(obj)->isModified()) return;
    QString res = obj->objectName().mid(2) + "," + chan + "," + qobject_cast<QLineEdit *>(obj)->text() + "\n";
    comms->SendCommand(res);
    qobject_cast<QLineEdit *>(obj)->setModified(false);
}

void ARB::ARBupdate(void)
{
    Update();
}

void ARB::Update(void)
{
    QString res;
    QObjectList widgetList;

    if(aui->tabMIPS->tabText(aui->tabMIPS->currentIndex()) != "ARB") return;
    res = comms->SendMess("GCHAN,ARB\n");
    NumChannels = res.toInt();
    if(NumChannels == 0)
    {
        aui->gbARBmodule1->setEnabled(false);
        aui->gbARBmodule2->setEnabled(false);
        return;
    }
    aui->gbARBmodule1->setEnabled(true);
    if(NumChannels > 8) aui->gbARBmodule2->setEnabled(true);

    res = comms->SendMess("GARBMODE,1\n");
    if(res == "ARB")   aui->tabARB->setCurrentIndex(1);
    if(res == "TWAVE") aui->tabARB->setCurrentIndex(0);

    if(aui->tabARB->tabText(aui->tabARB->currentIndex()) == "ARB mode")
    {
        QObjectList wl = aui->gbARBmodule1->children();
        if(NumChannels > 8) wl += aui->gbARBmodule2->children();
        foreach(QObject *w, wl)
        {
            if(w->objectName().contains("le"))
            {
                res = "G" + w->objectName().mid(3).replace("_", ",") + "\n";
                qobject_cast<QLineEdit *>(w)->setText(comms->SendMess(res));
            }
        }
    }
    if(aui->tabARB->tabText(aui->tabARB->currentIndex()) == "Twave mode")
    {
        if(NumChannels >= 16) aui->gbARBcompressor->setEnabled(Compressor);
        else                  aui->gbARBcompressor->setEnabled(false);

        if(Compressor)
        {
            widgetList  = aui->gbARBcompressor->children();
            widgetList += aui->gbARBtiming->children();
            foreach(QObject *w, widgetList)
            {
                if(w->objectName().contains("le"))
                {
                    res = "G" + w->objectName().mid(3).replace("_", ",");
                    if(res.right(1) == ",") res = res.left(res.length() - 1);
                    qobject_cast<QLineEdit *>(w)->setText(comms->SendMess(res + "\n"));
                }
            }
        }

        QString chan = aui->comboARBmodule->currentText();
        if(comms->SendMess("GARBOFFA," + chan + "\n").contains("?"))
            aui->gbDualOutput->setEnabled(false);
        else
            aui->gbDualOutput->setEnabled(true);

        widgetList = aui->gbARBtwaveParms->children();
        if(aui->gbDualOutput->isEnabled()) widgetList += aui->gbDualOutput->children();
        foreach(QObject *w, widgetList)
        {
            if(w->objectName().startsWith("le"))
            {
                res = "G" + w->objectName().mid(3) + "," + chan;
                qobject_cast<QLineEdit *>(w)->setText(comms->SendMess(res + "\n"));
            }
        }

        res = comms->SendMess("GWFDIR," + chan + "\n");
        if(res == "FWD") aui->rbSWFDIR_FWD->setChecked(true);
        if(res == "REV") aui->rbSWFDIR_REV->setChecked(true);
        res = comms->SendMess("GWFTYP," + chan + "\n");
        aui->comboSWFTYP->setCurrentIndex(aui->comboSWFTYP->findData(res));
    }
    if(Compressor)
    {
        res = comms->SendMess("GARBCMODE\n");
        if(res == "Normal")   aui->rbSARBCMODE_NORMAL->setChecked(true);
        if(res == "Compress") aui->rbSARBCMODE_COMPRESS->setChecked(true);
        res = comms->SendMess("GARBCSW\n");
        if(res == "Open")  aui->rbSARBCSW_OPEN->setChecked(true);
        if(res == "Close") aui->rbSARBCSW_CLOSE->setChecked(true);
    }
}

void ARB::ARBclearLog(void)
{
    LogString = "";
}

void ARB::ARBviewLog(void)
{
    LogedData->SetTitle("Log of commands sent to MIPS");
    LogedData->LoadStr(LogString);
    LogedData->show();
}

void ARB::Save(QString Filename)
{
    if(Filename == "") return;
    QFile file(Filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QDateTime dateTime = QDateTime::currentDateTime();
        stream << "# ARB settings, " + dateTime.toString() + "\n";

        QObjectList widgetList = aui->gbARBtwaveParms->children();
        for(int i = 0; i < aui->comboARBmodule->count(); i++)
        {
            aui->comboARBmodule->setCurrentIndex(i);
            QApplication::processEvents();
            foreach(QObject *w, widgetList)
            {
                if(w->objectName().mid(0, 3) == "leS")
                {
                    QString res = w->objectName().mid(2) + "," + aui->comboARBmodule->currentText()
                                  + "," + qobject_cast<QLineEdit *>(w)->text() + "\n";
                    stream << res;
                }
            }
            QString res;
            if(aui->rbSWFDIR_FWD->isChecked()) res = "SWFDIR," + aui->comboARBmodule->currentText() + ",FWD\n";
            if(aui->rbSWFDIR_REV->isChecked()) res = "SWFDIR," + aui->comboARBmodule->currentText() + ",REV\n";
            stream << res;
            stream << "SWFTYP," + aui->comboARBmodule->currentText() + "," + aui->comboSWFTYP->currentText() + "\n";
        }

        widgetList  = aui->gbARBcompressor->children();
        widgetList += aui->gbARBmodule1->children();
        if(NumChannels > 8) widgetList += aui->gbARBmodule2->children();
        foreach(QObject *w, widgetList)
        {
            if(w->objectName().mid(0, 3) == "leS")
            {
                QString res = w->objectName().mid(2).replace("_", ",") + "," + qobject_cast<QLineEdit *>(w)->text() + "\n";
                stream << res;
            }
        }

        stream << "\n";
        stream << LogString;
        file.close();
        aui->statusBar->showMessage("Settings saved to " + Filename, 2000);
    }
}

void ARB::Load(QString Filename)
{
    if(Filename == "") return;
    QFile file(Filename);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            if(line.trimmed().mid(0, 1) != "#") comms->SendCommand(line + "\n");
            QApplication::processEvents();
        } while(!line.isNull());
        file.close();
        Update();
        aui->statusBar->showMessage("Settings loaded from " + Filename, 2000);
    }
}

void ARB::ReadWaveform(void)
{
    int Waveform[32];
    ARBwfEdit->GetWaveform(Waveform);
    QString cmd = "SWFARB," + aui->comboARBmodule->currentText();
    for(int i = 0; i < PPP; i++) cmd += "," + QString::number(Waveform[i]);
    cmd += "\n";
    if(!comms->SendCommand(cmd))
        aui->statusBar->showMessage("Error sending waveform to MIPS", 2000);
}

void ARB::EditARBwaveform(void)
{
    int     Waveform[32];
    QString cmd, res;

    for(int i = 0; i < 32; i++) Waveform[i] = i * 4 - 64;

    if(comms != NULL)
    {
        cmd = "GARBPPP," + aui->comboARBmodule->currentText();
        res = comms->SendMess(cmd + "\n");
        if(!res.isEmpty())
        {
            PPP = res.toInt();
            if(PPP < 8)  PPP = 8;
            if(PPP > 32) PPP = 32;
        }
    }

    res = comms->SendMess("GWFARB," + aui->comboARBmodule->currentText() + "\n");
    if(res.contains("?"))
    {
        aui->statusBar->showMessage("Error reading waveform from MIPS", 2000);
        return;
    }
    QStringList Vals = res.split(",");
    for(int i = 0; i < PPP; i++)
        Waveform[i] = (i < Vals.count()) ? Vals[i].toInt() : 0;

    ARBwfEdit = new ARBwaveformEdit(0, PPP);
    connect(ARBwfEdit, &ARBwaveformEdit::WaveformReady, this, &ARB::ReadWaveform);
    ARBwfEdit->SetWaveform(Waveform);
    ARBwfEdit->show();
}

// *************************************************************************************************
// ARBchannel  *************************************************************************************
// *************************************************************************************************

ARBchannel::ARBchannel(QWidget *parent, QString name, QString MIPSname, int x, int y) : QWidget(parent)
{
    p          = parent;
    Title      = name;
    MIPSnm     = MIPSname;
    X          = x;
    Y          = y;
    comms      = NULL;
    statusBar  = NULL;
    PPP        = 32;
    isShutdown = false;
    Updating   = false;
    UpdateOff  = false;
}

void ARBchannel::Show(void)
{
    gbARB = new QGroupBox(Title, p);
    gbARB->setGeometry(X, Y, 250, 200);
    gbARB->setToolTip(MIPSnm + " ARB channel " + QString::number(Channel));

    leSWFREQ  = new QLineEdit(gbARB); leSWFREQ->setGeometry( 100,  22, 91, 21); leSWFREQ->setValidator(new QIntValidator);
    leSWFVRNG = new QLineEdit(gbARB); leSWFVRNG->setGeometry(100,  46, 91, 21); leSWFVRNG->setValidator(new QDoubleValidator);
    leSWFVAUX = new QLineEdit(gbARB); leSWFVAUX->setGeometry(100,  70, 91, 21); leSWFVAUX->setValidator(new QDoubleValidator);
    leSWFVOFF = new QLineEdit(gbARB); leSWFVOFF->setGeometry(100,  94, 91, 21); leSWFVOFF->setValidator(new QDoubleValidator);

    Waveform = new QComboBox(gbARB); Waveform->setGeometry(100, 118, 91, 21);
    Waveform->clear();
    Waveform->addItem("SIN",   "SIN");
    Waveform->addItem("RAMP",  "RAMP");
    Waveform->addItem("TRI",   "TRI");
    Waveform->addItem("PULSE", "PULSE");
    Waveform->addItem("ARB",   "ARB");

    EditWaveform = new QPushButton("Edit",    gbARB); EditWaveform->setGeometry(100, 142, 91, 30); EditWaveform->setAutoDefault(false);
    SWFDIR_FWD   = new QRadioButton("Forward", gbARB); SWFDIR_FWD->setGeometry( 20, 166, 91, 21);
    SWFDIR_REV   = new QRadioButton("Reverse", gbARB); SWFDIR_REV->setGeometry(150, 166, 91, 21);

    labels[0] = new QLabel("Frequency",    gbARB); labels[0]->setGeometry(10,  26, 80, 16);
    labels[1] = new QLabel("Amplitude",    gbARB); labels[1]->setGeometry(10,  48, 80, 16);
    labels[2] = new QLabel("Aux output",   gbARB); labels[2]->setGeometry(10,  73, 80, 16);
    labels[3] = new QLabel("Offset output",gbARB); labels[3]->setGeometry(10,  96, 80, 16);
    labels[4] = new QLabel("Waveform",     gbARB); labels[4]->setGeometry(10, 118, 80, 16);
    labels[5] = new QLabel("Hz",   gbARB); labels[5]->setGeometry(200,  26, 31, 21);
    labels[6] = new QLabel("Vp-p", gbARB); labels[6]->setGeometry(200,  48, 31, 21);
    labels[7] = new QLabel("V",    gbARB); labels[7]->setGeometry(200,  73, 31, 21);
    labels[8] = new QLabel("V",    gbARB); labels[8]->setGeometry(200,  96, 31, 21);

    leSWFREQ->setObjectName("leSWFREQ");
    leSWFVRNG->setObjectName("leSWFVRNG");
    leSWFVAUX->setObjectName("leSWFVAUX");
    leSWFVOFF->setObjectName("leSWFVOFF");
    SWFDIR_FWD->setObjectName("SWFDIR_FWD");
    SWFDIR_REV->setObjectName("SWFDIR_REV");

    foreach(QObject *w, gbARB->children())
    {
        if(w->objectName().startsWith("le"))
            connect(qobject_cast<QLineEdit *>(w), &QLineEdit::returnPressed, this, &ARBchannel::leChange);
    }
    connect(Waveform,     &QComboBox::currentIndexChanged,  this, &ARBchannel::wfChange);
    connect(EditWaveform, &QPushButton::pressed,            this, &ARBchannel::wfEdit);
    connect(SWFDIR_FWD,   &QRadioButton::clicked,           this, &ARBchannel::rbChange);
    connect(SWFDIR_REV,   &QRadioButton::clicked,           this, &ARBchannel::rbChange);

    gbARB->installEventFilter(this);
    gbARB->setMouseTracking(true);
    leSWFREQ->installEventFilter(this);  leSWFREQ->setMouseTracking(true);
    leSWFVRNG->installEventFilter(this); leSWFVRNG->setMouseTracking(true);
    leSWFVAUX->installEventFilter(this); leSWFVAUX->setMouseTracking(true);
    leSWFVOFF->installEventFilter(this); leSWFVOFF->setMouseTracking(true);
}

bool ARBchannel::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, gbARB, gbARB, event)) return true;
    if((obj != leSWFREQ) && (obj != leSWFVRNG) && (obj != leSWFVAUX) && (obj != leSWFVOFF))
        return QObject::eventFilter(obj, event);
    if(Updating) return true;
    UpdateOff = true;
    float mult = 10;
    if(obj == leSWFREQ) mult *= 10;
    if(adjustValue(obj, qobject_cast<QLineEdit *>(obj), event, mult))
    {
        UpdateOff = false;
        return true;
    }
    UpdateOff = false;
    return QObject::eventFilter(obj, event);
}

// Returns a comma-separated string:
//   Name,Frequency,Amplitude,AuxOutput,OffsetOutput,Direction,Waveform
QString ARBchannel::Report(void)
{
    QString title;
    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;

    QString res = title + "," + leSWFREQ->text() + ",";
    if(isShutdown)
        res += activeVRNG + "," + activeVAUX + "," + activeVOFF + ",";
    else
        res += leSWFVRNG->text() + "," + leSWFVAUX->text() + "," + leSWFVOFF->text() + ",";
    res += (SWFDIR_FWD->isChecked() ? "FWD," : "REV,");
    res += Waveform->currentText();
    return res;
}

bool ARBchannel::SetValues(QString strVals)
{
    QString title;
    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!strVals.startsWith(title)) return false;

    QStringList resList = strVals.split(",");
    if(resList[0] != title) return false;
    if(resList.count() < 7) return false;

    leSWFREQ->setText(resList[1]); leSWFREQ->setModified(true); emit leSWFREQ->editingFinished();
    if(isShutdown)
    {
        activeVRNG = resList[2];
        activeVAUX = resList[3];
        activeVOFF = resList[4];
    }
    else
    {
        leSWFVRNG->setText(resList[2]); leSWFVRNG->setModified(true); emit leSWFVRNG->editingFinished();
        leSWFVAUX->setText(resList[3]); leSWFVAUX->setModified(true); emit leSWFVAUX->editingFinished();
        leSWFVOFF->setText(resList[4]); leSWFVOFF->setModified(true); emit leSWFVOFF->editingFinished();
    }
    if(resList[5] == "FWD") { SWFDIR_FWD->setChecked(true); emit SWFDIR_FWD->clicked(true); }
    else if(resList[5] == "REV") { SWFDIR_REV->setChecked(true); emit SWFDIR_REV->clicked(true); }
    int i = Waveform->findData(resList[6]);
    Waveform->setCurrentIndex(i);
    emit Waveform->currentIndexChanged(i);
    return true;
}

void ARBchannel::Update(void)
{
    QString res;

    if(comms == NULL) return;
    comms->rb.clear();
    if(UpdateOff) return;
    Updating = true;

    // Read all line edit values from MIPS
    foreach(QObject *w, gbARB->children())
    {
        if(w->objectName().startsWith("le"))
        {
            res = comms->SendMess("G" + w->objectName().mid(3) + "," + QString::number(Channel) + "\n");
            QLineEdit *le = qobject_cast<QLineEdit *>(w);
            if(!le->hasFocus() && res != "") le->setText(res);
        }
    }

    // Update the waveform selection box
    res = comms->SendMess("GWFTYP," + QString::number(Channel) + "\n");
    if(res != "") Waveform->setCurrentIndex(Waveform->findData(res));

    // Update the direction radio buttons
    res = comms->SendMess("GWFDIR," + QString::number(Channel) + "\n");
    if(res == "FWD") SWFDIR_FWD->setChecked(true);
    if(res == "REV") SWFDIR_REV->setChecked(true);
    Updating = false;
}

// Slot connected to returnPressed on all leS* fields in the group box.
// Derives the MIPS command from the widget's objectName and sends the value.
void ARBchannel::leChange(void)
{
    QObject* obj = sender();
    if(comms == NULL) return;
    QLineEdit *le = qobject_cast<QLineEdit *>(obj);
    if(!le->isModified()) return;
    QString res = obj->objectName().mid(2) + "," + QString::number(Channel) + "," + le->text() + "\n";
    comms->SendCommand(res);
    le->setModified(false);
}

void ARBchannel::rbChange(void)
{
    if(comms == NULL) return;
    QString res = "SWFDIR," + QString::number(Channel);
    res += (SWFDIR_FWD->isChecked() ? ",FWD\n" : ",REV\n");
    comms->SendCommand(res);
}

void ARBchannel::wfChange(void)
{
    if(comms == NULL) return;
    comms->SendCommand("SWFTYP," + QString::number(Channel) + "," + Waveform->currentText() + "\n");
}

void ARBchannel::ReadWaveform(void)
{
    int Wform[32];
    ARBwfEdit->GetWaveform(Wform);
    QString cmd = "SWFARB," + QString::number(Channel);
    for(int i = 0; i < PPP; i++) cmd += "," + QString::number(Wform[i]);
    cmd += "\n";
    if(comms == NULL) return;
    if(!comms->SendCommand(cmd))
    {
        if(statusBar != NULL) statusBar->showMessage("Error sending waveform to MIPS", 2000);
    }
}

void ARBchannel::wfEdit(void)
{
    int Wform[32];
    QString cmd, res;

    for(int i = 0; i < 32; i++) Wform[i] = i * 4 - 64;

    if(comms != NULL)
    {
        cmd = "GARBPPP," + QString::number(Channel);
        res = comms->SendMess(cmd + "\n");
        PPP = res.toInt();
        if(PPP < 8)  PPP = 8;
        if(PPP > 32) PPP = 32;
    }

    if(comms != NULL) res = comms->SendMess("GWFARB," + QString::number(Channel) + "\n");
    if(res.contains("?"))
    {
        if(statusBar != NULL) statusBar->showMessage("Error reading waveform from MIPS", 2000);
        return;
    }
    QStringList Vals = res.split(",");
    for(int i = 0; i < PPP; i++)
        Wform[i] = (i < Vals.count()) ? Vals[i].toInt() : 0;

    ARBwfEdit = new ARBwaveformEdit(0, PPP);
    connect(ARBwfEdit, &ARBwaveformEdit::WaveformReady, this, &ARBchannel::ReadWaveform);
    ARBwfEdit->SetWaveform(Wform);
    ARBwfEdit->show();
}

void ARBchannel::Shutdown(void)
{
    if(isShutdown) return;
    isShutdown = true;
    activeVAUX = leSWFVAUX->text();
    leSWFVAUX->setText("0"); leSWFVAUX->setModified(true); emit leSWFVAUX->editingFinished();
    activeVRNG = leSWFVRNG->text();
    leSWFVRNG->setText("0"); leSWFVRNG->setModified(true); emit leSWFVRNG->editingFinished();
    activeVOFF = leSWFVOFF->text();
    leSWFVOFF->setText("0"); leSWFVOFF->setModified(true); emit leSWFVOFF->editingFinished();
}

void ARBchannel::Restore(void)
{
    if(!isShutdown) return;
    isShutdown = false;
    leSWFVAUX->setText(activeVAUX); leSWFVAUX->setModified(true); emit leSWFVAUX->editingFinished();
    leSWFVRNG->setText(activeVRNG); leSWFVRNG->setModified(true); emit leSWFVRNG->editingFinished();
    leSWFVOFF->setText(activeVOFF); leSWFVOFF->setModified(true); emit leSWFVOFF->editingFinished();
}

// Processes scripting commands for this channel.
// Field names: Frequency, Amplitude, Aux output, Offset output, Forward, Reverse, Waveform.
// Returns "?" if the command was not handled.
QString ARBchannel::ProcessCommand(QString cmd)
{
    QLineEdit    *le = NULL;
    QRadioButton *rb = NULL;
    QString title;
    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";

    QStringList resList = cmd.split("=");
    if(resList[0].trimmed() == title + ".Frequency")     le = leSWFREQ;
    else if(resList[0].trimmed() == title + ".Amplitude")     le = leSWFVRNG;
    else if(resList[0].trimmed() == title + ".Aux output")    le = leSWFVAUX;
    else if(resList[0].trimmed() == title + ".Offset output") le = leSWFVOFF;
    else if(resList[0].trimmed() == title + ".Forward")       rb = SWFDIR_FWD;
    else if(resList[0].trimmed() == title + ".Reverse")       rb = SWFDIR_REV;
    else if(resList[0].trimmed() == title + ".Waveform")
    {
        if(resList.count() == 1) return Waveform->currentText();
        int i = Waveform->findText(resList[1].trimmed());
        if(i < 0) return "?";
        Waveform->setCurrentIndex(i);
        return "";
    }

    if(le != NULL)
    {
        if(resList.count() == 1) return le->text();
        le->setText(resList[1]);
        le->setModified(true);
        emit le->editingFinished();
        return "";
    }
    if(rb != NULL)
    {
        if(resList.count() == 1) return rb->isChecked() ? "TRUE" : "FALSE";
        if(resList[1].trimmed() == "TRUE")  rb->setChecked(true);
        if(resList[1].trimmed() == "FALSE") rb->setChecked(false);
        emit rb->clicked();
        return "";
    }
    return "?";
}
