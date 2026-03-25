// =============================================================================
// mips.h
//
// MIPS — top-level Qt main window for the MIPS instrument control application.
// Owns all subsystem objects (Comms, DCbias, RFdriver, ARB, FAIMS, etc.) and
// the main tab widget. Provides a scripting API via Q_INVOKABLE methods.
//
// Depends on:  comms.h, controlpanel.h, and all subsystem headers
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 1+2+3 refactoring
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef MIPS_H
#define MIPS_H

#include <QMainWindow>
#include <QtCore/QtGlobal>
#include <QLineEdit>
#include <QTimer>
#include <QProcess>
#include <QStatusBar>

namespace Ui {
class MIPS;
}

/*! \brief Blocking one-second delay that keeps the event loop alive (defined in connection.cpp). */
void delay();

class Console;
class SettingsDialog;
class pseDialog;
class psgPoint;
class Comms;
class Twave;
class DCbias;
class DIO;
class RFdriver;
class PSG;
class Program;
class Help;
class ARB;
class FAIMS;
class Filament;
class ARBwaveformEdit;
class ADC;
class ControlPanel;
class ScriptingConsole;
class Properties;
class Plot;

/*! \brief Event filter that removes the highlighted combo-box item when Shift+Backspace is pressed. */
class DeleteHighlightedItemWhenShiftDelPressedEventFilter : public QObject
{
     Q_OBJECT
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

/*! \class MIPS
 * \brief Top-level main window for the MIPS instrument control application.
 *
 * Owns all subsystem objects and the main tab widget. Manages serial/TCP
 * connection lifecycle, file transfer, control-panel loading, and exposes
 * a scripting API via Q_INVOKABLE and public-slot methods callable from
 * QJSEngine scripts in ControlPanel.
 */
class MIPS : public QMainWindow
{
    Q_OBJECT

public:
    explicit MIPS(QWidget *parent = nullptr, QString CPfilename = "");
    ~MIPS();
    virtual void closeEvent(QCloseEvent *event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void mousePressEvent(QMouseEvent * event);
    Ui::MIPS *ui;
    QString SelectedTab;
    Q_INVOKABLE void RemoveTab(QString TabName);
    Q_INVOKABLE void AddTab(QString TabName);
    Q_INVOKABLE void FindMIPSandConnect(void);
    Q_INVOKABLE void FindAllMIPSsystems(void);
    Comms* FindCommPort(QString name, QList<Comms*> Systems);

public slots:
    void setWidgets(QWidget*, QWidget*);
    void MIPSconnect(void);
    void MIPSsearch(void);
    void MIPSsetup(void);
    void MIPSdisconnect(void);
    void tabSelected();
    void writeData(const QByteArray &data);
    void readData2Console(void);
    void pollLoop(void);
    void loadSettings(void);
    void loadPlot(void);
    void saveSettings(void);
    void DisplayAboutMessage(void);
    void clearConsole(void);
    void MIPScommands(void);
    void GeneralHelp(void);
    void GetRepeatMessage(void);
    void UpdateSystem(void);
    void GetFileFromMIPS(void);
    void PutFiletoMIPS(void);
    void ReadEEPROM(void);
    void WriteEEPROM(void);
    void ReadARBFLASH(void);
    void WriteARBFLASH(void);
    void ARBupload(void);
    void SelectCP(QString fileName = "");
    void CloseControlPanel(QString);
    void slotScripting(void);
    void DisplayProperties(void);
    void slotLogStatusBarMessage(QString);
    void slotPlotDialogClosed(Plot *thisPlot);

    // Scripting API — callable from QJSEngine scripts in ControlPanel
    bool    SendCommand(QString message);
    QString SendMess(QString message);
    bool    SendCommand(QString MIPSname, QString message);
    QString SendMess(QString MIPSname, QString message);
    void    statusMessage(QString message);
    void    popupMessage(QString message);
    bool    popupYesNoMessage(QString message);
    QString popupUserInput(QString title, QString message);

private:
    Properties      *properties;
    Comms           *comms;
    Twave           *twave;
    DCbias          *dcbias;
    DIO             *dio;
    RFdriver        *rfdriver;
    PSG             *SeqGen;
    Program         *pgm;
    Console         *console;
    SettingsDialog  *settings;
    Help            *help;
    ARB             *arb;
    FAIMS           *faims;
    Filament        *filament;
    ControlPanel    *cp;
    bool             cp_deleteRequest;
    ADC             *adc;
    QTimer          *pollTimer;
    QString          appPath;
    QString          RepeatMessage;
    QList<Comms*>    Systems;
    ScriptingConsole *scriptconsole;
    QString          NextCP;
    QList<Plot *>    plots;
};

#endif // MIPS_H
