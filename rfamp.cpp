#include "rfamp.h"
#include "ui_rfamp.h"
#include "Utilities.h"

RFamp::RFamp(QWidget *parent, QString name, QString MIPSname, int Module) :
    QDialog(parent),
    ui(new Ui::RFamp)
{
    p = parent;
    ui->setupUi(this);
    Updating = false;
    UpdateOff = false;
    isShutdown = false;
    comms = NULL;
    Title  = name;
    MIPSnm = MIPSname;
    Channel = Module;

    ui->gbRFamp->setTitle(Title);
    ui->gbRFamp->setToolTip(MIPSnm + " Module: " + QString::number(Module));
    QObjectList widgetList = ui->tabRFsettings->children();
    widgetList += ui->tabMassFilter->children();
    foreach(QObject *w, widgetList)
    {
       if(w->objectName().contains("leS")) connect(((QLineEdit *)w),SIGNAL(editingFinished()),this,SLOT(Updated()));
       else if(w->objectName().contains("chk")) connect(((QCheckBox *)w),SIGNAL(toggled(bool)),this,SLOT(Updated()));
       else if(w->objectName().contains("rb")) connect(((QRadioButton *)w),SIGNAL(clicked(bool)),this,SLOT(Updated()));
    }
    connect(ui->pbRFAupdate,SIGNAL(pressed()),this,SLOT(slotUpdate()));
    ui->leSRFAFREQ->installEventFilter(this);
    ui->leSRFAFREQ->setMouseTracking(true);
    ui->leSRFARES->installEventFilter(this);
    ui->leSRFARES->setMouseTracking(true);
    ui->leSRFAR0->installEventFilter(this);
    ui->leSRFAR0->setMouseTracking(true);
    ui->leSRFAK->installEventFilter(this);
    ui->leSRFAK->setMouseTracking(true);
    ui->gbRFamp->setMouseTracking(true);
}

RFamp::~RFamp()
{
    delete ui;
}

bool RFamp::eventFilter(QObject *obj, QEvent *event)
{
    if (((obj != ui->leSRFAFREQ) && (obj != ui->leSRFARES) && (obj != ui->leSRFAR0) && (obj != ui->leSRFAK))) return QObject::eventFilter(obj, event);
    if(Updating) return true;
    UpdateOff = true;
    float mult = 10;
    if (obj == ui->leSRFAR0) mult /= 1000;
    if (obj == ui->leSRFAK) mult /= 1000;
    if(adjustValue(obj,(QLineEdit *)obj,event,mult))
    {
        UpdateOff = false;
        return true;
    }
    UpdateOff = false;
    return QObject::eventFilter(obj, event);
}

void RFamp::Update(void)
{
    QStringList resList;
    QString res;
    QString CurrentList;
    int i;
    static bool inited = false;
    QObjectList widgetList;

    if(comms==NULL) return;
    if(UpdateOff) return;
    Updating = true;
    // Update all input boxes for the selected tab
    if(inited) widgetList = ui->tabRFquad->currentWidget()->children();
    else
    {
        widgetList = ui->tabRFsettings->children();
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
            // Checkbox names encode the command and response for check and uncheck
            // This has a bug in the PWR command, March 4, 2021
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
            // This is a list of parameters from a command common to several line edit boxes
            resList = w->objectName().split("_");
            if(resList.count() == 2)
            {
                i = resList[1].toInt();
                res = resList[0].mid(3);
                if(!CurrentList.startsWith(res)) CurrentList = res + "," + comms->SendMess(res + "," + QString::number(Channel) + "\n");
                resList = CurrentList.split(",");
                if(resList.count() > i) ((QLineEdit *)w)->setText(resList[i]);
            }
        }
    }
    Updating = false;
    inited = true;
}

void RFamp::Updated(void)
{
    QObject*    obj = sender();
    QString     res;
    QStringList resList;

    if(comms==NULL) return;
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
            if(((QRadioButton *)obj)->isChecked()) comms->SendCommand(resList[0] + "," + QString::number(Channel) + "," + resList[1] + "\n");
        }
    }
}

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
           if(resList.count() == 2) if(((QRadioButton *)w)->isChecked()) res += title + "," + resList[0] + "," + resList[1] + "\n";
       }
   }
   return res;
}

bool RFamp::SetValues(QString strVals)
{
    QStringList resList,ctrlList;
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
                    if((ctrlList[0] == resList[1]) && (ctrlList[1] == resList[2])) {((QCheckBox *)w)->setChecked(true); return(true);}
                    if((ctrlList[0] == resList[1]) && (ctrlList[2] == resList[2])) {((QCheckBox *)w)->setChecked(false); return(true);}
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

void RFamp::slotUpdate(void)
{
    if(comms == NULL) return;
    comms->SendCommand("RFAQUPDATE," + QString::number(Channel) + "\n");
}

void RFamp::Shutdown(void)
{
    if(isShutdown) return;
    isShutdown = true;
    activeEnableState = ui->chkSRFAENA_ON_OFF->checkState();
    ui->chkSRFAENA_ON_OFF->setChecked(false);
    emit ui->chkSRFAENA_ON_OFF->checkStateChanged(Qt::Unchecked);
    // Disable the DC supply
    ui->chkSDCPWR_ON_OFF->setChecked(false);
    emit ui->chkSDCPWR_ON_OFF->checkStateChanged(Qt::Unchecked);
}

void RFamp::Restore(void)
{
    if(!isShutdown) return;
    isShutdown = false;
    ui->chkSRFAENA_ON_OFF->setChecked(activeEnableState);
    if(activeEnableState) emit ui->chkSRFAENA_ON_OFF->checkStateChanged(Qt::Checked);
    else emit ui->chkSRFAENA_ON_OFF->checkStateChanged(Qt::Unchecked);
    // Enable the DC supply
    ui->chkSDCPWR_ON_OFF->setChecked(true);
    emit ui->chkSDCPWR_ON_OFF->checkStateChanged(Qt::Checked);
}
