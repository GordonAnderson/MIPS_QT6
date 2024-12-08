#ifndef SCRIPTINGCONSOLE_H
#define SCRIPTINGCONSOLE_H

#include <QDialog>
#include <QJSEngine>
#include <QStatusBar>
#include <QThread>
#include "properties.h"

class JSengine : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(bool UpdateHalted WRITE UpdateHaltedSigNB NOTIFY UpdateHaltedSigNB)
public:
    explicit JSengine(QWidget *parent = 0);
    Q_INVOKABLE QString Save(QString Filename) {QString res= emit SaveSig(Filename); return res;};
    Q_INVOKABLE QString Load(QString Filename) {QString res= emit LoadSig(Filename); return res;};
    Q_INVOKABLE QString GetLine(QString MIPSname) {QString res= emit GetLineSig(MIPSname); return res;};
    Q_INVOKABLE bool SendString(QString MIPSname, QString message) {bool res= emit SendStringSig(MIPSname, message); return res;};
    Q_INVOKABLE bool SendCommand(QString MIPSname, QString message) {bool res= emit SendCommandSig(MIPSname, message); return res;};
    Q_INVOKABLE QString SendMess(QString MIPSname, QString message) {QString res= emit SendMessSig(MIPSname,message); return res;};
    Q_INVOKABLE void SystemEnable(void) {emit SystemEnableSig();};
    Q_INVOKABLE void SystemShutdown(void) {emit SystemShutdownSig();};
    Q_INVOKABLE bool isShutDown(void) {bool res= emit isShutDownSig(); return res;};
    Q_INVOKABLE void Acquire(QString filePath) {emit AcquireSig(filePath);};
    Q_INVOKABLE bool isAcquiring(void) {bool res= emit isAcquiringSig(); return res;};
    Q_INVOKABLE void DismissAcquire(void) {emit DismissAcquireSig();};
    Q_INVOKABLE void msDelay(int ms) {emit msDelaySig(ms);};
    Q_INVOKABLE void statusMessage(QString message) {emit statusMessageSig(message);};
    Q_INVOKABLE void popupMessage(QString message) {emit popupMessageSig(message);};
    Q_INVOKABLE bool popupYesNoMessage(QString message) {bool res= emit popupYesNoMessageSig(message); return res;};
    Q_INVOKABLE QString popupUserInput(QString title, QString message) {QString res= emit popupUserInputSig(title, message); return res;};
    Q_INVOKABLE bool UpdateHalted(bool stop) {bool res= emit UpdateHaltedSig(stop); return res;};
    Q_INVOKABLE void WaitForUpdate(void) {emit WaitForUpdateSig();};
    Q_INVOKABLE QString SelectFile(QString fType, QString Title, QString Ext) {QString res= emit SelectFileSig(fType, Title, Ext); return res;};
    Q_INVOKABLE int ReadCSVfile(QString fileName, QString delimiter) {int res= emit ReadCSVfileSig(fileName, delimiter); return res;};
    Q_INVOKABLE QString ReadCSVentry(int line, int entry) {QString res= emit ReadCSVentrySig(line,entry); return res;};
    Q_INVOKABLE QString Command(QString cmd) {QString res= emit CommandSig(cmd); return res;};
    Q_INVOKABLE void CreatePlot(QString Title, QString Yaxis, QString Xaxis, int NumPlots) {emit CreatePlotSig(Title,Yaxis,Xaxis,NumPlots);};
    Q_INVOKABLE void PlotCommand(QString cmd) {emit PlotCommandSig(cmd);};
    Q_INVOKABLE bool isComms(void) {bool res= emit isCommsSig(); return res;};
    Q_INVOKABLE QString MakePathNameUnique(QString path) {QString res= emit MakePathNameUniqueSig(path); return res;};
    Q_INVOKABLE QString GenerateUniqueFileName(void) {QString res= emit GenerateUniqueFileNameSig(); return res;};
    Q_INVOKABLE int elapsedTime(bool init) {int res= emit elapsedTimeSig(init); return res;};
    Q_INVOKABLE QString tcpSocket(QString cmd, QString arg1, QString arg2) {QString res= emit tcpSocketSig(cmd, arg1, arg2); return res;};
    QJSEngine engine;
    QString   script;
    QJSValue  result;
    bool      isRunning = false;

private:
    QWidget *p;

public slots:
    QJSValue runEngine(void);

signals:
    void resultReady(QJSValue result);
    QString SaveSig(QString Filename);
    QString LoadSig(QString Filename);
    QString GetLineSig(QString MIPSname);
    bool SendStringSig(QString MIPSname, QString message);
    bool SendCommandSig(QString MIPSname, QString message);
    QString SendMessSig(QString MIPSname, QString message);
    void SystemEnableSig(void);
    void SystemShutdownSig(void);
    bool isShutDownSig(void);
    void AcquireSig(QString filePath);
    bool isAcquiringSig(void);
    void DismissAcquireSig(void);
    void msDelaySig(int ms);
    void statusMessageSig(QString message);
    void popupMessageSig(QString message);
    bool popupYesNoMessageSig(QString message);
    QString popupUserInputSig(QString title, QString message);
    bool UpdateHaltedSig(bool stop);
    bool UpdateHaltedSigNB(bool stop);
    void WaitForUpdateSig(void);
    QString SelectFileSig(QString fType, QString Title, QString Ext);
    int ReadCSVfileSig(QString fileName, QString delimiter);
    QString ReadCSVentrySig(int line, int entry);
    QString CommandSig(QString cmd);
    void CreatePlotSig(QString Title, QString Yaxis, QString Xaxis, int NumPlots);
    void PlotCommandSig(QString cmd);
    bool isCommsSig(void);
    QString MakePathNameUniqueSig(QString path);
    QString GenerateUniqueFileNameSig(void);
    int elapsedTimeSig(bool init);
    QString tcpSocketSig(QString cmd, QString arg1, QString arg2);
};

namespace Ui {
class ScriptingConsole;
}

class ScriptingConsole : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptingConsole(QWidget *parent = 0, Properties *prop = NULL);
    void UpdateStatus(void);
    void RunScript(void);
    ~ScriptingConsole();
    Properties  *properties;

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::ScriptingConsole *ui;
    QWidget *p;
    QThread  *thread;
    JSengine *engine;

private slots:
    void on_pbEvaluate_clicked();
    void on_pbSave_clicked();
    void on_pbLoad_clicked();
    void on_pbAbort_clicked();
    void on_pbHelp_clicked();
    void scriptFinished(QJSValue result);
    void engineDone();
};

// Creates a script button on the control panel. When pressed the
// script is loaded and executed. If a script is alreay executing
// then the user is asked if he would like to abort.
class ScriptButton : public QWidget
{
    Q_OBJECT

public:
    explicit ScriptButton(QWidget *parent, QString name, QString ScriptFile, int x, int y, Properties *prop, QStatusBar *statusbar);
    ~ScriptButton();
    void             Show(void);
    void             Update(void);
    QString          ProcessCommand(QString cmd);
    QString          Title;
    QString          FileName;
    int              X,Y;
    QWidget          *p;
    QStatusBar       *sb;
    Properties       *properties;
    QString          ScriptText;
    bool             CallOnUpdate;
    bool             CallOnStart;
    bool             ScriptTextFixed;
    bool             reportResults;
    QPushButton      *pbButton;
private:
    QThread          *thread;
    QJSValue         mips;
    JSengine         *engine;
    void ButtonPressed(bool AlwaysLoad);
private slots:
    void pbButtonPressed(void);
    void scriptFinished(QJSValue result);
    void engineDone();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // SCRIPTINGCONSOLE_H
