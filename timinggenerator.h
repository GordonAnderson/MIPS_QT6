// =============================================================================
// timinggenerator.h
//
// TimingGenerator — pulse/table sequence generator for MIPS hardware.
// Contains four cooperating classes:
//   AcquireData    — drives an external data-acquisition command-line app
//   EventControl   — single-event editor widget (label + line edit)
//   TimingControl  — top-level widget: edit/trigger/abort + AcquireData
//   TimingGenerator — full timing-sequence dialog with event table and
//                     MUX sequence generation
//
// Depends on:  comms.h, cmdlineapp.h, dcbias.h, cdirselectiondlg.h, properties.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 1+2+3 refactoring
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef TIMINGGENERATOR_H
#define TIMINGGENERATOR_H

#include "comms.h"
#include "cmdlineapp.h"
#include "dcbias.h"
#include "cdirselectiondlg.h"
#include "properties.h"

#include <QDialog>
#include <QStatusBar>
#include <QProcess>
#include <QGroupBox>

bool DownloadTable(Comms *comms, QString Table, QString ClockSource, QString TriggerSource);
QString MakePathUnique(QString path);

/*! \struct Event
 * \brief Describes a single timing event: signal name, channel, start/width times, and on/off values.
 */
struct Event
{
    QString Name;
    QString Signal;
    QString Channel;
    QString Start;
    QString Width;
    float   StartT;
    float   WidthT;
    float   Value;
    float   ValueOff;
};

/*! \class AcquireData
 * \brief Drives an external command-line data-acquisition application.
 *
 * Manages the lifecycle of a cmdlineapp process: starting, monitoring,
 * and signalling when data files are ready or the process completes.
 */
class AcquireData : public QWidget
{
    Q_OBJECT
signals:
    void dataAcquired(QString filePath);    //!< Emitted when a data file has been fully acquired.
    void dataFileDefined(QString filePath); //!< Emitted when the output file path is determined.
public:
    AcquireData(QWidget *parent = nullptr);
    void        StartAcquire(QString path, int FrameSize, int Accumulations);
    bool        isRunning(void);
    void        Dismiss(void);
    Comms       *comms;
    QWidget     *p;
    QStatusBar  *statusBar;
    QString     filePath;
    QString     fileName;
    QString     TriggerMode;
    bool        TableDownloaded;
    bool        Acquiring;
    QString     Acquire;
    Properties  *properties;
    int         LastFrameSize;
    int         LastAccumulations;

private:
    cmdlineapp   *cla;

private slots:
    void slotAppReady(void);
    void slotAppFinished(void);
    void slotDialogClosed(void);
};

namespace Ui {
class TimingGenerator;
}

class AcquireData;

/*! \class EventControl
 * \brief Single-event editor widget: a label and a QLineEdit for the event value.
 *
 * Communicates with MIPS to get/set the event value and emits EventChanged
 * when the user commits a new value.
 */
class EventControl : public QWidget
{
    Q_OBJECT
signals:
    void EventChanged(QString Ename, QString Val); //!< Emitted when the event value is committed by the user.

public:
    EventControl(QWidget *parent, QString name, QString Ename, int x, int y);
    void    Show(void);
    void    Update(void);
    QString Report(void);
    bool    SetValues(QString strVals);
    QString ProcessCommand(QString cmd);
    QWidget   *p;
    QString    Title;
    int        X, Y;
    QString    ECname;
    Comms     *comms;
    QString    Gcommand;
    QString    Scommand;
    QLineEdit *EventValue;
private:
    QFrame    *frmEvent;
    QLabel    *label;
private slots:
    void EventChange(void);
};

/*! \class TimingGenerator
 * \brief Full timing-sequence dialog: manages events, generates MUX sequences,
 *        downloads tables to MIPS, and handles load/save of sequence files.
 */
class TimingGenerator : public QDialog
{
    Q_OBJECT

public:
    explicit TimingGenerator(QWidget *parent, QString name, QString MIPSname);
    ~TimingGenerator();
    void    AddSignal(QString Title, QString Chan);
    void    UpdateEvents(void);
    QString Report(void);
    bool    SetValues(QString strVals);
    QString ProcessCommand(QString cmd);
    QString GenerateMuxSeq(QString Seq);
    bool    isTableMode(void);
    int     ConvertToCount(QString val);
    float   CalculateTime(QString val);
    QStringList Split(QString str, QString del);
    QWidget     *p;
    QString      Title;
    QString      MIPSnm;
    Comms       *comms;
    QStatusBar  *statusBar;
    Event       *selectedEvent;
    QList<Event *>        Events;
    QList<EventControl *> EC;
    Ui::TimingGenerator  *ui;
    QPushButton  *Edit;
    QPushButton  *Trigger;
    QPushButton  *Abort;
    Properties   *properties;
    int           FrameCtAdj;
    QString       scriptName = "";

signals:
    void change(QString scriptName); //!< Emitted when a scripted change occurs.
public slots:
    void slotEventChange(void);
    void slotEventUpdated(void);
    void slotGenerate(void);
    void slotClearEvents(void);
    void slotLoad(void);
    void slotSave(void);
    void slotClearTable(void);
};

/*! \class TimingControl
 * \brief Top-level timing control widget: wraps TimingGenerator with Edit/Trigger/Abort
 *        buttons and an AcquireData instance for external data capture.
 */
class TimingControl : public QWidget
{
    Q_OBJECT
signals:
    void dataAcquired(QString filePath);    //!< Forwarded from AcquireData when data is ready.
    void dataFileDefined(QString filePath); //!< Forwarded from AcquireData when file path is set.
public:
    TimingControl(QWidget *parent, QString name, QString MIPSname, int x, int y);
    void               Show(void);
    void               AcquireData(QString path);
    class AcquireData *AD;
    QWidget           *p;
    QString            Title;
    QString            Acquire;
    int                X, Y;
    QString            MIPSnm;
    QString            filePath;
    QString            fileName;
    Comms             *comms;
    QStatusBar        *statusBar;
    QProcess           process;
    TimingGenerator   *TG;
    bool               TableDownloaded;
    bool               Acquiring;
    Properties        *properties;
    bool               Downloading;
    bool               AlwaysGenerate;
    int                FrameCtAdj;
    QGroupBox         *gbTC;
private:
    QPushButton *Edit;
    QPushButton *Trigger;
    QPushButton *Abort;

public slots:
    void pbEdit(void);
    void pbTrigger(void);
    void pbAbort(void);
    void slotDataAcquired(QString);
    void slotDataFileDefined(QString);
    void slotEventChanged(QString, QString);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // TIMINGGENERATOR_H
