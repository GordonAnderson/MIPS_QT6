// =============================================================================
// mips.cpp
//
// MIPS — top-level Qt main window for the MIPS instrument control application.
// Owns and wires up all subsystem objects (Comms, DCbias, RFdriver, ARB,
// FAIMS, Filament, Twave, PSG, ADC, DIO) and manages the main tab widget,
// serial/TCP connection lifecycle, file transfer, and control-panel loading.
//
// Depends on:  mips.h and all subsystem headers
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 1+2+3 refactoring
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "mips.h"
#include "ui_mips.h"
#include "console.h"
#include "settingsdialog.h"
#include "ringbuffer.h"
#include "comms.h"
#include "twave.h"
#include "dcbias.h"
#include "dio.h"
#include "rfdriver.h"
#include "psg.h"
#include "program.h"
#include "help.h"
#include "arb.h"
#include "faims.h"
#include "filament.h"
#include "adc.h"
#include "controlpanel.h"
#include "scriptingconsole.h"
#include "properties.h"

#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QThread>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QCursor>
#include <QFileDialog>
#include <QDir>
#include <QProcess>
#include <QFileInfo>
#include <QtNetwork/QTcpSocket>
#include <QInputDialog>
#include <QElapsedTimer>

QString Version = "MIPS, Version 2.23 March 20,2026";

/*! \brief MIPS::MIPS
 * Constructor. Creates and wires all subsystem objects, connects all UI signals,
 * and starts the poll timer.
 */
MIPS::MIPS(QWidget *parent, QString CPfilename) :
    QMainWindow(parent),
    ui(new Ui::MIPS)
{
    ui->setupUi(this);
    ui->tabMIPS->setElideMode(Qt::ElideNone);
    ui->tabMIPS->setUsesScrollButtons(true);
    ui->tabMIPS->tabBar()->setStyleSheet("QTabBar::tab:selected {color: white; background-color: rgb(90,90,255);}");

    // Make the dialog fixed size.
    this->setFixedSize(this->size());

    ui->comboSystems->setVisible(false);
    ui->lblSystems->setVisible(false);

    MIPS::setWindowTitle(Version);

    properties = new Properties;
    plots.clear();
    scriptconsole = nullptr;
    cp = nullptr;
    cp_deleteRequest = false;
    NextCP.clear();
    appPath = QApplication::applicationDirPath();
    pollTimer = new QTimer;
    settings = new SettingsDialog;
    comms  = new Comms(settings,"",ui->statusBar);
    console = new Console(ui->Terminal,ui,comms);
    console->setEnabled(false);
    twave  = new Twave(ui,comms);
    dcbias = new DCbias(ui,comms);
    dio = new DIO(ui,comms);
    rfdriver = new RFdriver(ui,comms);
    SeqGen = new PSG(ui,comms);
    pgm = new Program(ui, comms, console);
    help = new Help();
    arb = new ARB(ui, comms);
    faims = new FAIMS(ui, comms);
    filament = new Filament(ui, comms);
    adc = new ADC(this,ui, comms);

    RepeatMessage = "";
    ui->actionClear->setEnabled(true);
    ui->actionOpen->setEnabled(true);
    ui->actionLoad_Plot->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionHelp->setEnabled(true);
    ui->actionMIPS_commands->setEnabled(true);
    ui->actionProgram_MIPS->setEnabled(true);
    ui->actionSave_current_MIPS_firmware->setEnabled(true);
    ui->actionSet_bootloader_boot_flag->setEnabled(true);
    ui->actionMessage_repeat->setEnabled(true);
    ui->actionGet_file_from_MIPS->setEnabled(true);
    ui->actionSend_file_to_MIPS->setEnabled(true);
    ui->actionRead_EEPROM->setEnabled(true);
    ui->actionWrite_EEPROM->setEnabled(true);
    ui->actionRead_ARB_FLASH->setEnabled(true);
    ui->actionWrite_ARB_FLASH->setEnabled(true);
    ui->actionARB_upload->setEnabled(true);
    ui->actionSelect->setEnabled(true);
    ui->menuTerminal->setEnabled(false);
    ui->actionScripting->setEnabled(true);

    connect(ui->actionProperties, &QAction::triggered, this, &MIPS::DisplayProperties);
    connect(ui->actionOpen, &QAction::triggered, this, &MIPS::loadSettings);
    connect(ui->actionLoad_Plot, &QAction::triggered, this, &MIPS::loadPlot);
    connect(ui->actionSave, &QAction::triggered, this, &MIPS::saveSettings);
    connect(ui->pbConfigure, &QPushButton::pressed, settings, &SettingsDialog::show);
    connect(ui->actionMIPS_commands, &QAction::triggered, this, &MIPS::MIPScommands);
    connect(ui->actionHelp, &QAction::triggered, this, &MIPS::GeneralHelp);
    connect(ui->tabMIPS, &QTabWidget::currentChanged, this, [this](int){ tabSelected(); });
    connect(ui->pbConnect, &QPushButton::pressed, this, &MIPS::MIPSconnect);
    connect(ui->pbSearchandConnect, &QPushButton::pressed, this, &MIPS::MIPSsearch);
    connect(ui->pbDisconnect, &QPushButton::pressed, this, &MIPS::MIPSdisconnect);
    connect(pollTimer, &QTimer::timeout, this, &MIPS::pollLoop);
    connect(ui->actionAbout, &QAction::triggered, this, &MIPS::DisplayAboutMessage);
    connect(ui->actionClear, &QAction::triggered, this, &MIPS::clearConsole);
    connect(ui->actionMessage_repeat, &QAction::triggered, this, &MIPS::GetRepeatMessage);
    connect(ui->actionGet_file_from_MIPS, &QAction::triggered, this, &MIPS::GetFileFromMIPS);
    connect(ui->actionSend_file_to_MIPS, &QAction::triggered, this, &MIPS::PutFiletoMIPS);
    connect(ui->actionRead_EEPROM, &QAction::triggered, this, &MIPS::ReadEEPROM);
    connect(ui->actionWrite_EEPROM, &QAction::triggered, this, &MIPS::WriteEEPROM);
    connect(ui->actionRead_ARB_FLASH, &QAction::triggered, this, &MIPS::ReadARBFLASH);
    connect(ui->actionWrite_ARB_FLASH, &QAction::triggered, this, &MIPS::WriteARBFLASH);
    connect(ui->actionARB_upload, &QAction::triggered, this, &MIPS::ARBupload);
    connect(ui->actionSelect, &QAction::triggered, this, [this](){ SelectCP(); });
    connect(ui->actionScripting, &QAction::triggered, this, &MIPS::slotScripting);
    connect(ui->statusBar, &QStatusBar::messageChanged, this, &MIPS::slotLogStatusBarMessage);

    ui->comboMIPSnetNames->installEventFilter(new DeleteHighlightedItemWhenShiftDelPressedEventFilter);
    // Sets the polling loop interval and starts the timer
    if(properties != nullptr) pollTimer->start(1000 * properties->UpdateSecs);
    else pollTimer->start(1000);

    if(properties != nullptr)
    {
        for(int i=0;i<properties->MIPS_TCPIP.count();i++) ui->comboMIPSnetNames->addItem(properties->MIPS_TCPIP[i]);
        if(properties != nullptr) properties->Log("MIPS loaded: " + Version);
    }
    #if defined(Q_OS_MAC)
    if(properties != nullptr)
    {
        properties->Log("Command line: " + CPfilename);
        QFont font = QApplication::font();
        font.setPointSize(properties->sysFontSize.toInt());
        ui->lblMIPSconnectionNotes->setFont(font);
        if(!CPfilename.isEmpty())
        {
            properties->LoadControlPanel = CPfilename;
            properties->AutoConnect = true;
            properties->AutoRestore = true;
        }
    }
    #else
    QFont font("Tahoma", properties->sysFontSize.toInt());
    QApplication::setFont(font);
    #endif
}

/*! \brief MIPS::~MIPS
 * Destructor. Logs the closing event and schedules deferred deletion.
 */
MIPS::~MIPS()
{
    if(properties != nullptr) properties->Log("MIPS closing: " + Version);
    for(int i=0;i<plots.count();i++) emit plots[i]->DialogClosed(plots[i]);
    this->deleteLater();
}

/*! \brief MIPS::closeEvent
 * Handles window close: deletes the active control panel, settings, help objects, and plots.
 */
void MIPS::closeEvent(QCloseEvent *)
{
    if(cp != nullptr) delete cp;
    delete settings;
    delete help;
    for(int i=0;i<plots.count();i++) emit plots[i]->DialogClosed(plots[i]);
    delete ui;
}

/*! \brief DeleteHighlightedItemWhenShiftDelPressedEventFilter::eventFilter
 * Removes the highlighted combo-box entry when Shift+Backspace is pressed.
 */
bool DeleteHighlightedItemWhenShiftDelPressedEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if ((keyEvent->key() == Qt::Key::Key_Backspace) && (keyEvent->modifiers() == Qt::ShiftModifier))
        {
            auto combobox = dynamic_cast<QComboBox *>(obj);
            if (combobox){
                combobox->removeItem(combobox->currentIndex());
                return true;
            }
        }
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}

/*! \brief MIPS::MIPScommands
 * Opens the MIPS command reference in the Help dialog.
 */
void MIPS::MIPScommands(void)
{
    help->SetTitle("MIPS Commands");
    help->LoadHelpText(":/MIPScommands.txt");
    help->show();
}

// Removes a tab by its name
/*! \brief MIPS::RemoveTab
 * Removes the named tab from the main tab widget, if present.
 */
void MIPS::RemoveTab(QString TabName)
{
   for(int i = 0; i < ui->tabMIPS->count(); i++)
   {
       if( ui->tabMIPS->tabText(i) == TabName)
       {
           ui->tabMIPS->removeTab(i);
           return;
       }
   }
}

/*! \brief MIPS::AddTab
 * Inserts the named tab into the main tab widget at position 3, if not already present.
 */
void MIPS::AddTab(QString TabName)
{
    // If tab is present then do nothing and exit
    for(int i = 0; i < ui->tabMIPS->count(); i++)
    {
        if( ui->tabMIPS->tabText(i) == TabName)
        {
            return;
        }
    }
    if(TabName == "DCbias") ui->tabMIPS->insertTab(3,ui->DCbias, "DCbias");
    if(TabName == "RFdriver") ui->tabMIPS->insertTab(3,ui->RFdriver, "RFdriver");
    if(TabName == "Twave") ui->tabMIPS->insertTab(3,ui->Twave, "Twave");
    if(TabName == "ARB") ui->tabMIPS->insertTab(3,ui->ARB, "ARB");
    if(TabName == "FAIMS") ui->tabMIPS->insertTab(3,ui->FAIMS, "FAIMS");
    if(TabName == "Filament") ui->tabMIPS->insertTab(3,ui->Filament, "Filament");
}

/*! \brief MIPS::GeneralHelp
 * Shows context-sensitive help for the currently active tab.
 */
void MIPS::GeneralHelp(void)
{
    help->SetTitle("MIPS Help");
    help->LoadHelpText(":/MIPShelp.txt");
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "System")
    {
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal")
    {
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Digital IO")
    {
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "DCbias")
    {
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "RFdriver")
    {
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "FAIMS")
    {
        help->SetTitle("FAIMS help");
        help->LoadHelpText(":/FAIMShelp.txt");
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Pulse Sequence Generation")
    {
        help->SetTitle("Pulse Sequence Generation Help");
        help->LoadHelpText(":/PSGhelp.txt");
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "ARB")
    {
        help->SetTitle("ARB Help");
        help->LoadHelpText(":/ARBhelp.txt");
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Twave")
    {
        help->SetTitle("Twave Help");
        help->LoadHelpText(":/TWAVEhelp.txt");
    }
    help->show();
    return;
}

/*! \brief MIPS::DisplayAboutMessage
 * Shows the application About message box.
 */
void MIPS::DisplayAboutMessage(void)
{
    QMessageBox::information(
        this,
        tr("Application Named"),
        tr("MIPS interface application, written by Gordon Anderson. This application allows communications with the MIPS system supporting monitoring and control as well as pulse sequence generation.") );
}

/*! \brief MIPS::DisplayProperties
 * Opens the Properties dialog.
 */
void MIPS::DisplayProperties(void)
{
    properties->exec();

}

/*! \brief MIPS::slotPlotDialogClosed
 * Slot: deletes the closed Plot object and removes it from the plots list.
 */
void MIPS::slotPlotDialogClosed(Plot *thisPlot)
{
    // Find this plot object in the list and then
    // delete the objet and remove from the list
    for(int i=0;i<plots.count();i++)
    {
        if(plots[i] == thisPlot)
        {
            delete plots[i];
            plots.removeAt(i);
        }
    }
}

/*! \brief MIPS::loadPlot
 * Opens a .plot file and creates a new Plot dialog for it.
 */
void MIPS::loadPlot(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load plot file"),"",tr("Plot (*.plot);;All files (*.*)"));

    plots.append(new Plot(0, "Title", "Yaxis", "Xaxis", 1));
    plots.last()->show();
    plots.last()->raise();
    connect(plots.last(), &Plot::DialogClosed, this, &MIPS::slotPlotDialogClosed);
    plots.last()->PlotCommand("Load," + fileName);
}

/*! \brief MIPS::loadSettings
 * Loads settings from file for the currently active tab's subsystem.
 */
void MIPS::loadSettings(void)
{
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "System")
    {
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal")
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Load Data from File"),"",tr("Settings (*.settings);;All files (*.*)"));
        console->Load(fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Digital IO")
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Load Settings from File"),"",tr("Settings (*.settings);;All files (*.*)"));
        dio->Load(fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "DCbias")
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Load Settings from File"),"",tr("Settings (*.settings);;All files (*.*)"));
        dcbias->Load(fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "RFdriver")
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Load Settings from File"),"",tr("Settings (*.settings);;All files (*.*)"));
        rfdriver->Load(fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "FAIMS")
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Load Settings from File"),"",tr("Settings (*.settings);;All files (*.*)"));
        faims->Load(fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Pulse Sequence Generation")
    {
        SeqGen->Load();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Twave")
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Load Settings from File"),"",tr("Settings (*.settings);;All files (*.*)"));
        twave->Load(fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "ARB")
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Load Settings from File"),"",tr("Settings (*.settings);;All files (*.*)"));
        arb->Load(fileName);
    }
    return;
}

/*! \brief MIPS::saveSettings
 * Saves settings to file for the currently active tab's subsystem, or the open control panel.
 */
void MIPS::saveSettings(void)
{
    if(cp != nullptr)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save to Settings File"),"",tr("Settings (*.settings);;All files (*.*)"));
        cp->Save(fileName);
        return;
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "System")
    {
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal")
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save to Data File"),"",tr("Settings (*.settings);;All files (*.*)"));
        console->Save(fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Digital IO")
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save to Settings File"),"",tr("Settings (*.settings);;All files (*.*)"));
        dio->Save(fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "DCbias")
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save to Settings File"),"",tr("Settings (*.settings);;All files (*.*)"));
        dcbias->Save(fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "RFdriver")
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save to Settings File"),"",tr("Settings (*.settings);;All files (*.*)"));
        rfdriver->Save(fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "FAIMS")
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save to Settings File"),"",tr("Settings (*.settings);;All files (*.*)"));
        faims->Save(fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Pulse Sequence Generation")
    {
        SeqGen->Save();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Twave")
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save to Settings File"),"",tr("Settings (*.settings);;All files (*.*)"));
        twave->Save(fileName);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "ARB")
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save to Settings File"),"",tr("Settings (*.settings);;All files (*.*)"));
        arb->Save(fileName);
    }
    return;
}

/*! \brief MIPS::pollLoop
 * Timer slot: on first call handles auto-connect and auto-load; thereafter drives
 * per-tab polling and control-panel updates.
 */
void MIPS::pollLoop(void)
{
    static  bool firstCall = true;

    if(firstCall)
    {
        if(properties->AutoConnect) MIPSsearch();
        if(Systems.count() >= properties->MinMIPS)
        {
           if(properties->LoadControlPanel != "") SelectCP(properties->LoadControlPanel);
        }
    }
    firstCall = false;
    if(scriptconsole != nullptr) scriptconsole->UpdateStatus();
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Pulse Sequence Generation")
    {

    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal")
    {
        if(RepeatMessage != "") comms->SendString(RepeatMessage.toLocal8Bit());
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "DCbias")
    {
         if(ui->chkDCBautoUpdate->isChecked())  dcbias->Update();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "RFdriver")
    {
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Filament")
    {
        if(comms->isConnected()) filament->Update();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "ARB")
    {
        if(comms->isConnected()) if(ui->chkARBautoUpdate->isChecked()) arb->Update();
    }
    if(comms->isConnected()) faims->PollLoop();

    if(cp_deleteRequest)
    {
        delete cp;
        cp = nullptr;
        cp_deleteRequest = false;
        if(!NextCP.isEmpty()) SelectCP(NextCP);
    }
    if(cp != nullptr) cp->Update();
 }

/*! \brief MIPS::mousePressEvent
 * Passes mouse press events to the base class.
 */
void MIPS::mousePressEvent(QMouseEvent * event)
{
    QMainWindow::mousePressEvent(event);
}

/*! \brief MIPS::resizeEvent
 * Resizes the tab widget and console to match the new window dimensions.
 */
void MIPS::resizeEvent(QResizeEvent* event)
{
   QFontMetrics fm(console->font());
   int charHeight = fm.height();
   ui->tabMIPS->setFixedWidth(MIPS::width());
   #if defined(Q_OS_MAC)
     int i = (int)(MIPS::height()-ui->statusBar->height())/charHeight * charHeight;
     ui->tabMIPS->setFixedHeight(i);
   #else
     // Not sure why I need this 3x for a windows system??
     ui->tabMIPS->setFixedHeight(MIPS::height()-(ui->statusBar->height()*3));
   #endif

   console->resize(ui->Terminal);
   QMainWindow::resizeEvent(event);
}

/*! \brief MIPS::tabSelected
 * Slot: disconnects the previous tab's data signals, reconnects for the new tab,
 * and triggers the relevant subsystem Update().
 */
void MIPS::tabSelected()
{
    static QString LastTab = "System";
    if(properties != nullptr) properties->Log("Tab selected, " + ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()));
    disconnect(comms, &Comms::DataReady, nullptr, nullptr);
    disconnect(console, &Console::getData, nullptr, nullptr);
    ui->menuTerminal->setEnabled(false);
    if(LastTab == "Pulse Sequence Generation")
    {
        // Abort when exiting this tab
        comms->SendString("TBLABRT\n");
        delay();
        comms->rb.clear();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "System")
    {
        LastTab = "System";
        settings->fillPortsInfo();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "ADC")
    {
        LastTab = "ADC";
        adc->Update(true);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Terminal")
    {
        ui->menuTerminal->setEnabled(true);
        LastTab = "Terminal";
        connect(comms, &Comms::DataReady, this, &MIPS::readData2Console);
        connect(console, &Console::getData, this, &MIPS::writeData);
        console->resize(ui->Terminal);
        console->setEnabled(true);
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Digital IO")
    {
        ui->chkRemoteNav->setChecked(false);
        dio->comms = comms;
        LastTab = "Digital IO";
        dio->Update();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "DCbias")
    {
        dcbias->comms = comms;
        LastTab = "DCbias";
        dcbias->Update();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "RFdriver")
    {
        rfdriver->comms = comms;
        LastTab = "RFdriver";
        rfdriver->Update();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Pulse Sequence Generation")
    {
        SeqGen->comms = comms;
        LastTab = "Pulse Sequence Generation";
        SeqGen->Update();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Twave")
    {
        twave->comms = comms;
        LastTab = "Twave";
        twave->Update();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "ARB")
    {
        arb->comms = comms;
        LastTab = "ARB";
        arb->Update();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "FAIMS")
    {
        faims->comms = comms;
        LastTab = "FAIMS";
        faims->Update();
    }
    if( ui->tabMIPS->tabText(ui->tabMIPS->currentIndex()) == "Filament")
    {
        filament->comms = comms;
        LastTab = "Filament";
        filament->Update();
    }
    SelectedTab = LastTab;
}

/*! \brief MIPS::writeData
 * Slot: forwards raw bytes from the terminal console to the active Comms port.
 */
void MIPS::writeData(const QByteArray &data)
{
    comms->writeData(data);
}

/*! \brief MIPS::readData2Console
 * Slot: reads all available bytes from Comms and appends them to the terminal console.
 */
void MIPS::readData2Console(void)
{
    QByteArray data;

    data = comms->readall();
    console->putData(data);
}

/*! \brief MIPS::setWidgets
 * Placeholder — reserved for future widget layout control.
 */
void MIPS::setWidgets(QWidget* , QWidget*)
{
}

/*! \brief MIPS::clearConsole
 * Clears the terminal console.
 */
void MIPS::clearConsole(void)
{
    console->clear();
}

/*! \brief MIPS::GetRepeatMessage
 * Prompts the user for a command string to send every poll interval.
 */
void MIPS::GetRepeatMessage(void)
{
    bool ok;

    QString text = QInputDialog::getText(0, "MIPS", "Enter Command to send every sec:", QLineEdit::Normal,"", &ok );
    if ( ok && !text.isEmpty() )
    {
        RepeatMessage =  text + "\n";
    } else
    {
        RepeatMessage = "";
    }
}

/*! \brief MIPS::CloseControlPanel
 * Slot called when the control panel dialog closes. If newPC is non-empty the
 * named control panel is queued for opening; otherwise the main window is re-enabled.
 */
void MIPS::CloseControlPanel(QString newPC)
{
    NextCP = newPC;
    cp_deleteRequest = true;
    if(!newPC.isEmpty()) return;
    this->setWindowState(Qt::WindowActive);
    ui->tabMIPS->setDisabled(false);
}

/*! \brief MIPS::SelectCP
 * Opens the named control panel dialog. Exits immediately if a panel is already open.
 * Minimises the main window and disables the tab widget while the panel is active.
 */
void MIPS::SelectCP(QString fileName)
{
    if(cp != nullptr) return;
    ui->tabMIPS->setCurrentIndex(0);
    ControlPanel *c = new ControlPanel(0,fileName,Systems,properties);
    if(!c->LoadedConfig)
    {
        delete c;
        return;
    }
    connect(c, &ControlPanel::DialogClosed, this, &MIPS::CloseControlPanel);
    c->Update();
    this->setWindowState(Qt::WindowMinimized);
    cp = c;
    cp->show();
    ui->tabMIPS->setDisabled(true);
    cp->raise();
}

/*! \brief MIPS::slotScripting
 * Opens the scripting console, creating it on first use.
 */
void MIPS::slotScripting(void)
{
    if(!scriptconsole) scriptconsole = new ScriptingConsole(this,properties);
    scriptconsole->show();
}

// Returns a pointer to the comm port for the MIPS system defined my its name.
// Returns null if the MIPS system can't be found.
/*! \brief MIPS::FindCommPort
 * Returns the Comms pointer for the named MIPS system, or nullptr if not found.
 */
Comms* MIPS::FindCommPort(QString name, QList<Comms*> Systems)
{
   for(int i = 0; i < Systems.length(); i++)
   {
       if(Systems.at(i)->MIPSname == name) return(Systems.at(i));
   }
   return nullptr;
}

/*! \brief MIPS::SendCommand
 * Sends a command string to the primary Comms port.
 */
bool MIPS::SendCommand(QString message)
{
   return comms->SendCommand(message);
}

/*! \brief MIPS::SendMess
 * Sends a message and returns the response from the primary Comms port.
 */
QString MIPS::SendMess(QString message)
{
   return comms->SendMess(message);
}

/*! \brief MIPS::SendCommand (two-arg)
 * Sends a command to the named MIPS system, or returns false if not found.
 */
bool MIPS::SendCommand(QString MIPSname, QString message)
{
    Comms *cp =  FindCommPort(MIPSname,Systems);
    if(cp == nullptr) return false;
    return cp->SendCommand(message);
}

/*! \brief MIPS::SendMess (two-arg)
 * Sends a message to the named MIPS system and returns the response,
 * or an error string if not found.
 */
QString MIPS::SendMess(QString MIPSname, QString message)
{
    Comms *cp =  FindCommPort(MIPSname,Systems);
    if(cp == nullptr) return "MIPS not found!";
    return cp->SendMess(message);
}

/*! \brief MIPS::statusMessage
 * Displays a message on the status bar.
 */
void MIPS::statusMessage(QString message)
{
    ui->statusBar->showMessage(message);
}

/*! \brief MIPS::popupMessage
 * Shows a modal message box with an OK button.
 */
void MIPS::popupMessage(QString message)
{
    QMessageBox msgBox;

    msgBox.setText(message);
    msgBox.setInformativeText("");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

/*! \brief MIPS::popupYesNoMessage
 * Shows a Yes/No modal message box; returns true if the user chooses Yes.
 */
bool MIPS::popupYesNoMessage(QString message)
{
    QMessageBox msgBox;

    msgBox.setText(message);
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if(ret == QMessageBox::No) return false;
    return true;
}

/*! \brief MIPS::popupUserInput
 * Shows an input dialog and returns the entered text, or an empty string if cancelled.
 */
QString MIPS::popupUserInput(QString title, QString message)
{
    bool ok;

    QString text = QInputDialog::getText(this, title, message, QLineEdit::Normal,QString(), &ok);
    if(!ok) text="";
    return text;
}

/*! \brief MIPS::slotLogStatusBarMessage
 * Logs non-empty status bar messages to the Properties log.
 */
void MIPS::slotLogStatusBarMessage(QString statusMess)
{
    if(statusMess == "") return;
    if(statusMess.isEmpty()) return;
    if(properties != nullptr) properties->Log("MIPS StatusBar: " + statusMess);
}
