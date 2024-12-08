#ifndef DCBIAS
#define DCBIAS

#include "ui_mips.h"
#include "mips.h"
#include "comms.h"

#include <QtCore/QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QStatusBar>
#include <QMessageBox>
#include <QObject>
#include <QApplication>
#include <QFileInfo>

class DCbias : public QDialog
{
     Q_OBJECT

public:
    DCbias(Ui::MIPS *w, Comms *c);
    void SetNumberOfChannels(int num);
    void Update(void);
    void Save(QString Filename);
    void Load(QString Filename);
    void ApplyDelta(QString GrpName, float change);

    Ui::MIPS *dui;
    Comms *comms;
    int NumChannels;
    QLineEdit *selectedLineEdit;

private:
    bool Updating;
    bool UpdateOff;

private slots:
    void DCbiasUpdated(void);
    void DCbiasPower(void);
    void UpdateDCbias(void);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class DCBchannel : public QWidget
{
    Q_OBJECT
public:
    DCBchannel(QWidget *parent, QString name, QString MIPSname, int x, int y);
    void    Show(void);
    void    Update(QString sVals = "");
    QString Report(void);
    bool    SetValues(QString strVals);
    void    Shutdown(void);
    void    Restore(void);
    QString ProcessCommand(QString cmd);
    QWidget               *p;
    QString               Title;
    int                   X,Y;
    QString               MIPSnm;
    int                   Channel;
    Comms                 *comms;
    QLineEdit             *Vsp;
    bool                  LinkEnable;
    QList<DCBchannel*>    DCBs;
    float                 CurrentVsp;
    bool                  isShutdown;
    QFrame                *frmDCB;
private:
    QLineEdit             *Vrb;
    QLabel                *labels[2];
    int                   UpdateCount;
    QString               activeVoltage;
    bool                  Updating;
    bool                  UpdateOff;
private slots:
    void VspChange(void);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class DCBoffset : public QWidget
{
    Q_OBJECT
public:
    DCBoffset(QWidget *parent, QString name, QString MIPSname, int x, int y);
    void Show(void);
    void Update(void);
    QString Report(void);
    bool SetValues(QString strVals);
    QString ProcessCommand(QString cmd);
    QWidget *p;
    QString Title;
    int     X,Y;
    QString MIPSnm;
    int     Channel;
    Comms   *comms;
    QFrame      *frmDCBO;
private:
    QLineEdit   *Voff;
    QLabel      *labels[2];
private slots:
    void VoffChange(void);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class DCBenable : public QWidget
{
    Q_OBJECT
public:
    DCBenable(QWidget *parent, QString name, QString MIPSname, int x, int y);
    void Show(void);
    void Update(void);
    QString Report(void);
    QString ProcessCommand(QString cmd);
    bool SetValues(QString strVals);
    void Shutdown(void);
    void Restore(void);
    QWidget *p;
    QString Title;
    int     X,Y;
    QString MIPSnm;
    Comms   *comms;
    bool    isShutdown;
    QFrame      *frmDCBena;
private:
    QCheckBox   *DCBena;
    bool        activeEnableState;
private slots:
    void DCBenaChange(void);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // DCBIAS

