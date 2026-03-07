// =============================================================================
// pse.cpp
//
// Pulse sequence editor (PSE) classes for the MIPS host application.
//
// Depends on:  pse.h, ui_pse.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "pse.h"
#include "ui_pse.h"

#include <QMessageBox>
#include <QFile>

static const int PSG_DEFAULT_DELTA_T = 100; // Default time step (clock counts) when inserting a point at end

// ---------------------------------------------------------------------------
// psgPoint serialisation operators
// ---------------------------------------------------------------------------

// operator<< — serialises a psgPoint to a binary QDataStream.
QDataStream &operator<<(QDataStream &out, const psgPoint &point)
{
    out << point.Name << quint32(point.TimePoint);
    for(int i = 0; i < PSG_CHANNELS; i++) out << point.DigitalO[i];
    for(int i = 0; i < PSG_CHANNELS; i++) out << point.DCbias[i];
    out << point.Loop << point.LoopName << quint32(point.LoopCount);
    return out;
}

// operator>> — deserialises a psgPoint from a binary QDataStream.
QDataStream &operator>>(QDataStream &in, psgPoint &point)
{
    quint32 TimePoint;
    quint32 LoopCount;

    in >> point.Name >> TimePoint;
    for(int i = 0; i < PSG_CHANNELS; i++) in >> point.DigitalO[i];
    for(int i = 0; i < PSG_CHANNELS; i++) in >> point.DCbias[i];
    in >> point.Loop >> point.LoopName >> LoopCount;

    point.TimePoint = TimePoint;
    point.LoopCount = LoopCount;
    return in;
}

// ---------------------------------------------------------------------------
// psgPoint
// ---------------------------------------------------------------------------

// psgPoint — constructor. Initialises all channels to zero/false and clears name fields.
psgPoint::psgPoint()
{
    for(int i = 0; i < PSG_CHANNELS; i++)
    {
        DigitalO[i] = false;
        DCbias[i]   = 0.0;
    }
    Loop      = false;
    LoopCount = 0;
    Name      = "";
    LoopName  = "";
    TimePoint = 0;
}

// ---------------------------------------------------------------------------
// pseDialog
// ---------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Constructor — connects dynamically-named DIO checkboxes and DC bias line
// edits, then displays the first pulse sequence point.
// -----------------------------------------------------------------------------
pseDialog::pseDialog(QList<psgPoint *> *psg, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pseDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());

    p            = psg;
    activePoint  = (*psg)[0];
    CurrentIndex = 0;
    UpdateDialog(activePoint);

    QObjectList widgetList = ui->gbDigitalOut->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("chkDO"))
            connect(((QCheckBox *)w), &QCheckBox::clicked, this, &pseDialog::on_DIO_checked);
    }
    ui->gbCurrentPoint->setTitle("Current time point: " + QString::number(CurrentIndex + 1) + " of " + QString::number(p->size()));

    widgetList = ui->gbDCbias->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("leDCB"))
            connect(((QLineEdit *)w), &QLineEdit::textEdited, this, &pseDialog::on_DCBIAS_edited);
    }
    connect(ui->pbInsertPulse,   &QPushButton::pressed, this, &pseDialog::InsertPulse);
    connect(ui->pbInsertRamp,    &QPushButton::pressed, this, &pseDialog::MakeRamp);
    connect(ui->pbInsertCancel,  &QPushButton::pressed, this, &pseDialog::RampCancel);
    ui->gbInsertPulse->setVisible(false);
}

// ~pseDialog — destructor. Releases the UI form.
pseDialog::~pseDialog()
{
    delete ui;
}

// -----------------------------------------------------------------------------
// on_DCBIAS_edited — updates the active point's DC bias value for the channel
// whose line edit was changed. Channel index is encoded in the widget name.
// -----------------------------------------------------------------------------
void pseDialog::on_DCBIAS_edited()
{
    QObject *obj = sender();
    int Index = QStringView{obj->objectName()}.mid(5).toInt() - 1;
    activePoint->DCbias[Index] = ((QLineEdit *)obj)->text().toFloat();
}

// -----------------------------------------------------------------------------
// on_DIO_checked — updates the active point's digital output state for the
// channel whose checkbox was toggled. Channel letter is encoded in widget name.
// -----------------------------------------------------------------------------
void pseDialog::on_DIO_checked()
{
    QObject *obj = sender();
    int Index = (int)obj->objectName().mid(5, 1).toStdString().c_str()[0] - (int)'A';
    activePoint->DigitalO[Index] = ((QCheckBox *)obj)->isChecked();
}

// -----------------------------------------------------------------------------
// UpdateDialog — refreshes all dialog controls from a psgPoint.
// -----------------------------------------------------------------------------
void pseDialog::UpdateDialog(psgPoint *point)
{
    QList<psgPoint*>::iterator it;
    int Index;
    QString temp = point->LoopName;

    ui->comboLoop->clear();
    ui->comboLoop->addItem("");
    for(it = p->begin(); it != p->end(); ++it)
    {
        if(*it == point) break;
        ui->comboLoop->addItem((*it)->Name);
    }
    Index = ui->comboLoop->findText(temp);
    if(Index != -1) ui->comboLoop->setCurrentIndex(Index);
    ui->leName->setText(point->Name);
    ui->leClocks->setText(QString::number(point->TimePoint));
    ui->leCycles->setText(QString::number(point->LoopCount));
    ui->chkLoop->setChecked(point->Loop);
    ui->comboLoop->setCurrentText(point->LoopName);

    QObjectList widgetList = ui->gbDigitalOut->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("chkDO"))
        {
            Index = (int)w->objectName().mid(5, 1).toStdString().c_str()[0] - (int)'A';
            ((QCheckBox *)w)->setChecked(point->DigitalO[Index]);
        }
    }

    widgetList = ui->gbDCbias->children();
    foreach(QObject *w, widgetList)
    {
        if(w->objectName().contains("leDCB"))
        {
            Index = QStringView{w->objectName()}.mid(5).toInt() - 1;
            ((QLineEdit *)w)->setText(QString::asprintf("%.2f", point->DCbias[Index]));
        }
    }
}

// on_pbNext_pressed — advances to the next time point and refreshes the dialog.
void pseDialog::on_pbNext_pressed()
{
    if(CurrentIndex < p->size() - 1) CurrentIndex++;
    activePoint = (*p)[CurrentIndex];
    UpdateDialog(activePoint);
    ui->gbCurrentPoint->setTitle("Current time point: " + QString::number(CurrentIndex + 1) + " of " + QString::number(p->size()));
}

// on_pbPrevious_pressed — moves back to the previous time point and refreshes the dialog.
void pseDialog::on_pbPrevious_pressed()
{
    if(CurrentIndex > 0) CurrentIndex--;
    activePoint = (*p)[CurrentIndex];
    UpdateDialog(activePoint);
    ui->gbCurrentPoint->setTitle("Current time point: " + QString::number(CurrentIndex + 1) + " of " + QString::number(p->size()));
}

// -----------------------------------------------------------------------------
// on_pbInsert_pressed — inserts a new point after the current point.
// Auto-generates a unique name by incrementing a trailing _N suffix.
// -----------------------------------------------------------------------------
void pseDialog::on_pbInsert_pressed()
{
    psgPoint *point = new psgPoint;
    QList<psgPoint*>::iterator it;
    int i, deltaT;

    *point = *activePoint;

    // Generate a unique name by incrementing a trailing _N suffix
    if((i = point->Name.lastIndexOf("_")) < 0)
    {
        point->Name += "_1";
    }
    else
    {
        point->Name = point->Name.mid(0, i + 1) + QString::number(QStringView{point->Name}.mid(i + 1).toInt() + 1);
    }
    // Ensure the generated name is unique
    for(it = p->begin(); it != p->end(); ++it)
    {
        if((*it)->Name == point->Name) { point->Name = activePoint->Name + "_1"; break; }
    }

    for(it = p->begin(); it != p->end(); ++it) if(*it == activePoint) break;
    int ctime = point->TimePoint;
    if(CurrentIndex >= p->size() - 1) deltaT = PSG_DEFAULT_DELTA_T;
    else deltaT = ((*p)[CurrentIndex + 1]->TimePoint - point->TimePoint) / 2;
    it++;
    p->insert((QList<psgPoint*>::const_iterator)it, point);
    CurrentIndex++;
    activePoint          = point;
    point->TimePoint     = ctime + deltaT;
    point->Loop          = false;
    point->LoopCount     = 0;
    point->LoopName      = "";
    UpdateDialog(activePoint);
    ui->gbCurrentPoint->setTitle("Current time point: " + QString::number(CurrentIndex + 1) + " of " + QString::number(p->size()));
}

// -----------------------------------------------------------------------------
// on_pbDelete_pressed — deletes the current point. Refuses if only one remains.
// -----------------------------------------------------------------------------
void pseDialog::on_pbDelete_pressed()
{
    QList<psgPoint*>::iterator it, itnext;

    if(p->size() <= 1)
    {
        QMessageBox::information(NULL, "Error!", "Can't delete the only point in the sequence!");
        ui->pbDelete->setDown(false);
        return;
    }
    for(it = p->begin(); it != p->end(); ++it) if(*it == activePoint) break;
    itnext      = p->erase((QList<psgPoint*>::const_iterator)it);
    activePoint = *itnext;
    CurrentIndex = 0;
    for(it = p->begin(); it != p->end(); ++it, ++CurrentIndex) if(*it == activePoint) break;
    if(CurrentIndex >= p->size()) CurrentIndex = p->size() - 1;
    activePoint = (*p)[CurrentIndex];
    UpdateDialog(activePoint);
    ui->gbCurrentPoint->setTitle("Current time point: " + QString::number(CurrentIndex + 1) + " of " + QString::number(p->size()));
}

// on_leName_textChanged — updates the active point's Name as the user types.
void pseDialog::on_leName_textChanged(const QString &arg1)
{
    activePoint->Name = arg1;
}

// on_leClocks_textChanged — updates the active point's TimePoint clock count.
void pseDialog::on_leClocks_textChanged(const QString &arg1)
{
    activePoint->TimePoint = arg1.toInt();
}

// on_leCycles_textChanged — updates the active point's loop repeat count.
void pseDialog::on_leCycles_textChanged(const QString &arg1)
{
    activePoint->LoopCount = arg1.toInt();
}

// on_chkLoop_clicked — sets or clears the active point's Loop flag.
void pseDialog::on_chkLoop_clicked(bool checked)
{
    activePoint->Loop = checked;
}

// on_comboLoop_currentIndexChanged — sets the active point's LoopName to the
// selected combo box entry (the loop target time-point name).
void pseDialog::on_comboLoop_currentIndexChanged(const QString &arg1)
{
    activePoint->LoopName = arg1;
}

// -----------------------------------------------------------------------------
// InsertPulse — shows the pulse/ramp insertion panel, hides the main controls.
// -----------------------------------------------------------------------------
void pseDialog::InsertPulse(void)
{
    ui->gbInsertPulse->setVisible(true);
    ui->frmLoop->setVisible(false);
    ui->gbDigitalOut->setVisible(false);
    ui->gbDCbias->setVisible(false);
}

// -----------------------------------------------------------------------------
// MakeRamp — generates a voltage ramp pulse on the selected DC bias channel.
// Inserts intermediate time points for the rise, hold, and fall phases.
// -----------------------------------------------------------------------------
void pseDialog::MakeRamp(void)
{
    float restingV, pulseV;
    int   channel, width;

    channel = ui->lePulseChannel->text().toInt() - 1;
    if((channel < 0) || (channel > PSG_CHANNELS - 1))
    {
        QMessageBox::information(NULL, "Error!", "Invalid channel number!");
        return;
    }
    restingV = activePoint->DCbias[channel];
    pulseV   = ui->lePulseVoltage->text().toFloat();
    width    = ui->lePulseWidth->text().toInt();

    // Subtract half the rise and fall times from the flat-top pulse width
    if(ui->leRampUp->text().toInt() > 1)  width -= (ui->leRampStepSize->text().toInt() * ui->leRampUp->text().toInt()) / 2;
    if(ui->leRampDwn->text().toInt() > 1) width -= (ui->leRampStepSize->text().toInt() * ui->leRampDwn->text().toInt()) / 2;
    if(width < 10)
    {
        QMessageBox::information(NULL, "Error!", "For the defined rise and fall times pulse width must be at least "
                                                     + QString::number(ui->lePulseWidth->text().toInt() - width + 10));
        return;
    }

    if(ui->leRampUp->text().toInt() <= 1)
    {
        activePoint->DCbias[channel] = pulseV;
        on_pbInsert_pressed();
        activePoint->TimePoint -= PSG_DEFAULT_DELTA_T;
    }
    else
    {
        for(int i = 0; i < ui->leRampUp->text().toInt(); i++)
        {
            activePoint->DCbias[channel] += ((pulseV - restingV) / (ui->leRampUp->text().toInt()));
            activePoint->TimePoint += ui->leRampStepSize->text().toInt();
            UpdateDialog(activePoint);
            on_pbInsert_pressed();
            activePoint->TimePoint -= PSG_DEFAULT_DELTA_T;
        }
    }

    // Hold at pulse voltage for the flat-top width
    activePoint->TimePoint += width - PSG_DEFAULT_DELTA_T;

    if(ui->leRampDwn->text().toInt() <= 1)
    {
        activePoint->DCbias[ui->lePulseChannel->text().toInt()] = restingV;
        on_pbInsert_pressed();
    }
    else
    {
        for(int i = 0; i < ui->leRampDwn->text().toInt(); i++)
        {
            activePoint->DCbias[channel] -= ((pulseV - restingV) / (ui->leRampDwn->text().toInt()));
            activePoint->TimePoint += ui->leRampStepSize->text().toInt();
            UpdateDialog(activePoint);
            on_pbInsert_pressed();
            activePoint->TimePoint -= PSG_DEFAULT_DELTA_T;
        }
        activePoint->TimePoint += PSG_DEFAULT_DELTA_T;
    }

    activePoint->DCbias[channel] = restingV;
    UpdateDialog(activePoint);
    ui->gbInsertPulse->setVisible(false);
    ui->frmLoop->setVisible(true);
    ui->gbDigitalOut->setVisible(true);
    ui->gbDCbias->setVisible(true);
}

// -----------------------------------------------------------------------------
// RampCancel — cancels the pulse/ramp insertion and restores the main controls.
// -----------------------------------------------------------------------------
void pseDialog::RampCancel(void)
{
    UpdateDialog(activePoint);
    ui->gbInsertPulse->setVisible(false);
    ui->frmLoop->setVisible(true);
    ui->gbDigitalOut->setVisible(true);
    ui->gbDCbias->setVisible(true);
}
