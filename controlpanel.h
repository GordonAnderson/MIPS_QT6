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

class ESI : public QWidget
{
    Q_OBJECT
public:
    ESI(QWidget *parent, QString name, QString MIPSname, int x, int y);
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
    int     Channel;
    Comms   *comms;
    bool    isShutdown;
    QFrame      *frmESI;
private:
    QLineEdit   *ESIsp;
    QLineEdit   *ESIrb;
    QCheckBox   *ESIena;
    QLabel      *labels[2];
    bool        activeEnableState;
private slots:
    void ESIChange(void);
    void ESIenaChange(void);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class Ccontrol : public QWidget
{
    Q_OBJECT
public:
    Ccontrol(QWidget *parent, QString name, QString MIPSname,QString Type, QString Gcmd, QString Scmd, QString RBcmd, QString Units, int x, int y);
    void    Show(void);
    void    Update(void);
    QString Report(void);
    bool    SetValues(QString strVals);
    void    SetList(QString strOptions);
    void    Shutdown(void);
    void    Restore(void);
    QString ProcessCommand(QString cmd);
    QWidget          *p;
    QString          Title;
    int              X,Y;
    int              skipCount = SKIPCOUNT;
    QString          MIPSnm;
    QString          Ctype;
    QString          Dtype;
    QString          GetCmd;
    QString          SetCmd;
    QString          ReadbackCmd;
    QString          UnitsText;
    QString          ActiveValue;
    QString          ShutdownValue;
    Comms            *comms;
    bool             isShutdown;
    float            Step;
    QFrame           *frmCc=NULL;
    QPushButton      *pbButton=NULL;
    QString          scriptName = "";
    QString          scriptCall = "";
private:
    QLineEdit        *Vsp=NULL;
    QLineEdit        *Vrb;
    QCheckBox        *chkBox;
    QComboBox        *comboBox;
    QLabel           *labels[2]={NULL,NULL};
    bool             Updating;
    bool             UpdateOff;
    bool             firstUpdate = true;
    QRandomGenerator generator;
    int              updateCount;
signals:
    void change(QString scriptName);
private slots:
    void VspChange(void);
    void pbButtonPressed(void);
    void chkBoxStateChanged(Qt::CheckState);
    void comboBoxChanged(int);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

// Creates a new control panel and places a button on the parent control
// panel that will show this new control panel.
class Cpanel : public QWidget
{
    Q_OBJECT

public:
    explicit Cpanel(QWidget *parent, QString name, QString CPfileName, int x, int y, QList<Comms*> S, Properties *prop, ControlPanel *pcp);
//    ~Cpanel();
    void             Show(void);    // This will show the button, pressing the button will show the control panel
    void             Update(void);
    QString          ProcessCommand(QString cmd);
    QString          Title;
    int              X,Y;
    QWidget          *p;
    ControlPanel     *cp;
    QPushButton      *pbButton;
private slots:
    void pbButtonPressed(void);
    void slotDialogClosed(void);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#include "cpbutton.h"

class LightWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
public:
    LightWidget(const QColor &color, QWidget *parent = NULL)
        : QWidget(parent), m_color(color), m_on(false) {}

    bool isOn() const
        { return m_on; }
    void setOn(bool on)
    {
        if (on == m_on)
            return;
        m_on = on;
        update();
    }

public slots:
    void turnOff() { setOn(false); }
    void turnOn() { setOn(true); }

protected:
    void paintEvent(QPaintEvent *) override
    {
        if (!m_on)
        {
            //return;
            QColor offc;
            if(m_color == Qt::red) offc = Qt::darkRed;
            if(m_color == Qt::yellow) offc = Qt::darkYellow;
            if(m_color == Qt::green) offc = Qt::darkGreen;
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(offc);
            painter.drawEllipse(0, 0, width(), height());
            return;
        }
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(m_color);
        painter.drawEllipse(0, 0, width(), height());
    }

private:
    QColor m_color;
    bool m_on;
};

class TrafficLightWidget : public QWidget
{
public:
    TrafficLightWidget(QWidget *parent = NULL)
        : QWidget(parent)
    {
        QVBoxLayout *vbox = new QVBoxLayout(this);
        m_red = new LightWidget(Qt::red);
        vbox->addWidget(m_red);
        m_yellow = new LightWidget(Qt::yellow);
        vbox->addWidget(m_yellow);
        m_green = new LightWidget(Qt::green);
        vbox->addWidget(m_green);
        QPalette pal = palette();
        pal.setColor(QPalette::Window, Qt::gray);
        setPalette(pal);
        setAutoFillBackground(true);
    }

    LightWidget *redLight() const
        { return m_red; }
    LightWidget *yellowLight() const
        { return m_yellow; }
    LightWidget *greenLight() const
        { return m_green; }

private:
    LightWidget *m_red;
    LightWidget *m_yellow;
    LightWidget *m_green;
};

class StatusLight : public QWidget
{
    Q_OBJECT
public:
    StatusLight(QWidget *parent, QString name, int x, int y);
    void Show(void);
    void ShowMessage(void);
    QString ProcessCommand(QString cmd);
    QWidget *p;
    QString Title;
    QString Status;
    QString Mode;
    int     X,Y;

    QLabel *label;
private:
    QLabel *TL;
    QLabel *Message;
    TrafficLightWidget *widget;
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

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
