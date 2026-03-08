#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include "comms.h"
#include "mipscomms.h"
#include "help.h"
#include "rfamp.h"
#include "tcpserver.h"
#include "arb.h"
#include "adc.h"
#include "dio.h"
#include "dcbias.h"
#include "rfdriver.h"
#include "timinggenerator.h"
#include "compressor.h"
#include "properties.h"
#include "scriptingconsole.h"
#include "plot.h"
#include "device.h"
#include "modbus.h"
#include "cmdlineapp.h"
#include "zmqworker.h"

#include <QDialog>
#include <QDebug>
#include <QLineEdit>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QPushButton>
#include <QProcess>
#include <QKeyEvent>
#include <QBoxLayout>
#include <QUdpSocket>
#include <QMenu>
#include <QTabWidget>
#include <QSemaphore>
#include <QSlider>

extern QString Version;
extern QString MakePathUnique(QString path);  // Source is located in TimingGenerator.cpp
extern QSemaphore UpdateSemaphore;

#define SKIPCOUNT   1

namespace Ui {
class ControlPanel;
}

class ScriptingConsole;

#include "textlabel.h"

#include "shutdown.h"

#include "saveload.h"

#include "dacchannel.h"

#include "dcbgroups.h"

#include "esi.h"

#include "ccontrol.h"

#include "cpanel.h"

#include "cpbutton.h"

#include "statuslight.h"

class TextMessage : public QWidget
{
    Q_OBJECT
public:
    TextMessage(QWidget *parent, QString name, int x, int y);
    void Show(void);
    void ShowMessage(void);
    QString ProcessCommand(QString cmd);
    QWidget *p;
    QString Title;
    QString MessageText;
    int     X,Y;

    QLabel *label;
private:
    QLabel *TL;
    QLabel *Message;
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class Table : public QWidget
{
    Q_OBJECT
public:
    Table(QWidget *parent, QString name, int rows, int columns, int width, int height, int x, int y);
    void Show(void);
    QString Report(void);
    bool SetValues(QString strVals);
    QString ProcessCommand(QString cmd);
    QWidget *p;
    QString Title;
    int     Width,Height;
    int     X,Y;
    int     Rows,Columns;
    QString          scriptName = "";
    QString          scriptCall = "";
signals:
    void change(QString scriptName);
private:
    QTableWidget *QTable;
public slots:
    void tableChange(int row, int column);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class Slider : public QWidget
{
    Q_OBJECT
public:
    Slider(QWidget *parent, QString name, QString orentation, int min, int max, int width, int x, int y);
    void    Show(void);
    QString Report(void);
    bool    SetValues(QString strVals);
    QString ProcessCommand(QString cmd);
    QWidget          *p;
    QString          Title;
    int              X,Y;
    QString          scriptName = "";
    QString          scriptCall = "";
    QString          Orientation;
    int              Min;
    int              Max;
    int              Width;
signals:
    void change(QString scriptName);
private:
    QSlider        *slider;
public slots:
    void sliderChange(int value);
};

typedef struct
{
   QString  Name;
   QString  Value;
} Define;

typedef struct
{
   QString  Name;
   QString  ScriptText;
} ScriptString;

class ControlPanel : public QDialog
{
    Q_OBJECT

//    Q_PROPERTY(bool UpdateHalted WRITE UpdateHalted NOTIFY UpdateHalted)
signals:
    void DialogClosed(QString NextCP);

public:
    explicit ControlPanel(QWidget *parent, QString CPfileName, QList<Comms*> S, Properties *prop, ControlPanel *pcp = NULL);
    ~ControlPanel();
    virtual void reject();
    void Update(void);
    void UpdateStateMachine(void);
    void InitMIPSsystems(QString initFilename);
    void LogDataFile(void);
    QString findFile(QString filename, QString posiblePath);
    QList<Comms*> Systems;
    QStatusBar  *statusBar;
    DCBchannel  *FindDCBchannel(QString name);
    bool LoadedConfig;
    Properties *properties;
    QList<QWidget *> Containers;
    QList<QVariant> cpObjects;
    ControlPanel *parentCP;
    //QWidget *Container;
    Q_INVOKABLE bool SystemIsShutdown;  // Cannot access this from a javascript

private:
    Comms   *FindCommPort(QString name, QList<Comms*> Systems);
    int     updateState = 0;
    int     updateIndex = 0;
    Ui::ControlPanel *ui;
    TCPserver *tcp;
    void    msDelay(int ms);
    QMenu   *contextMenu2Dplot;
    QAction *Comments;
    QAction *SaveCP;
    QAction *GeneralHelp;
    QAction *MIPScommands;
    QAction *ScriptHelp;
    QAction *ThisHelp;
    QAction *OpenLogFile;
    QAction *CloseLogFile;
    QString HelpFile;
    QString LogFile;
    QString ControlPanelFile;
    QString MIPSnames;
    QRandomGenerator generator;
    int              updateCount;

    QList<Define *>         Defines;
    QList<QTabWidget *>     Tabs;
    QList<QGroupBox *>      GroupBoxes;
    QList<ScriptButton *>   ScripButtons;
    QList<Cpanel *>         Cpanels;
    QList<TextLabel *>      TextLabels;
    QList<Table *>          Tables;
    QList<RFchannel *>      RFchans;
    QList<RFCchannel *>     RFCchans;
    QList<ADCchannel *>     ADCchans;
    QList<DACchannel *>     DACchans;
    QList<DCBchannel *>     DCBchans;
    QList<DCBoffset *>      DCBoffsets;
    QList<DCBenable *>      DCBenables;
    QList<DIOchannel *>     DIOchannels;
    QList<ESI *>            ESIchans;
    QList<ARBchannel *>     ARBchans;
    QList<RFamp *>          rfa;
    QList<Ccontrol *>       Ccontrols;
    QList<QStringList *>    CSVdata;
    QList<Plot *>           plots;
    Plot                    *activePlot = NULL;
    QList<Device *>         devices;
    QList<CPbutton *>       CPbuttons;
    QList<ScriptString *>   scriptStr;
    QList<Script *>         scriptObj;
    QList<PlotData *>       plotData;
    ModBus                  *modbus;
    QList<ModChannel *>     ModChans;
    QList<TimingControl *>  TC;
    QList<Compressor  *>    comp;
    QList<StatusLight *>    sl;
    QList<TextMessage *>    tm;
    QList<QLabel *>         iBox;
    QList<Slider *>         Sliders;
    QList<extProcess *>     extProcs;
    ZmqReqRep               zmq;
    uint                    LogStartTime;
    int                     skipCount = SKIPCOUNT;
    int                     LogPeriod;
    int                     UpdateHoldOff;
    int                     SerialWatchDog;
    bool                    UpdateStop;
    bool                    isUpdating = false;
    bool                    ShutdownFlag;
    bool                    RestoreFlag;
    bool                    StartMIPScomms;
    bool                    firstCall = true;
    QString                 CommentText = "";
    Shutdown                *SD;
    SaveLoad                *SL;
    MIPScomms               *mc;
    DCBiasGroups            *DCBgroups;
    QPushButton             *MIPScommsButton;
    QList<QPushButton *>    ARBcompressorButton;
    QPushButton             *scriptButton;
    ScriptingConsole        *scriptconsole;
    QUdpSocket              *udpSocket;
    Help                    *help;
    Help                    *comments;
    QMap<QString, QVariant> m_storage;
public slots:
    void pbSD(void);
    void pbSE(void);
    void pbSave(void);
    void pbLoad(void);
    void pbMIPScomms(void);
    void CloseMIPScomms(void);
    void scriptShutdown(void);
    void DCBgroupDisable(void);
    void DCBgroupEnable(void);
    void slotDataAcquired(QString filepath);
    void slotDataFileDefined(QString filepath);
    void pbScript(void);
    void popupHelp(QPoint);
    void slotComments(void);
    void slotCommentsFinished(void);
    void slotSaveCP(void);
    void slotGeneralHelp(void);
    void slotMIPScommands(void);
    void slotScriptHelp(void);
    void slotThisControlPanelHelp(void);
    void tcpCommand(void);
    void pbARBcompressor(void);
    void slotLogStatusBarMessage(QString);
    void slotOpenLogFile(void);
    void slotCloseLogFile(void);
    void slotCPselected(QString);
    void slotPlotDialogClosed(Plot *);
    void controlChange(QString);
    void slotExtProcessClosed(QString);
    void slotExternalProcessChange(QString);

    QString  Save(QString Filename);
    QString  Load(QString Filename);
    QString  GetLine(QString MIPSname);
    bool     SendString(QString MIPSname, QString message);
    bool     SendCommand(QString MIPSname, QString message);
    QString  SendMess(QString MIPSname, QString message);
    void     SystemEnable(void);
    void     SystemShutdown(void);
    bool     isShutDown(void);
    void     Acquire(QString filePath);
    bool     isAcquiring(void);
    void     DismissAcquire(void);
    void     statusMessage(QString message);
    void     popupMessage(QString message);
    bool     popupYesNoMessage(QString message);
    QString  popupUserInput(QString title, QString message);
    bool     sysUpdating(void);
    bool     UpdateHalted(bool stop);
    QString  SelectFile(QString fType, QString Title, QString Ext);
    int      ReadCSVfile(QString fileName, QString delimiter);
    QString  ReadCSVentry(int line, int entry);
    QString  Command(QString cmd);
    void     CreatePlot(QString Title, QString Yaxis, QString Xaxis, int NumPlots);
    void     PlotCommand(QString cmd);
    bool     isComms(void);
    QString  MakePathNameUnique(QString path);
    QString  GenerateUniqueFileName(void);
    QString  MakeFileNameUnique(QString fileName);
    QString  AppendToFile(QString fileName, QString record);
    QString  ReadFileLine(QString fileName, int line);
    QString  ReadFile(QString fileName);
    int      elapsedTime(bool init);
    QString  tcpSocket(QString cmd, QString arg1, QString arg2);
    void     setValue(const QString &key, const QVariant &value);
    QVariant getValue(const QString &key);
    bool     CreateProcess(QString name, QString program);
    QString  ZMQ(QString command);

protected:
    bool    eventFilter(QObject *obj, QEvent *event);
};

#endif // CONTROLPANEL_H
