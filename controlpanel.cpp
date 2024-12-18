//
// Controlpanel.cpp
//
// This file contains the classes used to create a user defined control panel. The control panels
// display MIPS controls for most features supported by MIPS. The user can define a background
// image and then place controls such as RF channels and DC bias channels at the locations the user
// defines on the background image.
//
// This feature also supports multiple MIPS systems connected to you PC at the same time. Use the
// "Find MIPS and Connect" button to find all connected MIPS system. The control panel configuration
// file uses the MIPS name to determine where the channels are located. Its important that you
// define a unique name for each MIPS system.
//
// The control panel also support the IFT (Ion Funnel Trap). This includes generation of an IFT
// trapping pulse sequence as well as calling an external application to acquire data from you
// detection system. Additionally a more generic timing generator control has been added.
//
// Feb 26, 2019, new control idea(s)
//  Implement a generic control with the following features:
//      - TYPE, defines the control type
//              - LineEdit, CheckBox, Button
//      - Set command, to set the MIPS value from LineEdit box
//      - Get command, to read the MIPS value to update the LineEdit box
//      - Readback command, if set add a readback box and this command will read the value
//  Syntax example:
//      Ccontrol,name,mips name,type,get command,set command,readback command,units,X,Y
//  This idea can be expanded to check boxes, radio buttons, etc.
//  This would allow a MIPS status page with control of system parameters.
//  The cool thing is this is not hard to implement and will support the FAIMSFB system.
//  For FAIMSFB I need to figureout how to do the plotting and heat maps. Generic plotting
//  is an idea I have thought about and need to mature this concept.
//  How can I support tabs or multiple control panel pages?
//      - Could have a control panel call another control panel basically a
//        child control panel. Somehow I would need to pass control, to the child.
//      - Could allow multiple control panels, this whould be pretty easy to do.
//      - Could have a master control panel and buttons to start other control panels,
//        how would methode save and load work in this case? (leading idea)
//      - Have a button that is tied to a control panel config file. load all the
//        control panels on start. when the button is pressed show the panel.
//
//  Add groupbox
//      GroupBox,name,width,hieght,X,Y  // Command to start group
//      GroupBoxEnd                     // Signals end of group box
//      Commands after group box is defined and before the end will be
//      placed in the group boc and there names will have the groupbox
//      name prepended delineated by a ".".
//
//  Add plotting capability
//      Could tie to scripting and popup a plot that is populated by a script. The script
//      could issue all the MIPS commands to scan and plot.
//
//  Buttons tied to scripts
//      This could allow plotting / scanning. You press the button and the script loads
//      and plays, the button name changes to abort. (leading idea)
//      Light up the button and if pressed again ask if the user would like to abort.
//      Add script commands for:
//          - Open file for save and load
//          - Write string to file
//          - Read string from file
//          - File CSV IO
//      Need to add script commands to allow generation of a plot and live feed of data.
//      Performance could be an issue here.
//      How to tie a script to a static plot on the control panel, via its name?
//
//  FAIMSFB specific notes:
//      A scan command could cause FAIMSFB to start scanning and send back the scan data
//      records. The scan data could be the scan point number followed by the specific
//      values, these would all be monitor or readback values. Plot(s) would be crreated
//      live. This could be a speed issues, maybe just send the electrometer data. The scan
//      data would not use a handshake, it would just be sent from MIPS.
//
//  Add script command for file selection dialog pop up. Return the selected
//  file name or empty string on cancel.
//      SelectFile(type,message,ext)
//
//  Add csv file support for scripting. Call a function with file name that will
//  read the full file and make a list of lists. The first first list is of each
//  line in the file. The second list is of the entries in each line. This will
//  be a list of QStringLists. Need the following functions:
//      ReadCSVfile(filename)
//      ReadCSVentry(int line, int entry)
//
//  Add Define capability, Define,Name,Value. This will define Name to Value and every
//  place Name is used in the control panel it will be replaced with Value. Case sensative.
//  Defines so into a list and can be over written. Defines are always strings. You can not
//  perform any opearations on Defined values. Create a type that is a Name,Value set of QStrings.
//  Create a function to perform the subsitution, do this at or after string split.
//
//  Add Subroutine capability, Subroutine,Name,args... This will define a subroutine
//  named Name with a variable number of arguments. To call; Call,Name,args....
//
//
// Gordon Anderson
//
#include "controlpanel.h"
#include "ui_controlpanel.h"
#include "cdirselectiondlg.h"
#include "scriptingconsole.h"
#include "Utilities.h"
//#include "ui_help.h"

#include <QPixmap>
//#include <math.h>
#include <QTextEdit>
#include <QTreeView>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTabWidget>

// Set ProcessEvents to true to reduce the blocking during data collection.
// This has to be false in QT 6.7.1 else the script system will cause problems!
//#define ProcessEvents   false // Moved to a variable 9/20/24

ControlPanel::ControlPanel(QWidget *parent, QString CPfileName, QList<Comms*> S, Properties *prop, ControlPanel *pcp) :
    QDialog(parent),
    ui(new Ui::ControlPanel)
{
    QStringList resList;
    qint64 filePos = -1;
    QList<Define *> savedDefines;

    ui->setupUi(this);
    ControlPanel::setWindowTitle("Custom control panel, right click for options");
    Systems = S;

    // Init a number of variables
    float StepSize = 1.0;
    parentCP = pcp;
    SerialWatchDog = 0;
    HelpFile.clear();
    LoadedConfig = false;
    mc        = NULL;
    DCBgroups = NULL;
    statusBar = NULL;
    modbus = NULL;
    Defines.clear();
    ARBcompressorButton.clear();
    comp.clear();
    TC.clear();
    TextLabels.clear();
    RFchans.clear();
    RFCchans.clear();
    rfa.clear();
    ADCchans.clear();
    DACchans.clear();
    DCBchans.clear();
    DCBoffsets.clear();
    DCBenables.clear();
    DIOchannels.clear();
    ESIchans.clear();
    ARBchans.clear();
    Ccontrols.clear();
    Cpanels.clear();
    GroupBoxes.clear();
    Tabs.clear();
    ScripButtons.clear();
    devices.clear();
    scriptStr.clear();
    plots.clear();
    Containers.clear();
    CPbuttons.clear();
    ModChans.clear();
    cpObjects.clear();
    iBox.clear();
    plotData.clear();
    UpdateHoldOff  = 0;
    UpdateStop     = false;
    ShutdownFlag   = false;
    RestoreFlag    = false;
    StartMIPScomms = false;
    SystemIsShutdown = false;
    scriptconsole = NULL;
    tcp = NULL;
    sl = NULL;
    tm = NULL;
    properties = prop;
    help = new Help();
    comments = new Help();
    LogFile.clear();
    // Allow user to select the configuration file
    QString fileName;
    if(CPfileName == "") fileName = QFileDialog::getOpenFileName(this, tr("Load Configuration from File"),"",tr("cfg (*.cfg);;All files (*.*)"));
    else fileName = CPfileName;
    if((fileName == "") || (fileName.isEmpty())) return;
    QFile file(fileName);
    ControlPanelFile = fileName;
    if(properties != NULL) properties->Log("Control panel loaded: " + ControlPanelFile);
    MIPSnames.clear();
    for(int i=0;i<Systems.count();i++)
    {
       QString res = Systems[i]->MIPSname + ": " + Systems[i]->SendMess("GVER\n");
       // Add MIPS firmware version to log file
       if(properties != NULL) properties->Log(Systems[i]->MIPSname + ": " + Systems[i]->SendMess("GVER\n"));
       MIPSnames += "# " + res + "\n";
    }
    // Open UDP socket to send commands to reader app
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress("127.0.0.1"), 7755);
    QString mess = "Loading control panel, "+ ControlPanelFile;
    udpSocket->writeDatagram(mess.toLocal8Bit(),QHostAddress("127.0.0.1"), 7755);
    // Read the configuration file and create the form as
    // well as all the controls.
    ui->lblBackground->setObjectName("");
    //Container = ui->lblBackground;
    Containers.append(ui->lblBackground);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            if(line.isNull()) break;
            if(line.trimmed().startsWith("#")) continue;
            resList = line.trimmed().split(",");
            // Add the Define subsitutions here, also trim all the tokens to allow
            // the use of white space.
            if((resList[0].toUpper() == "DEFINE") && (resList.length()==3))
            {
                Defines.append(new Define);
                Defines.last()->Name = resList[1].trimmed();
                Defines.last()->Value = resList[2].trimmed();
            }
            for(int i=0;i<Defines.count();i++)
            {
                for(int j=0;j<resList.count();j++)
                {
                    resList[j] = resList[j].trimmed();
                    if(resList[j] == Defines[i]->Name) resList[j] = Defines[i]->Value;
                }
            }
            // Ignore subroutines while processing the cfg file
            if(resList[0].toUpper() == "SUBROUTINE")
            {
                // Read lines until end of subroutine statement
                do
                {
                    line = stream.readLine();
                    if(line.trimmed().startsWith("#")) continue;
                    if(line.trimmed().toUpper().startsWith("ENDSUBROUTINE")) break;
                } while(!line.isNull());
                continue;
            }
            // Process subroutine call. Do the following:
            //  - Save the file pointer
            //  - Find the Subroutine, add its arguments to the list of defines
            //  - Execute until we see EndSubroutine then restore the file pointer
            if((resList[0].toUpper() == "CALL") && (resList.length()>=2))
            {
                // Save file pointer
                filePos = stream.pos();
                // Find the subroutine in the CFG file
                stream.seek(0);
                do
                {
                   line = stream.readLine();
                   if(line.trimmed().startsWith("#")) continue;
                   if(line.trimmed().toUpper().startsWith("SUBROUTINE"))
                   {
                       // We found a subroutine, see if its the right one and has the correct number of args
                       // note this supports overloading
                       QStringList subList;
                       subList = line.trimmed().split(',');
                       if((subList[1] == resList[1]) && (subList.count() == resList.count()))
                       {
                           // Save the current define list and apply args using the define list
                           savedDefines = Defines;
                           Defines.clear();
                           for(int i=2;i<subList.count();i++)
                           {
                               Defines.append((new Define));
                               Defines.last()->Name = subList[i];
                               Defines.last()->Value = resList[i];
                           }
                           break;
                       }
                   }
                } while(!line.isNull());
             }
            // End of Call processing
            if((resList[0].toUpper() == "ENDSUBROUTINE") && (resList.length()==1))
            {
                if(filePos >= 0)
                {
                    stream.seek(filePos);
                    line = stream.readLine();
                    Defines.clear();
                    Defines = savedDefines;
                    savedDefines.clear();
                    filePos = -1;
                }
            }
            if((resList[0].toUpper() == "SCRIPT") && (resList.length()==2))
            {
                scriptStr.append(new ScriptString);
                scriptStr.last()->Name = resList[1].trimmed();
                scriptStr.last()->ScriptText="";
                // Now read the config eing until the ENDSCRIPT statement
                do
                {
                    line = stream.readLine();
                    if(line.trimmed().toUpper().startsWith("ENDSCRIPT")) break;
                    scriptStr.last()->ScriptText += line.trimmed() + "\n";
                } while(!line.isNull());
                continue;
            }
            if((resList[0].toUpper() == "SERIALWATCHDOG") && (resList.length()==2)) SerialWatchDog = resList[1].toInt();
            if((resList[0].toUpper() == "SIZE") && (resList.length()==3))
            {
                this->setFixedSize(resList[1].toInt(),resList[2].toInt());
                ui->lblBackground->setGeometry(0,0,resList[1].toInt(),resList[2].toInt());
                statusBar = new QStatusBar(this);
                statusBar->setGeometry(0,resList[2].toInt()-18,resList[1].toInt(),18);
                statusBar->showMessage("Control panel loaded: " + QDateTime().currentDateTime().toString());
                statusBar->raise();
                connect(statusBar,SIGNAL(messageChanged(QString)),this,SLOT(slotLogStatusBarMessage(QString)));
            }
            if((resList[0].toUpper() == "IMAGE") && (resList.length()==2))
            {
                #ifdef Q_OS_MAC
                if(resList[1].startsWith("~")) resList[1] = QDir::homePath() + "/" + resList[1].mid(2);
                #endif
                QPixmap img(findFile(resList[1],QFileInfo(ControlPanelFile).canonicalPath()));
                ui->lblBackground->clear();
                ui->lblBackground->setPixmap(img);
            }
            if((resList[0].toUpper() == "PLOTDATA") && (resList.length()==9))
            {
                plotData.append(new PlotData(Containers.last(),resList[1],resList[2],resList[3],resList[4].toInt(),resList[5].toInt(),resList[6].toInt(),resList[7].toInt(),resList[8].toInt()));
                plotData.last()->Show();
                plotData.last()->w->installEventFilter(this);
                plotData.last()->w->setMouseTracking(true);
                cpObjects.append(stream.pos());
                cpObjects.append("PlotData");
                cpObjects.append(QVariant::fromValue(plotData.last()->w));
            }
            if((resList[0].toUpper() == "IMAGEBOX") && (resList.length()==6))
            {
                iBox.append(new QLabel(Containers.last()));
                iBox.last()->setGeometry(resList[4].toInt(),resList[5].toInt(),resList[2].toInt(),resList[3].toInt());
                #ifdef Q_OS_MAC
                if(resList[1].startsWith("~")) resList[1] = QDir::homePath() + "/" + resList[1].mid(2);
                #endif
                QPixmap img(findFile(resList[1],QFileInfo(ControlPanelFile).canonicalPath()));
                img = img.scaled(resList[3].toInt(),resList[3].toInt(), Qt::IgnoreAspectRatio);
                iBox.last()->clear();
                iBox.last()->setPixmap(img);
                iBox.last()->show();
                iBox.last()->lower();
                iBox.last()->installEventFilter(this);
                iBox.last()->setMouseTracking(true);
                cpObjects.append(stream.pos());
                cpObjects.append("ImageBox");
                cpObjects.append(QVariant::fromValue(iBox.last()));
            }
            if((resList[0].toUpper() == "DEVICE") && (resList.length()==6))
            {
                devices.append(new Device(Containers.last(),resList[1],findFile(resList[2],QFileInfo(ControlPanelFile).canonicalPath()),resList[3],resList[4].toInt(),resList[5].toInt()));
                devices.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("Device");
                cpObjects.append(QVariant::fromValue(devices.last()));
            }
            if((resList[0].toUpper() == "TEXTLABEL") && (resList.length()==5))
            {
                TextLabels.append(new TextLabel(Containers.last(),resList[1],resList[2].toInt(),resList[3].toInt(),resList[4].toInt()));
                TextLabels.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("TextLabel");
                cpObjects.append(QVariant::fromValue(TextLabels.last()));
            }
            if((resList[0].toUpper() == "RFCHANNEL") && (resList.length()==6))
            {
                RFchans.append(new RFchannel(Containers.last(),resList[1],resList[2],resList[4].toInt(),resList[5].toInt()));
                RFchans.last()->Channel = resList[3].toInt();
                RFchans.last()->comms = FindCommPort(resList[2],Systems);
                RFchans.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("RFchannel");
                cpObjects.append(QVariant::fromValue(RFchans.last()));
            }
            if((resList[0].toUpper() == "RFCCHANNEL") && (resList.length()==6))
            {
                RFCchans.append(new RFCchannel(Containers.last(),resList[1],resList[2],resList[4].toInt(),resList[5].toInt()));
                RFCchans.last()->Channel = resList[3].toInt();
                RFCchans.last()->comms = FindCommPort(resList[2],Systems);
                RFCchans.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("RFCchannel");
                cpObjects.append(QVariant::fromValue(RFCchans.last()));
            }
            if((resList[0].toUpper() == "ADCCHANNEL") && (resList.length()>=6))
            {
                ADCchans.append(new ADCchannel(Containers.last(),resList[1],resList[2],resList[4].toInt(),resList[5].toInt()));
                ADCchans.last()->Channel = resList[3].toInt();
                ADCchans.last()->comms = FindCommPort(resList[2],Systems);
                if(resList.length()>=7)  ADCchans.last()->m = resList[6].toFloat();
                if(resList.length()>=8)  ADCchans.last()->b = resList[7].toFloat();
                if(resList.length()>=9)  ADCchans.last()->Units = resList[8];
                if(resList.length()>=10) ADCchans.last()->Format = resList[9];
                if(resList.length()>=11) ADCchans.last()->U = resList[10].toFloat();
                if(resList.length()>=12) ADCchans.last()->LLimit = resList[11].trimmed();
                ADCchans.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("ADCchannel");
                cpObjects.append(QVariant::fromValue(ADCchans.last()));
            }
            if((resList[0].toUpper() == "DACCHANNEL") && (resList.length()>=6))
            {
                DACchans.append(new DACchannel(Containers.last(),resList[1],resList[2],resList[4].toInt(),resList[5].toInt()));
                DACchans.last()->Channel = resList[3].toInt();
                DACchans.last()->comms = FindCommPort(resList[2],Systems);
                if(resList.length()>=7)  DACchans.last()->m = resList[6].toFloat();
                if(resList.length()>=8)  DACchans.last()->b = resList[7].toFloat();
                if(resList.length()>=9)  DACchans.last()->Units = resList[8];
                if(resList.length()>=10) DACchans.last()->Format = resList[9];
                DACchans.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("DACchannel");
                cpObjects.append(QVariant::fromValue(DACchans.last()));
            }
            if((resList[0].toUpper() == "CPANEL") && (resList.length()==5))
            {
                Cpanels.append(new Cpanel(Containers.last(),resList[1],findFile(resList[2],QFileInfo(ControlPanelFile).canonicalPath()),resList[3].toInt(),resList[4].toInt(),Systems,properties,this));
                Cpanels.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("Cpanel");
                cpObjects.append(QVariant::fromValue(Cpanels.last()));
            }
            if((resList[0].toUpper() == "STEP") && (resList.length()==2))
            {
                StepSize = resList[1].toFloat();
            }
            if((resList[0].toUpper() == "CCONTROL") && (resList.length()>=10))
            {
                Ccontrols.append(new Ccontrol(Containers.last(),resList[1],resList[2],resList[3],resList[4],resList[5],resList[6],resList[7],resList[8].toInt(),resList[9].toInt()));
                if(resList.length()>=11) Ccontrols.last()->Dtype = resList[10];
                Ccontrols.last()->comms = FindCommPort(resList[2],Systems);
                Ccontrols.last()->Step = StepSize;
                Ccontrols.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("Ccontrol");
                cpObjects.append(QVariant::fromValue(Ccontrols.last()));
            }
            if(resList[0].toUpper() == "COMBOBOXLIST")
            {
                QString list;
                list.clear();
                for(int i=1;i<resList.count();i++)
                {
                    if(i>1) list += ",";
                    list += resList[i];
                }
                Ccontrols.last()->SetList(list);
            }
            if((resList[0].toUpper() == "DCBCHANNEL") && (resList.length()==6))
            {
                DCBchans.append(new DCBchannel(Containers.last(),resList[1],resList[2],resList[4].toInt(),resList[5].toInt()));
                DCBchans.last()->Channel = resList[3].toInt();
                DCBchans.last()->comms = FindCommPort(resList[2],Systems);
                DCBchans.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("DCBchannel");
                cpObjects.append(QVariant::fromValue(DCBchans.last()));
            }
            if((resList[0].toUpper() == "DCBOFFSET") && (resList.length()==6))
            {
                DCBoffsets.append(new DCBoffset(Containers.last(),resList[1],resList[2],resList[4].toInt(),resList[5].toInt()));
                DCBoffsets.last()->Channel = resList[3].toInt();
                DCBoffsets.last()->comms = FindCommPort(resList[2],Systems);
                DCBoffsets.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("DCBoffset");
                cpObjects.append(QVariant::fromValue(DCBoffsets.last()));
            }
            if((resList[0].toUpper() == "DCBENABLE") && (resList.length()==5))
            {
                DCBenables.append(new DCBenable(Containers.last(),resList[1],resList[2],resList[3].toInt(),resList[4].toInt()));
                DCBenables.last()->comms = FindCommPort(resList[2],Systems);
                DCBenables.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("DCBenable");
                cpObjects.append(QVariant::fromValue(DCBenables.last()));
            }
            if((resList[0].toUpper() == "DIOCHANNEL") && (resList.length()>=6))
            {
                DIOchannels.append(new DIOchannel(Containers.last(),resList[1],resList[2],resList[4].toInt(),resList[5].toInt()));
                DIOchannels.last()->Channel = resList[3];
                DIOchannels.last()->comms = FindCommPort(resList[2],Systems);
                DIOchannels.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("DIOchannel");
                cpObjects.append(QVariant::fromValue(DIOchannels.last()));
                if(resList.length() >= 7)
                {
                    if(resList[6].toUpper().trimmed() == "TRUE") DIOchannels.last()->ReadOnly=true;
                    if(resList[6].toUpper().trimmed() == "FALSE") DIOchannels.last()->ReadOnly=false;
                }
            }
            if((resList[0].toUpper() == "ESICHANNEL") && (resList.length()==6))
            {
                ESIchans.append(new ESI(Containers.last(),resList[1],resList[2],resList[4].toInt(),resList[5].toInt()));
                ESIchans.last()->Channel = resList[3].toInt();
                ESIchans.last()->comms = FindCommPort(resList[2],Systems);
                ESIchans.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("ESI");
                cpObjects.append(QVariant::fromValue(ESIchans.last()));
            }
            if((resList[0].toUpper() == "ARBCHANNEL") && (resList.length()==6))
            {
                ARBchans.append(new ARBchannel(Containers.last(),resList[1],resList[2],resList[4].toInt(),resList[5].toInt()));
                ARBchans.last()->Channel = resList[3].toInt();
                ARBchans.last()->comms = FindCommPort(resList[2],Systems);
                ARBchans.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("ARBchannel");
                cpObjects.append(QVariant::fromValue(ARBchans.last()));
            }
            if((resList[0].toUpper() == "RFAMP") && (resList.length()==6))
            {
                rfa.append(new RFamp(Containers.last(),resList[1],resList[2],resList[3].toInt()));
                rfa.last()->comms = FindCommPort(resList[2],Systems);
                QWidget *widget = new QWidget(this);
                widget->setGeometry(resList[4].toInt(),resList[5].toInt(),280,290);
                QVBoxLayout *layout = new QVBoxLayout(widget);
                layout->addWidget(rfa.last());
                rfa.last()->show();
                rfa.last()->installEventFilter(this);
                rfa.last()->setMouseTracking(true);
                widget->setMouseTracking(true);
                cpObjects.append(stream.pos());
                cpObjects.append("RFamp");
                cpObjects.append(QVariant::fromValue(widget));
            }
            if((resList[0].toUpper() == "MODBUS") && (resList.length()==4))
            {
               modbus = new ModBus(resList[1],resList[2],resList[3]);
            }
            if((resList[0].toUpper() == "MODCHANNEL") && (resList.length()==10))
            {
               // ModChannel,Name,Add,Table,Chan,m,b,units,x,y
               ModChans.append(new ModChannel(Containers.last(),resList[1],resList[2].toInt(),resList[3].toInt(),resList[4].toInt(),resList[5].toFloat(),resList[6].toFloat(),resList[7],resList[8].toInt(),resList[9].toInt()));
               ModChans.last()->modbus = modbus;
               ModChans.last()->Show();
               cpObjects.append(stream.pos());
               cpObjects.append("ModChannel");
               cpObjects.append(QVariant::fromValue(ModChans.last()));
            }
            if((resList[0].toUpper() == "MODCHANNELWRITABLE") && (resList.length()==2))
            {
               if(ModChans.count() > 0)
               {
                   if(resList[1].toUpper() == "TRUE")
                   {
                       ModChans.last()->Writable = true;
                   }
                   else
                   {
                       ModChans.last()->Writable = false;
                   }
               }
            }
            if((resList[0].toUpper() == "SCRIPTBUTTON") && (resList.length()==5))
            {
                QString sfn = findFile(resList[2],QFileInfo(ControlPanelFile).canonicalPath());
                if(sfn == "") sfn = resList[2];
                ScripButtons.append(new ScriptButton(Containers.last(), resList[1], sfn, resList[3].toInt(), resList[4].toInt(), properties, statusBar));
                ScripButtons.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("ScriptButton");
                cpObjects.append(QVariant::fromValue(ScripButtons.last()));
            }
            if((resList[0].toUpper() == "CALLONUPDATE") && (resList.length()==2))
            {
               if(ScripButtons.count() >= 1)
                  if(resList[1].toUpper().trimmed() == "TRUE") ScripButtons.last()->CallOnUpdate = true;
            }
            if((resList[0].toUpper() == "CALLONSTART") && (resList.length()==2))
            {
               if(ScripButtons.count() >= 1)
                  if(resList[1].toUpper().trimmed() == "TRUE") ScripButtons.last()->CallOnStart = true;
            }
            if((resList[0].toUpper() == "CPBUTTON") && (resList.length()==5))
            {
                CPbuttons.append(new CPbutton(Containers.last(), resList[1], findFile(resList[2],QFileInfo(ControlPanelFile).canonicalPath()), resList[3].toInt(), resList[4].toInt()));
                CPbuttons.last()->Show();
                connect(CPbuttons.last(),SIGNAL(CPselected(QString)),this,SLOT(slotCPselected(QString)));
                cpObjects.append(stream.pos());
                cpObjects.append("CPbutton");
                cpObjects.append(QVariant::fromValue(CPbuttons.last()));
            }
            if((resList[0].toUpper() == "STATUSLIGHT") && (resList.length()==4))
            {
                sl = new StatusLight(Containers.last(),resList[1],resList[2].toInt(), resList[3].toInt());
                sl->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("StatusLight");
                cpObjects.append(QVariant::fromValue(sl));
            }
            if((resList[0].toUpper() == "TEXTMESSAGE") && (resList.length()==4))
            {
                tm = new TextMessage(Containers.last(),resList[1],resList[2].toInt(), resList[3].toInt());
                tm->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("TextMessage");
                cpObjects.append(QVariant::fromValue(tm));
            }
            // Tab,width, Name, height, X, Y, Tab names....
            if((resList[0].toUpper() == "TAB") && (resList.length()>=7))
            {
                Tabs.append(new QTabWidget(Containers.last()));
                Tabs.last()->setGeometry(resList[4].toInt(),resList[5].toInt(),resList[2].toInt(),resList[3].toInt());
                Tabs.last()->tabBar()->setStyleSheet("QTabBar::tab:selected {color: white; background-color: rgb(90,90,255);}");
                for(int i=6; i<resList.length(); i++)
                {
                    Tabs.last()->addTab(new QWidget(),resList[i]);
                    Tabs.last()->widget(i-6)->setObjectName(resList[i]);
                }
                if(Containers.last()->objectName() != "") Tabs.last()->setObjectName(Containers.last()->objectName() + "." + resList[1]);
                else Tabs.last()->setObjectName(resList[1]);
                Tabs.last()->installEventFilter(this);
                Tabs.last()->setMouseTracking(true);
                cpObjects.append(stream.pos());
                cpObjects.append("Tab");
                cpObjects.append(QVariant::fromValue(Tabs.last()));
            }
            // Select tab for control placement
            // TabSelect,Name,TabName
            // TabSelectEnd
            if((resList[0].toUpper() == "TABSELECT") && (resList.length()==3))
            {
                // Find this tab name in the list of tabs
                QTabWidget *tab;
                foreach(tab, Tabs)
                {
                    if(tab->objectName() == resList[1])
                    {
                        // Now find the proper tab
                        for(int i = 0; i<tab->count(); i++)
                        {
                            if(tab->tabText(i) == resList[2]) Containers.last() = tab->widget(i);
                        }
                    }
                }
            }
            if(resList[0].toUpper() == "TABSELECTEND")
            {
                Containers.removeLast();
                if(Containers.count() == 0) Containers.append(ui->lblBackground);
            }
            // GroupBox,name,width,height,X,Y  // Command to start group
            // GroupBoxEnd                     // Signals end of group box
            if((resList[0].toUpper() == "GROUPBOX") && (resList.length()==6))
            {
                GroupBoxes.append(new QGroupBox(Containers.last()));
                GroupBoxes.last()->setGeometry(resList[4].toInt(),resList[5].toInt(),resList[2].toInt(),resList[3].toInt());
                GroupBoxes.last()->setTitle(resList[1]);
                if(Containers.last()->objectName() != "") GroupBoxes.last()->setObjectName(Containers.last()->objectName() + "." + resList[1]);
                else GroupBoxes.last()->setObjectName(resList[1]);
                Containers.append(GroupBoxes.last());
                GroupBoxes.last()->installEventFilter(this);
                GroupBoxes.last()->setMouseTracking(true);
                cpObjects.append(stream.pos());
                cpObjects.append("GroupBox");
                cpObjects.append(QVariant::fromValue(GroupBoxes.last()));
            }
            if(resList[0].toUpper() == "GROUPBOXEND")
            {
                Containers.removeLast();
                if(Containers.count() == 0) Containers.append(ui->lblBackground);
            }
            if((resList[0].toUpper() == "TIMING") && (resList.length()==5))
            {
                TC.append(new TimingControl(Containers.last(),resList[1],resList[2],resList[3].toInt(),resList[4].toInt()));
                TC.last()->comms = FindCommPort(resList[2],Systems);
                TC.last()->statusBar = statusBar;
                TC.last()->properties = properties;
                TC.last()->Show();
                connect(TC.last(),SIGNAL(dataAcquired(QString)),this,SLOT(slotDataAcquired(QString)));
                connect(TC.last(),SIGNAL(dataFileDefined(QString)),this,SLOT(slotDataFileDefined(QString)));
                cpObjects.append(stream.pos());
                cpObjects.append("Timing");
                cpObjects.append(QVariant::fromValue(TC.last()));
            }
            if((resList[0].toUpper() == "FRAMECNTADJ") && (resList.length()==2))
            {
                if(TC.count() > 0)
                {
                    TC.last()->FrameCtAdj = resList[1].toInt();
                    TC.last()->TG->FrameCtAdj = resList[1].toInt();
                }
            }
            if((resList[0].toUpper() == "ALWAYSGENERATE") && (resList.length()==2))
            {
                if((resList[1].toUpper() == "YES"))
                {
                    if(TC.count() > 0) TC.last()->AlwaysGenerate = true;
                }
            }
            if((resList[0].toUpper() == "EVENTCONTROL") && (resList.length()==5))
            {
                TC.last()->TG->EC.append(new EventControl(Containers.last(),resList[1],resList[2],resList[3].toInt(),resList[4].toInt()));
                TC.last()->TG->EC.last()->Show();
                connect(TC.last()->TG->EC.last(),SIGNAL(EventChanged(QString,QString)),TC.last(),SLOT(slotEventChanged(QString,QString)));
            }
            if((resList[0].toUpper() == "FILENAME") && (resList.length()==2)) if(TC.count() > 0) TC.last()->fileName = resList[1];
            if((resList[0].toUpper() == "SHUTDOWN") && (resList.length()==4))
            {
                SD = new Shutdown(Containers.last(),resList[1],resList[2].toInt(),resList[3].toInt());
                SD->Show();
                connect(SD,SIGNAL(ShutdownSystem()),this,SLOT(pbSD()));
                connect(SD,SIGNAL(EnableSystem()),this,SLOT(pbSE()));
                SD->setFocus();
                cpObjects.append(stream.pos());
                cpObjects.append("Shutdown");
                cpObjects.append(QVariant::fromValue(SD));
            }
            if((resList[0].toUpper() == "SAVELOAD") && (resList.length()==5))
            {
                SL = new SaveLoad(Containers.last(),resList[1],resList[2],resList[3].toInt(),resList[4].toInt());
                SL->Show();
                connect(SL,SIGNAL(Save()),this,SLOT(pbSave()));
                connect(SL,SIGNAL(Load()),this,SLOT(pbLoad()));
                cpObjects.append(stream.pos());
                cpObjects.append("SaveLoad");
                cpObjects.append(QVariant::fromValue(SL));
            }
            // COMPRESSOR,name,MIPS name,X,Y
            if((resList[0].toUpper() == "COMPRESSOR") && (resList.length()==5))
            {
                // Enable the ARB compressor button
                ARBcompressorButton.append(new QPushButton(resList[1],Containers.last()));
                ARBcompressorButton.last()->setGeometry(resList[3].toInt(),resList[4].toInt(),150,32);
                ARBcompressorButton.last()->setAutoDefault(false);
                ARBcompressorButton.last()->setToolTip("Press this button to edit the compression options");
                comp.append(new Compressor(Containers.last(),resList[1],resList[2]));
                comp.last()->comms = FindCommPort(resList[2],Systems);
                connect(ARBcompressorButton.last(),SIGNAL(pressed()),this,SLOT(pbARBcompressor()));
                ARBcompressorButton.last()->installEventFilter(this);
                ARBcompressorButton.last()->setMouseTracking(true);
                cpObjects.append(stream.pos());
                cpObjects.append("Compressor");
                cpObjects.append(QVariant::fromValue(ARBcompressorButton.last()));
            }
            if((resList[0].toUpper() == "MIPSCOMMS") && (resList.length()==3))
            {
                // Enable the MIPS communication button
                MIPScommsButton = new QPushButton("MIPS comms",Containers.last());
                MIPScommsButton->setGeometry(resList[1].toInt(),resList[2].toInt(),150,32);
                MIPScommsButton->setAutoDefault(false);
                MIPScommsButton->setToolTip("Press this button to manually send commands to MIPS");
                connect(MIPScommsButton,SIGNAL(pressed()),this,SLOT(pbMIPScomms()));
                MIPScommsButton->installEventFilter(this);
                MIPScommsButton->setMouseTracking(true);
                cpObjects.append(stream.pos());
                cpObjects.append("MIPScomms");
                cpObjects.append(QVariant::fromValue(MIPScommsButton));
            }
            if(resList[0].toUpper() == "ACQUIRE")
            {
                if(TC.count() > 0)
                {
                    TC.last()->AD->Acquire = line.mid(line.indexOf(",")+1);
                    #ifdef Q_OS_MAC
                    if(TC.last()->AD->Acquire .startsWith("~")) TC.last()->AD->Acquire  = QDir::homePath() + "/" + TC.last()->AD->Acquire .mid(2);
                    #endif
                }
            }
            if((resList[0].toUpper() == "SCRIPT") && (resList.length()==3))
            {
                // Use QTscript and place a button at the defined location.
                scriptButton = new QPushButton("Scripting",Containers.last());
                scriptButton->setGeometry(resList[1].toInt(),resList[2].toInt(),150,32);
                scriptButton->setAutoDefault(false);
                scriptButton->setToolTip("Press this button load or define a script");
                connect(scriptButton,SIGNAL(pressed()),this,SLOT(pbScript()));
                scriptconsole = new ScriptingConsole(this,properties);
                scriptButton->installEventFilter(this);
                scriptButton->setMouseTracking(true);
                cpObjects.append(stream.pos());
                cpObjects.append("ScriptButton");
                cpObjects.append(QVariant::fromValue(scriptButton));
            }
            if((resList[0].toUpper() == "DCBGROUPS") && (resList.length()==3))
            {
                DCBgroups = new DCBiasGroups(Containers.last(),resList[1].toInt(),resList[2].toInt());
                DCBgroups->Show();
                connect(DCBgroups,SIGNAL(disable()),this,SLOT(DCBgroupDisable()));
                connect(DCBgroups,SIGNAL(enable()),this,SLOT(DCBgroupEnable()));
                cpObjects.append(stream.pos());
                cpObjects.append("DCBgroups");
                cpObjects.append(QVariant::fromValue(DCBgroups));
            }
            if((resList[0].toUpper() == "INITPARMS") && (resList.length()==2))
            {
                // Load the file and apply the initialization MIPS commands
                #ifdef Q_OS_MAC
                if(resList[1].startsWith("~")) resList[1] = QDir::homePath() + "/" + resList[1].mid(2);
                #endif
                InitMIPSsystems(findFile(resList[1],QFileInfo(ControlPanelFile).canonicalPath()));
            }
            if((resList[0].toUpper() == "SENDCOMMAND") && (resList.length()>2))
            {
                // Send a command to the selected MIPS system
                SendCommand(resList[1],line.mid(line.indexOf(resList[1]) + resList[1].length() + 1) + "\n");
            }
            if((resList[0].toUpper() == "HELP") && (resList.length()==2))
            {
                HelpFile = resList[1];
                #ifdef Q_OS_MAC
                if(HelpFile.startsWith("~")) HelpFile = QDir::homePath() + "/" + HelpFile.mid(2);
                #endif
            }
            if((resList[0].toUpper() == "TCPSERVER") && (resList.length()==2))
            {
                tcp = new TCPserver();
                tcp->port = resList[1].toInt();
                tcp->statusbar = statusBar;
                tcp->listen();
                connect(tcp,SIGNAL(lineReady()),this,SLOT(tcpCommand()));
            }
        } while(!line.isNull());
    }
    file.close();
    foreach(TimingControl *tc, TC)
    {
        tc->TG->AddSignal("Trig out","t");
        tc->TG->AddSignal("Delta t","d");
        foreach(ARBchannel *arb, ARBchans)
        {
            if(arb->MIPSnm == tc->MIPSnm)
            {
                tc->TG->AddSignal("ARBcomp","r");
                tc->TG->AddSignal("ARBsync","s");
                tc->TG->AddSignal("ARBct","c");
                break;
            }
        }
        ControlPanel *cp = this;
        while(cp != NULL)
        {
            // Add DC bias channels
            foreach(DCBchannel *dcb, cp->DCBchans) if(dcb->MIPSnm == tc->MIPSnm) tc->TG->AddSignal(dcb->Title, QString::number(dcb->Channel));
            // Add DIO channels
            foreach(DIOchannel *dio, cp->DIOchannels) if(dio->MIPSnm == tc->MIPSnm) tc->TG->AddSignal(dio->Title, dio->Channel);
            // Add RF drive channels
            foreach(RFchannel *rfc, cp->RFchans) if(rfc->MIPSnm == tc->MIPSnm) tc->TG->AddSignal(rfc->Title, QString::number(rfc->Channel + 32));
            foreach(RFCchannel *rfc, cp->RFCchans) if(rfc->MIPSnm == tc->MIPSnm) tc->TG->AddSignal(rfc->Title, QString::number(rfc->Channel + 32));
            cp = cp->parentCP;
        }
    }
    // Look at all the script buttons and if the file name matches a script
    // string then place script text in the script button and set the
    // fixed string flag
    foreach(ScriptButton *scrpt, ScripButtons)
    {
        //qDebug() << scrpt->FileName;
        foreach(ScriptString *SS, scriptStr)
        {
            //qDebug() << SS->Name;
            if(SS->Name == scrpt->FileName)
            {
                scrpt->ScriptText = SS->ScriptText;
                scrpt->ScriptTextFixed = true;
                //qDebug() << "match";
            }
        }
    }
    LoadedConfig = true;
    //connect(ui->lblBackground, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(popupHelp(QPoint)));
    ui->lblBackground->installEventFilter(this);

    // Popup options menu actions
    Comments = new QAction("Comments", this);
    connect(Comments, SIGNAL(triggered()), this, SLOT(slotComments()));
    SaveCP = new QAction("Save this Control Panel", this);
    connect(SaveCP, SIGNAL(triggered()), this, SLOT(slotSaveCP()));
    GeneralHelp = new QAction("General help", this);
    connect(GeneralHelp, SIGNAL(triggered()), this, SLOT(slotGeneralHelp()));
    MIPScommands = new QAction("MIPS commands", this);
    connect(MIPScommands, SIGNAL(triggered()), this, SLOT(slotMIPScommands()));
    ScriptHelp  = new QAction("Script help", this);
    connect(ScriptHelp, SIGNAL(triggered()), this, SLOT(slotScriptHelp()));
    if(!HelpFile.isEmpty())
    {
        ThisHelp = new QAction("This control panel help", this);
        connect(ThisHelp, SIGNAL(triggered()), this, SLOT(slotThisControlPanelHelp()));
    }
    OpenLogFile = new QAction("Open data log", this);
    connect(OpenLogFile, SIGNAL(triggered()), this, SLOT(slotOpenLogFile()));
    CloseLogFile = new QAction("Close data log", this);
    connect(CloseLogFile, SIGNAL(triggered()), this, SLOT(slotCloseLogFile()));
    if(SerialWatchDog > 0)
    {
        if(properties != NULL) properties->Log("Enabling serial watch dog on MIPS system(s)");
        for(int i=0;i<Systems.count();i++)
        {
           Systems[i]->SendCommand("SSERWD," + QString::number(SerialWatchDog) + "\n");
        }
    }
}

bool ControlPanel::eventFilter(QObject* object, QEvent* event)
{
    if(object == scriptButton) if(moveWidget(object, scriptButton, scriptButton, event)) return true;
    if(object == MIPScommsButton) if(moveWidget(object, MIPScommsButton, MIPScommsButton, event)) return true;
    for(int i=0;i<ARBcompressorButton.count();i++) if(object == ARBcompressorButton[i]) if(moveWidget(object, ARBcompressorButton[i], ARBcompressorButton[i], event)) return true;
    for(int i=0;i<GroupBoxes.count();i++) if(object == GroupBoxes[i]) if(moveWidget(object, GroupBoxes[i], GroupBoxes[i], event)) return true;
    for(int i=0;i<Tabs.count();i++) if(object == Tabs[i]) if(moveWidget(object, Tabs[i], Tabs[i], event)) return true;
    for(int i=0;i<rfa.count();i++) if(object == rfa[i]) if(moveWidget(rfa[i]->parentWidget(), rfa[i]->parentWidget(), rfa[i]->parentWidget(), event)) return true;
    for(int i=0;i<iBox.count();i++) if(object == iBox[i]) if(moveWidget(iBox[i], iBox[i], iBox[i], event)) { if(!iBox[i]->property("moving").toBool()) iBox[i]->lower(); return true; }
    for(int i=0;i<plotData.count();i++) if(object == plotData[i]->w) if(moveWidget(object, plotData[i]->w, plotData[i]->w, event)) return true;

    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton)
        {
            // Check if the click happened within lblBackground's child widgets
            if (ui->lblBackground->childAt(mouseEvent->pos()) == nullptr)
            {
                // Handle right click event (e.g., call popupHelp)
                popupHelp(mouseEvent->pos());
                return true; // Consume the event (optional)
            }
        }
    }
    // Pass the event to the base class for further processing
    return QObject::eventFilter(object, event);
}

ControlPanel::~ControlPanel()
{
    // Press the on edit button to run the exit script
    Command("On exit");
    //
    if(SerialWatchDog > 0)
    {
        if(properties != NULL) properties->Log("Disabling serial watch dog on MIPS system(s)");
        for(int i=0;i<Systems.count();i++)
        {
           Systems[i]->SendCommand("SSERWD,0\n");
        }
    }
    if(properties != NULL) properties->Log("Control panel unloading");
    for(int i=0;i<devices.count();i++) delete devices[i];
    for(int i=0;i<Cpanels.count();i++) delete Cpanels[i]->cp;
    for(int i=0;i<ScripButtons.count();i++) delete ScripButtons[i];
    for(int i=0;i<ModChans.count();i++) delete ModChans[i];
    if(modbus != NULL) delete(modbus);
    QApplication::processEvents();
    for(int i=0;i<plots.count();i++) delete plots[i];
    QApplication::processEvents();
    delete tcp;
    delete ui;
}

void ControlPanel::reject()
{
    QMessageBox msgBox;

    if(SystemIsShutdown)  // If its shutdown send warning
    {
        QString msg = "The system is currently shutdown, this means all voltages are disabled and ";
        msg += "all RF drive levels are set to 0. Make sure you have saved all your settings ";
        msg += "because when the control panel is restarted you will lose the shutdown recover data.\n";
        msgBox.setText(msg);
        msgBox.setInformativeText("Are you sure you want to exit?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if(ret == QMessageBox::No) return;
    }
    emit DialogClosed("");
}

// This function will try to locate the filename. The full path and file name
// will be returned if found or an empty string is the file can't be located.
// The following steps are performed:
// 1.) The file is tested as given in filename
// 2.) The file is tested at the posiblePath is not empty
// 3.) The app path is tested.
QString ControlPanel::findFile(QString filename, QString posiblePath)
{
    QString fn = QFileInfo(filename).fileName();
    if(QFileInfo::exists(filename)) return filename;
    if(QFileInfo::exists(posiblePath + QDir::separator() + fn)) return posiblePath + QDir::separator() + fn;
#ifdef Q_OS_MAC
    QString ext = ".app";
#else
    QString ext = ".exe";
#endif
    int i = QApplication::applicationFilePath().indexOf(QApplication::applicationName() + ext);
    if(i != -1)
    {
        QString ap = QApplication::applicationFilePath().left(i);
        if(QFileInfo::exists(ap + fn)) return ap + fn;
    }
    return "";
}

bool ControlPanel::isComms(void)
{
    if(Systems.count() == 0) return false;
    for(int i=0;i<Systems.count();i++)
    {
        if(!Systems[i]->isConnected()) return false;
    }
    return true;
}

void ControlPanel::popupHelp(QPoint qp)
{
    statusMessage("X= " + QString::number(qp.x()) + ",Y= " + QString::number(qp.y()));
    contextMenu2Dplot = new QMenu(tr("Help options"),this);
    contextMenu2Dplot->addAction(Comments);
    contextMenu2Dplot->addAction(SaveCP);
    contextMenu2Dplot->addAction(GeneralHelp);
    contextMenu2Dplot->addAction(MIPScommands);
    contextMenu2Dplot->addAction(ScriptHelp);
    if(!HelpFile.isEmpty()) contextMenu2Dplot->addAction(ThisHelp);
    if(LogFile == "") contextMenu2Dplot->addAction(OpenLogFile);
    if(LogFile != "") contextMenu2Dplot->addAction(CloseLogFile);
    QPoint p = this->pos();
    p.setX(p.x() + qp.x());
    p.setY(p.y() + qp.y());
    contextMenu2Dplot->exec(p);
}

void ControlPanel::slotComments(void)
{
    comments->SetTitle("Comments");
    comments->setText(CommentText);
    comments->show();
    connect(comments, SIGNAL(finished(int)), this, SLOT(slotCommentsFinished()));

}

void ControlPanel::slotCommentsFinished(void)
{
    CommentText = comments->getText();
}


void ControlPanel::slotSaveCP(void)
{
    QStringList resList;
    QFileDialog fileDialog;

    if(properties != NULL) fileDialog.setDirectory(properties->MethodesPath);
    QString fileName = fileDialog.getSaveFileName(this, tr("Save to Config File"),"",tr("Config (*.cfg);;All files (*.*)"));
    if(fileName == "") return;
    QFile fileOut(fileName);
    if(!fileOut.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream streamOut(&fileOut);

    // Open the current control panel file and look for each
    // object in the displayed control panel
    QFile file(ControlPanelFile);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        // We're going to streaming the file
        // to a QString
        QTextStream stream(&file);
        QString line;
        int cpp = 0;
        do
        {
            line = stream.readLine();
            resList = line.split(",");

            if(cpObjects.count() > cpp)
            {
                if(cpObjects[cpp] == stream.pos())
                {
                    cpp++;
                    if(cpObjects[cpp] == "DCBchannel")
                    {
                        cpp++;
                        DCBchannel *dcb = cpObjects[cpp++].value<DCBchannel*>();
                        if(resList.count() == 6)
                        {
                            resList[4] = QString::number(dcb->frmDCB->x());
                            resList[5] = QString::number(dcb->frmDCB->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "DCBoffset")
                    {
                        cpp++;
                        DCBoffset *dcbo = cpObjects[cpp++].value<DCBoffset*>();
                        if(resList.count() == 6)
                        {
                            resList[4] = QString::number(dcbo->frmDCBO->x());
                            resList[5] = QString::number(dcbo->frmDCBO->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "DCBenable")
                    {
                        cpp++;
                        DCBenable *dcbe = cpObjects[cpp++].value<DCBenable*>();
                        if(resList.count() == 5)
                        {
                            resList[3] = QString::number(dcbe->frmDCBena->x());
                            resList[4] = QString::number(dcbe->frmDCBena->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "RFchannel")
                    {
                        cpp++;
                        RFchannel *rfc = cpObjects[cpp++].value<RFchannel*>();
                        if(resList.count() == 6)
                        {
                            resList[4] = QString::number(rfc->gbRF->x());
                            resList[5] = QString::number(rfc->gbRF->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "RFCchannel")
                    {
                        cpp++;
                        RFCchannel *rfc = cpObjects[cpp++].value<RFCchannel*>();
                        if(resList.count() == 6)
                        {
                            resList[4] = QString::number(rfc->gbRF->x());
                            resList[5] = QString::number(rfc->gbRF->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "ESI")
                    {
                        cpp++;
                        ESI *esi = cpObjects[cpp++].value<ESI*>();
                        if(resList.count() == 6)
                        {
                            resList[4] = QString::number(esi->frmESI->x());
                            resList[5] = QString::number(esi->frmESI->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "ARBchannel")
                    {
                        cpp++;
                        ARBchannel *arbc = cpObjects[cpp++].value<ARBchannel*>();
                        if(resList.count() == 6)
                        {
                            resList[4] = QString::number(arbc->gbARB->x());
                            resList[5] = QString::number(arbc->gbARB->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "DIOchannel")
                    {
                        cpp++;
                        DIOchannel *dioc = cpObjects[cpp++].value<DIOchannel*>();
                        if(resList.count() >= 6)
                        {
                            resList[4] = QString::number(dioc->frmDIO->x());
                            resList[5] = QString::number(dioc->frmDIO->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "Device")
                    {
                        cpp++;
                        Device *dev = cpObjects[cpp++].value<Device*>();
                        if(resList.count() == 6)
                        {
                            resList[4] = QString::number(dev->frmDevice->x());
                            resList[5] = QString::number(dev->frmDevice->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "TextLabel")
                    {
                        cpp++;
                        TextLabel *txt = cpObjects[cpp++].value<TextLabel*>();
                        if(resList.count() == 5)
                        {
                            resList[3] = QString::number(txt->label->x());
                            resList[4] = QString::number(txt->label->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "ADCchannel")
                    {
                        cpp++;
                        ADCchannel *adc = cpObjects[cpp++].value<ADCchannel*>();
                        if(resList.count() >= 6)
                        {
                            resList[4] = QString::number(adc->frmADC->x());
                            resList[5] = QString::number(adc->frmADC->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "DACchannel")
                    {
                        cpp++;
                        DACchannel *dac = cpObjects[cpp++].value<DACchannel*>();
                        if(resList.count() >= 6)
                        {
                            resList[4] = QString::number(dac->frmDAC->x());
                            resList[5] = QString::number(dac->frmDAC->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "Cpanel")
                    {
                        cpp++;
                        Cpanel *cpl = cpObjects[cpp++].value<Cpanel*>();
                        if(resList.count() == 5)
                        {
                            resList[3] = QString::number(cpl->pbButton->x());
                            resList[4] = QString::number(cpl->pbButton->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "Ccontrol")
                    {
                        cpp++;
                        Ccontrol *cctrl = cpObjects[cpp++].value<Ccontrol*>();
                        if(resList.count() >= 10)
                        {
                            resList[8] = QString::number(cctrl->frmCc->x());
                            resList[9] = QString::number(cctrl->frmCc->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "RFamp")
                    {
                        cpp++;
                        QWidget *rfa = cpObjects[cpp++].value<QWidget*>();
                        if(resList.count() == 6)
                        {
                            resList[4] = QString::number(rfa->x());
                            resList[5] = QString::number(rfa->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "ModChannel")
                    {
                        cpp++;
                        ModChannel *modch = cpObjects[cpp++].value<ModChannel*>();
                        if(resList.count() == 10)
                        {
                            resList[8] = QString::number(modch->frmModChannel->x());
                            resList[9] = QString::number(modch->frmModChannel->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "ScriptButton")
                    {
                        cpp++;
                        ScriptButton *scrpt = cpObjects[cpp++].value<ScriptButton*>();
                        if(resList.count() == 5)
                        {
                            resList[3] = QString::number(scrpt->pbButton->x());
                            resList[4] = QString::number(scrpt->pbButton->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "CPbutton")
                    {
                        cpp++;
                        CPbutton *cpb = cpObjects[cpp++].value<CPbutton*>();
                        if(resList.count() == 5)
                        {
                            resList[3] = QString::number(cpb->pbCPselect->x());
                            resList[4] = QString::number(cpb->pbCPselect->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "StatusLight")
                    {
                        cpp++;
                        StatusLight *sl = cpObjects[cpp++].value<StatusLight*>();
                        if(resList.count() == 4)
                        {
                            resList[2] = QString::number(sl->label->x());
                            resList[3] = QString::number(sl->label->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "TextMessage")
                    {
                        cpp++;
                        TextMessage *tm = cpObjects[cpp++].value<TextMessage*>();
                        if(resList.count() == 4)
                        {
                            resList[2] = QString::number(tm->label->x());
                            resList[3] = QString::number(tm->label->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "Tab")
                    {
                        cpp++;
                        QTabWidget *tab = cpObjects[cpp++].value<QTabWidget*>();
                        if(resList.count() >= 7)
                        {
                            resList[5] = QString::number(tab->x());
                            resList[6] = QString::number(tab->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "GroupBox")
                    {
                        cpp++;
                        QGroupBox *gb = cpObjects[cpp++].value<QGroupBox*>();
                        if(resList.count() == 6)
                        {
                            resList[4] = QString::number(gb->x());
                            resList[5] = QString::number(gb->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "Timing")
                    {
                        cpp++;
                        TimingControl *tc = cpObjects[cpp++].value<TimingControl*>();
                        if(resList.count() == 5)
                        {
                            resList[3] = QString::number(tc->gbTC->x());
                            resList[4] = QString::number(tc->gbTC->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "SaveLoad")
                    {
                        cpp++;
                        SaveLoad *sl = cpObjects[cpp++].value<SaveLoad*>();
                        if(resList.count() == 5)
                        {
                            resList[3] = QString::number(sl->pbSave->x());
                            resList[4] = QString::number(sl->pbSave->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "Compressor")
                    {
                        cpp++;
                        Compressor *cmp = cpObjects[cpp++].value<Compressor*>();
                        if(resList.count() == 6)
                        {
                            resList[4] = QString::number(cmp->x());
                            resList[5] = QString::number(cmp->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "MIPScomms")
                    {
                        cpp++;
                        QPushButton *mipsc = cpObjects[cpp++].value<QPushButton*>();
                        if(resList.count() == 3)
                        {
                            resList[1] = QString::number(mipsc->x());
                            resList[2] = QString::number(mipsc->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "DCBgroups")
                    {
                        cpp++;
                        DCBiasGroups *dcbg = cpObjects[cpp++].value<DCBiasGroups*>();
                        if(resList.count() == 3)
                        {
                            resList[1] = QString::number(dcbg->gbDCBgroups->x());
                            resList[2] = QString::number(dcbg->gbDCBgroups->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "Shutdown")
                    {
                        cpp++;
                        Shutdown *sd = cpObjects[cpp++].value<Shutdown*>();
                        if(resList.count() == 4)
                        {
                            resList[2] = QString::number(sd->pbShutdown->x());
                            resList[3] = QString::number(sd->pbShutdown->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "ImageBox")
                    {
                        cpp++;
                        QLabel *lbl = cpObjects[cpp++].value<QLabel*>();
                        if(resList.count() == 6)
                        {
                            resList[4] = QString::number(lbl->x());
                            resList[5] = QString::number(lbl->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else if(cpObjects[cpp] == "PlotData")
                    {
                        cpp++;
                        QWidget *w = cpObjects[cpp++].value<QWidget*>();
                        if(resList.count() == 9)
                        {
                            resList[7] = QString::number(w->x());
                            resList[8] = QString::number(w->y());
                        }
                        streamOut << resList.join(",") + "\n";
                    }
                    else
                    {
                        cpp += 2;
                        streamOut << resList.join(",") + "\n";
                    }
                }
                else streamOut << resList.join(",") + "\n";
            }
        } while(!line.isNull());
        file.close();
        fileOut.close();
    }
}

void ControlPanel::slotGeneralHelp(void)
{
    help->SetTitle("MIPS Help");
    help->LoadHelpText(":/MIPShelp.txt");
    help->show();
}

void ControlPanel::slotMIPScommands(void)
{
    help->SetTitle("MIPS Commands");
    help->LoadHelpText(":/MIPScommands.txt");
    help->show();
}

void ControlPanel::slotScriptHelp(void)
{
    help->SetTitle("Script help");
    help->LoadHelpText(":/scripthelp.txt");
    help->show();
}

void ControlPanel::slotThisControlPanelHelp(void)
{
    help->SetTitle("This Control panel help");
    help->LoadHelpText(HelpFile);
    help->show();
}

void ControlPanel::slotLogStatusBarMessage(QString statusMess)
{
    if(statusMess == "") return;
    if(statusMess.isEmpty()) return;
    if(properties != NULL) properties->Log("CP StatusBar: " + statusMess);
}

// This menu option allows the user to define a data log file name and
// define the minimum time between samples. After the file is opened a header
// is written with the variable names and the time the logging started.
// The data file is CSV with wach record having a time stamp in seconds from
// the time the file was opened.
void ControlPanel::slotOpenLogFile(void)
{
    QFileDialog fileDialog;
    QMessageBox msgBox;
    bool ok;

    QString msg = "This option will open a data log file allowing you to define the ";
    msg += "minimum time between samples. The data file in CSV with a header holding ";
    msg += "the value names. Most control parameter values are saved.\n";
    msgBox.setText(msg);
    msgBox.setInformativeText("Are you sure you want to continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if(ret == QMessageBox::No) return;
    // Ask for file name
    QString dataFile = fileDialog.getSaveFileName(this, tr("Save data to log File"),"",tr("Log (*.log);;All files (*.*)"));
    if(dataFile == "") return;
    // Ask for time between samples
    QString res = QInputDialog::getText(this, "Log file sample period", "Enter the minimum time in seconds between samples.", QLineEdit::Normal,"10", &ok);
    // Initalize the logging parameters
    LogStartTime = 0;
    LogPeriod = res.toInt();
    if(LogPeriod <= 0) LogPeriod = 1;
    LogFile = dataFile;
    statusBar->showMessage("Data log file opened: " + dataFile);
    ControlPanel::setWindowTitle("Custom control panel, right click for options, data log file open: " + dataFile);
}

void ControlPanel::slotCloseLogFile(void)
{
    LogFile.clear();
    statusBar->showMessage("Data log file closed!");
    ControlPanel::setWindowTitle("Custom control panel, right click for options");
}

void ControlPanel::LogDataFile(void)
{
   QStringList vals;
   QString     header;
   QString     record;
   int         i;
   QDateTime   qt;
   static uint NextSampleTime;

   if(LogFile.isEmpty()) return;
   header.clear();
   record.clear();
   if(LogStartTime == 0)
   {
       LogStartTime = qt.currentDateTime().toMSecsSinceEpoch();
       NextSampleTime = LogStartTime;
       // Write the file header
       header = QDateTime().currentDateTime().toString() + "\n";
       // Build the CSV header record
       header += "Seconds";
       for(i=0;i<DCBchans.count();i++)
       {
           vals = DCBchans[i]->Report().split(",");
           header += "," + vals[0] + ".SP," + vals[0] + ".RP";
       }
       for(i=0;i<RFchans.count();i++)
       {
           vals = RFchans[i]->Report().split(",");
           header += "," + vals[0] + ".DRV," + vals[0] + ".FREQ," + vals[0] + ".RF+," + vals[0] + ".RF-," + vals[0] + ".PWR";
       }
       for(i=0;i<RFCchans.count();i++)
       {
           vals = RFCchans[i]->Report().split(",");
           header += "," + vals[0] + ".DRV," + vals[0] + ".STP," + vals[0] + ".FREQ," + vals[0] + ".RF+," + vals[0] + ".RF-," + vals[0] + ".PWR";
       }
       header += "\n";
   }
   if(qt.currentDateTime().toMSecsSinceEpoch() >= NextSampleTime)
   {
       while(NextSampleTime <= qt.currentDateTime().toSecsSinceEpoch()) NextSampleTime += LogPeriod;
       record = QString::number(qt.currentDateTime().toMSecsSinceEpoch() - LogStartTime);
       // Build the data string to write to the log file
       for(i=0;i<DCBchans.count();i++)
       {
           vals = DCBchans[i]->Report().split(",");
           record += "," + vals[1] + "," + vals[2];
       }
       for(i=0;i<RFchans.count();i++)
       {
           vals = RFchans[i]->Report().split(",");
           record += "," + vals[1] + "," + vals[2] + "," + vals[3] + "," + vals[4] + "," + vals[5];
       }
       for(i=0;i<RFCchans.count();i++)
       {
           vals = RFCchans[i]->Report().split(",");
           record += "," + vals[1] + "," + vals[2] + "," + vals[3] + "," + vals[4] + "," + vals[5] + "," + vals[6];
       }
       record += "\n";
   }
   if(!header.isEmpty() || !record.isEmpty())
   {
       // Open file for append and save log results
       QFile file(LogFile);
       if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
       {
           QTextStream stream(&file);
           if(!header.isEmpty()) stream << header;
           if(!record.isEmpty()) stream << record;
           file.close();
       }
   }
}

void ControlPanel::InitMIPSsystems(QString initFilename)
{
    // Open the file and send commands to MIPS systems.
    QFile file(initFilename);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            if(line.trimmed().startsWith("#")) continue;
            QStringList resList = line.split(",");
            Comms *cp =  FindCommPort(resList[0],Systems);
            if(cp!=NULL) cp->SendString(line.mid(line.indexOf(resList[0]) + resList[0].length() + 1) + "\n");
        } while(!line.isNull());
        file.close();
    }
}

// Returns a pointer to the comm port for the MIPS system defined my its name.
// Returns null if the MIPS system can't be found.
Comms* ControlPanel::FindCommPort(QString name, QList<Comms*> Systems)
{
   for(int i = 0; i < Systems.length(); i++) if(Systems.at(i)->MIPSname == name) return(Systems.at(i));
   return NULL;
}

void ControlPanel::Update(void)
{
   QMessageBox msgBox;
   int i,j,k;

   // If the watch dog is enabled send a \n to MIPS to keep them
   // alive.
   if(SerialWatchDog > 0)
   {
       for(i=0;i<Systems.count();i++)
       {
          Systems[i]->SendString("\n");
       }
   }
   if(tcp != NULL) if(tcp->bytesAvailable() > 0) tcp->readData();
   QApplication::processEvents();
   if(scriptconsole!=NULL) scriptconsole->UpdateStatus();
   if(UpdateStop) return;
   for(i=0;i<TC.count();i++) if(TC[i]->Downloading) return;
   if(StartMIPScomms)
   {
       mc = new MIPScomms(0,Systems);
       mc->show();
       UpdateStop = true;
       connect(mc, SIGNAL(DialogClosed()), this, SLOT(CloseMIPScomms()));
       StartMIPScomms = false;
       UpdateStop = true;
       return;
   }
   if(UpdateHoldOff > 0)
   {
       UpdateHoldOff--;
       return;
   }
   if(ShutdownFlag)
   {
       ShutdownFlag = false;
       SystemIsShutdown = true;
       UpdateHoldOff = 1000;
       // Make sure all MIPS systems are in local mode
       for(i=0;i<Systems.count();i++)    Systems[i]->SendString("SMOD,LOC\n");
       msDelay(100);
       for(i=0;i<Systems.count();i++)    Systems[i]->rb.clear();
       for(i=0;i<ESIchans.count();i++)   ESIchans[i]->Shutdown();
       for(i=0;i<DCBenables.count();i++) DCBenables[i]->Shutdown();
       for(i=0;i<RFchans.count();i++)    RFchans[i]->Shutdown();
       for(i=0;i<RFCchans.count();i++)   RFCchans[i]->Shutdown();
       for(i=0;i<Ccontrols.count();i++)
       {
           Ccontrols[i]->Shutdown();
           if(Ccontrols[i]->Title.contains("ENABLE",Qt::CaseInsensitive))
           {
               if(Ccontrols[i]->p->objectName() != "") Ccontrols[i]->SetValues(Ccontrols[i]->p->objectName() + "." + Ccontrols[i]->Title + ",FALSE");
               else Ccontrols[i]->SetValues(Ccontrols[i]->Title + ",FALSE");
               //qDebug() << Ccontrols[i]->Title + "=FALSE";
           }
       }
       for(i=0;i<rfa.count();i++)        rfa[i]->Shutdown();
       for(i=0;i<ARBchans.count();i++)   ARBchans[i]->Shutdown();
       if(statusBar != NULL) statusBar->showMessage("System shutdown, " + QDateTime().currentDateTime().toString());
       if(properties != NULL) properties->Log("System Shutdown");
       UpdateHoldOff = 1;
       return;
   }
   if(RestoreFlag)
   {
       RestoreFlag = false;
       SystemIsShutdown = false;
       UpdateHoldOff = 1000;
       for(i=0;i<ESIchans.count();i++)   ESIchans[i]->Restore();
       msDelay(100);
       for(i=0;i<DCBenables.count();i++) {DCBenables[i]->Restore(); msDelay(250);}
       for(i=0;i<RFchans.count();i++)    {RFchans[i]->Restore(); msDelay(250);}
       for(i=0;i<RFCchans.count();i++)   {RFCchans[i]->Restore(); msDelay(250);}
       for(i=0;i<Ccontrols.count();i++)  Ccontrols[i]->Restore();
       for(i=0;i<rfa.count();i++)        rfa[i]->Restore();
       for(i=0;i<ARBchans.count();i++)   ARBchans[i]->Restore();
       if(statusBar != NULL) statusBar->showMessage("System enabled, " + QDateTime().currentDateTime().toString());
       if(properties != NULL) properties->Log("System Restored");
       UpdateHoldOff = 1;
       return;
   }
   // Look for any broken connections and warn the user.
   // Also allow user to try and reconnect.
   QString res;
   res.clear();
   for(i=0;i<Systems.count();i++)
   {
       QApplication::processEvents();  // Not sure why this is needed?
       if(!Systems[i]->isConnected())
       {
           res += Systems[i]->MIPSname + ",";
       }
   }
   if(!res.isEmpty())
   {
       int ret;
       bool AutoR = false;
       if(properties != NULL) if(properties->AutoRestore) AutoR = true;
       if(!AutoR)
       {
           QString msg = "Connection(s) have been lost to " + res;
           msg += "would you like to try and reestablish the connection(s)";
           msg += "or exit this control panel.";
           msgBox.setText(msg);
           msgBox.setInformativeText("Select Yes to reestablish connection(s)?");
           msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
           msgBox.setDefaultButton(QMessageBox::Yes);
           ret = msgBox.exec();
       }
       else
       {
           if(statusBar != NULL) statusBar->showMessage("Attempting to reestablish connection(s).",2000);
           msDelay(2000);
           ret = QMessageBox::Yes;
       }
       if(ret == QMessageBox::Yes)
       {
           for(i=0;i<Systems.count();i++) if(!Systems[i]->isConnected()) Systems[i]->reopenPort();
       }
       else reject();
   }

   for(i=0;i<ScripButtons.count();i++) {ScripButtons[i]->Update(); if(ProcessEvents) QApplication::processEvents();}
   //for(i=0;i<ScripButtons.count();i++) {if(ScripButtons[i]->CallOnStart || ScripButtons[i]->CallOnUpdate) ScripButtons[i]->Update();}
   // For each MIPS system present if there are RF channels for the selected
   // MIPS system then read all values using the read all commands to speed up the process.
   for(i=0;i<Systems.count();i++)
   {
       for(j=0;j<RFchans.count();j++) if(RFchans[j]->comms == Systems[i])
       {
           // Read all the RF parameters
           QString RFallRes = Systems[i]->SendMess("GRFALL\n");
           if(ProcessEvents) QApplication::processEvents();
           QStringList RFallResList = RFallRes.split(",");
           if(RFallResList.count() < 1)
           {
               // Here with group read error so process one at a time
               for(k=0;k<RFchans.count();k++) if(RFchans[k]->comms == Systems[i]) { RFchans[k]->Update(); /*QApplication::processEvents();*/ }
           }
           else
           {
               // build strings and update all channels that use this comm port
               for(k=0;k<RFchans.count();k++) if(RFchans[k]->comms == Systems[i])
               {
                   if(RFallResList.count() < (RFchans[k]->Channel * 4)) RFchans[k]->Update();
                   else RFchans[k]->Update(RFallResList[(RFchans[k]->Channel - 1) * 4 + 0] + "," + \
                                           RFallResList[(RFchans[k]->Channel - 1) * 4 + 1] + "," + \
                                           RFallResList[(RFchans[k]->Channel - 1) * 4 + 2] + "," + \
                                           RFallResList[(RFchans[k]->Channel - 1) * 4 + 3]);
                   if(ProcessEvents) QApplication::processEvents();
               }
           }
           break;
       }
   }
   for(i=0;i<Systems.count();i++)
   {
       for(j=0;j<RFCchans.count();j++) if(RFCchans[j]->comms == Systems[i])
       {
           // Read all the RF parameters
           QString RFallRes = Systems[i]->SendMess("GRFALL\n");
           if(ProcessEvents) QApplication::processEvents();
           QStringList RFallResList = RFallRes.split(",");
           if(RFallResList.count() < 1)
           {
               // Here with group read error so process one at a time
               for(k=0;k<RFCchans.count();k++) if(RFCchans[k]->comms == Systems[i]) { RFCchans[k]->Update(); /*QApplication::processEvents();*/ }
           }
           else
           {
               // build strings and update all channels that use this comm port
               for(k=0;k<RFCchans.count();k++) if(RFCchans[k]->comms == Systems[i])
               {
                   if(RFallResList.count() < (RFCchans[k]->Channel * 4)) RFCchans[k]->Update();
                   else RFCchans[k]->Update(RFallResList[(RFCchans[k]->Channel - 1) * 4 + 0] + "," + \
                                           RFallResList[(RFCchans[k]->Channel - 1) * 4 + 1] + "," + \
                                           RFallResList[(RFCchans[k]->Channel - 1) * 4 + 2] + "," + \
                                           RFallResList[(RFCchans[k]->Channel - 1) * 4 + 3]);
                   if(ProcessEvents) QApplication::processEvents();
               }
           }
           break;
       }
   }
   for(i=0;i<ADCchans.count();i++)    {ADCchans[i]->Update(); if(ProcessEvents) QApplication::processEvents();}
   for(i=0;i<DACchans.count();i++)    {DACchans[i]->Update(); if(ProcessEvents) QApplication::processEvents();}
   // For each MIPS system present if there are DCBchannels for the selected
   // MIPS system then read all setpoints and values using the read all
   // commands to speed up the process.
   for(i=0;i<Systems.count();i++)
   {
       for(j=0;j<DCBchans.count();j++) if(DCBchans[j]->comms == Systems[i])
       {
           // Read all the setpoints and readbacks and parse the strings
           QString VspRes = Systems[i]->SendMess("GDCBALL\n");
           if(ProcessEvents) QApplication::processEvents();
           QStringList VspResList = VspRes.split(",");
           QString VrbRes = Systems[i]->SendMess("GDCBALLV\n");
           if(ProcessEvents) QApplication::processEvents();
           QStringList VrbResList = VrbRes.split(",");
           if((VspResList.count() != VrbResList.count()) || (VspResList.count() < 1))
           {
               // Here with group read error so process one at a time
               for(k=0;k<DCBchans.count();k++) if(DCBchans[k]->comms == Systems[i]) {DCBchans[k]->Update(); /*QApplication::processEvents();*/}
           }
           else
           {
               // build strings and update all channels that use this comm port
               for(k=0;k<DCBchans.count();k++) if(DCBchans[k]->comms == Systems[i])
               {
                   if(VspResList.count() < (DCBchans[k]->Channel)) DCBchans[k]->Update();
                   else DCBchans[k]->Update(VspResList[DCBchans[k]->Channel - 1] + "," + VrbResList[DCBchans[k]->Channel - 1]);
               }
           }
           break;  //??
       }
   }
   for(i=0;i<DCBoffsets.count();i++)  {DCBoffsets[i]->Update(); if(ProcessEvents) QApplication::processEvents();}
   for(i=0;i<DCBenables.count();i++)  {DCBenables[i]->Update(); if(ProcessEvents) QApplication::processEvents();}
   if(TC.count() > 0)
   {
       for(i=0;i<=TC.count();i++)
       {
           if(i==TC.count())
           {
              for(j=0;j<DIOchannels.count();j++) {DIOchannels[j]->Update(); if(ProcessEvents) QApplication::processEvents();}
              break;
           }
           if(TC[i]->TG->isTableMode()) break;
       }
   }
   else for(i=0;i<DIOchannels.count();i++) {DIOchannels[i]->Update(); if(ProcessEvents) QApplication::processEvents();}
   for(i=0;i<ESIchans.count();i++)         {ESIchans[i]->Update(); if(ProcessEvents) QApplication::processEvents();}
   for(i=0;i<ARBchans.count();i++)         {ARBchans[i]->Update(); if(ProcessEvents) QApplication::processEvents();}
   for(i=0;i<Ccontrols.count();i++)        {Ccontrols[i]->Update();if(ProcessEvents) QApplication::processEvents();}
   for(i=0;i<rfa.count();i++)              {rfa[i]->Update();      if(ProcessEvents) QApplication::processEvents();}
   for(i=0;i<devices.count();i++)          {devices[i]->Update();}
   for(i=0;i<Cpanels.count();i++)          Cpanels[i]->Update();
   for(i=0;i<TC.count();i++)               for(j=0;j<TC[i]->TG->EC.count();j++) TC[i]->TG->EC[j]->Update();
   for(i=0;i<comp.count();i++)             {comp[i]->Update();     if(ProcessEvents) QApplication::processEvents();}
   for(i=0;i<ModChans.count();i++)         {ModChans[i]->Update(); if(ProcessEvents) QApplication::processEvents();}
   RequestUpdate = false;
   LogDataFile();
}

// Save methode function
QString ControlPanel::Save(QString Filename)
{
    //QString res;
    ControlPanel *cp;

    #ifdef Q_OS_MAC
    if(Filename.startsWith("~")) Filename = QDir::homePath() + "/" + Filename.mid(2);
    #endif
    if(Filename == "") return "No file defined!";
    UpdateHoldOff = 1000;
    if(properties != NULL) properties->Log("Save method file: " + Filename);
    QFile file(Filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // We're going to streaming text to the file
        QTextStream stream(&file);
        QDateTime dateTime = QDateTime::currentDateTime();
        stream << "# Control panel settings, " + dateTime.toString() + "\n";
        stream << "# " + Version + "\n";
        stream << "# Control panel file, " + ControlPanelFile + "\n";
        stream << MIPSnames;
        // Add uptime if MIPS systems are connected
        for(int i=0;i<Systems.count();i++)
        {
           QString res = Systems[i]->SendMess("UPTIME\n");
           if(res!="" && !res.contains("?")) stream << "# " + Systems[i]->MIPSname + ": " + res + "\n";
        }
        if(!CommentText.isEmpty())
        {
            stream << "CommentText\n";
            stream << CommentText;
            stream << "\n";
            stream << "EndCommentText\n";
            CommentText.clear();
        }
        for(int cpnum = -1; cpnum < Cpanels.count(); cpnum++)
        {
            if(cpnum == -1) cp=this;
            else
            {
                cp = Cpanels[cpnum]->cp;
                stream << "Control panel," + Cpanels[cpnum]->Title + "\n";
            }
            if(cp->DCBoffsets.count() > 0)
            {
                stream << "DCB offsets," + QString::number(cp->DCBoffsets.count()) + "\n";
                for(int i=0; i<cp->DCBoffsets.count(); i++) if(cp->DCBoffsets[i] != NULL) stream << cp->DCBoffsets[i]->Report() + "\n";
            }
            if(cp->DCBgroups != NULL)
            {
                stream << "DC bias groups\n";
                stream << cp->DCBgroups->Report() + "\n";
            }
            if(cp->RFchans.count() > 0)
            {
                stream << "RF channels," + QString::number(cp->RFchans.count()) + "\n";
                for(int i=0;i<cp->RFchans.count();i++) stream << cp->RFchans[i]->Report() + "\n";
            }
            if(cp->RFCchans.count() > 0)
            {
                stream << "RFC channels," + QString::number(cp->RFCchans.count()) + "\n";
                for(int i=0;i<cp->RFCchans.count();i++) stream << cp->RFCchans[i]->Report() + "\n";
            }
            if(cp->DCBchans.count() > 0)
            {
                stream << "DCB channels," + QString::number(cp->DCBchans.count()) + "\n";
                for(int i=0; i<cp->DCBchans.count(); i++) stream << cp->DCBchans[i]->Report() + "\n";
            }
            if(cp->DACchans.count() > 0)
            {
                stream << "DAC channels," + QString::number(cp->DACchans.count()) + "\n";
                for(int i=0; i<cp->DACchans.count(); i++) stream << cp->DACchans[i]->Report() + "\n";
            }
            if(cp->DCBenables.count() > 0)
            {
                stream << "DCB enables," + QString::number(cp->DCBenables.count()) + "\n";
                for(int i=0; i<cp->DCBenables.count(); i++) stream << cp->DCBenables[i]->Report() + "\n";
            }
            if(cp->DIOchannels.count() > 0)
            {
                stream << "DIO channels," + QString::number(cp->DIOchannels.count()) + "\n";
                for(int i=0; i<cp->DIOchannels.count(); i++) stream << cp->DIOchannels[i]->Report() + "\n";
            }
            if(cp->ESIchans.count() > 0)
            {
                stream << "ESI channels," + QString::number(cp->ESIchans.count()) + "\n";
                for(int i=0; i<cp->ESIchans.count(); i++) stream << cp->ESIchans[i]->Report() + "\n";
            }
            if(cp->ARBchans.count() > 0)
            {
                stream << "ARB channels," + QString::number(cp->ARBchans.count()) + "\n";
                for(int i=0; i<cp->ARBchans.count(); i++) stream << cp->ARBchans[i]->Report() + "\n";
            }
            if(cp->Ccontrols.count() > 0)
            {
                stream << "Custom control channels," + QString::number(cp->Ccontrols.count()) + "\n";
                for(int i=0; i<cp->Ccontrols.count(); i++) stream << cp->Ccontrols[i]->Report() + "\n";
            }
            if(cp->comp.count() > 0)
            {
                stream << "Compression parameters\n";
                for(int i=0; i<cp->comp.count(); i++) stream << cp->comp[i]->Report() + "\n";
                stream << "CompressionEnd\n";
            }
            if(cp->rfa.count() > 0)
            {
                stream << "RFamp channels," + QString::number(cp->rfa.count()) + "\n";
                for(int i=0; i<cp->rfa.count(); i++) if(cp->rfa[i] != NULL) stream << cp->rfa[i]->Report() + "\n";
                stream << "RFampEnd\n";
            }
            if(cp->devices.count() > 0)
            {
                stream << "External devices," + QString::number(cp->devices.count()) + "\n";
                for(int i=0; i<cp->devices.count(); i++) if(cp->devices[i] != NULL) stream << cp->devices[i]->Report() + "\n";
                stream << "ExternalDevicesEnd\n";
            }
            if(cp->ModChans.count() > 0)
            {
                stream << "ModBus channels," + QString::number(cp->ModChans.count()) + "\n";
                for(int i=0; i<cp->ModChans.count(); i++) if(cp->ModChans[i] != NULL) stream << cp->ModChans[i]->Report() + "\n";
                stream << "ModBusChannelsEnd\n";
            }
            if(cp->plotData.count() > 0)
            {
                stream << "Plot parameters\n";
                for(int i=0; i<cp->plotData.count(); i++) stream << cp->plotData[i]->Report() + "\n";
                stream << "PlotParametersEnd\n";
            }
            if(cp->TC.count() > 0)
            {
                stream << "Timing control parameters\n";
                for(int i=0; i<cp->TC.count(); i++)
                {
                    stream << cp->TC[i]->TG->Report() + "\n";
                    for(int j=0; j<cp->TC[i]->TG->EC.count();j++) stream << cp->TC[i]->TG->EC[j]->Report() + "\n";
                }
                stream << "TCparametersEnd\n";
            }
        }
        file.close();
        UpdateHoldOff = 1;
        return "Settings saved to " + Filename;
    }
    return "Can't open file!";
}

QString ControlPanel::Load(QString Filename)
{
    QStringList resList;
    ControlPanel *cp;

    cp = this;
    // Test is Filename ends in .settings, if not add .settings
    if(!Filename.endsWith(".SETTINGS",Qt::CaseInsensitive)) Filename += ".settings";
    if(properties != NULL) QDir().setCurrent(properties->MethodesPath);
    #ifdef Q_OS_MAC
    if(Filename.startsWith("~")) Filename = QDir::homePath() + "/" + Filename.mid(2);
    #endif
    if(Filename == "") return "No file defined!";
    UpdateHoldOff = 1000;
    QFile file(Filename);
    if(properties != NULL) properties->Log("Load method file: " + Filename);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        CommentText.clear();
        // We're going to streaming the file
        // to a QString
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            resList = line.split(",");
            if(line=="CommentText")
            {
                while(true)
                {
                    line = stream.readLine();
                    if(line=="EndCommentText") break;
                    if(line.isNull()) break;
                    CommentText += line + "\n";
                }
                continue;
            }
            if(resList.count() >= 2)
            {
                if(resList[0] == "Control panel")
                {
                    // See if we can find this control panel name and then point to it,
                    // quit is we can't find it...
                    cp = NULL;
                    for(int j=0;j<Cpanels.count();j++)
                    {
                        if(Cpanels[j]->Title == resList[1]) cp = Cpanels[j]->cp;
                    }
                    if(cp == NULL) break;
                }
                if(resList[0] == "RF channels") for(int i=0;i<resList[1].toInt();i++)
                {
                     line = stream.readLine();
                     if(line.isNull()) break;
                     for(int j=0;j<cp->RFchans.count();j++) if(cp->RFchans[j]->SetValues(line)) break;
                }
                if(resList[0] == "RFC channels") for(int i=0;i<resList[1].toInt();i++)
                {
                     line = stream.readLine();
                     if(line.isNull()) break;
                     for(int j=0;j<cp->RFCchans.count();j++) if(cp->RFCchans[j]->SetValues(line)) break;
                }
                if(resList[0] == "DCB channels") for(int i=0;i<resList[1].toInt();i++)
                {
                     line = stream.readLine();
                     if(line.isNull()) break;
                     //if(i >= DCBchans.count()) break;
                     //if(DCBchans[i]->SetValues(line)) continue;
                     for(int j=0;j<cp->DCBchans.count();j++) if(cp->DCBchans[j]->SetValues(line)) break;
                }
                if(resList[0] == "DAC channels") for(int i=0;i<resList[1].toInt();i++)
                {
                     line = stream.readLine();
                     if(line.isNull()) break;
                     for(int j=0;j<cp->DACchans.count();j++) if(cp->DACchans[j]->SetValues(line)) break;
                }
                if(resList[0] == "DCB offsets") for(int i=0;i<resList[1].toInt();i++)
                {
                     line = stream.readLine();
                     if(line.isNull()) break;
                     for(int j=0;j<cp->DCBoffsets.count();j++) if(cp->DCBoffsets[j]->SetValues(line)) break;
                }
                if(resList[0] == "DCB enables") for(int i=0;i<resList[1].toInt();i++)
                {
                     line = stream.readLine();
                     if(line.isNull()) break;
                     for(int j=0;j<cp->DCBenables.count();j++) if(cp->DCBenables[j]->SetValues(line)) break;
                }
                if(resList[0] == "DIO channels") for(int i=0;i<resList[1].toInt();i++)
                {
                     line = stream.readLine();
                     if(line.isNull()) break;
                     for(int j=0;j<cp->DIOchannels.count();j++) if(cp->DIOchannels[j]->SetValues(line)) break;
                }
                if(resList[0] == "ESI channels") for(int i=0;i<resList[1].toInt();i++)
                {
                     line = stream.readLine();
                     if(line.isNull()) break;
                     for(int j=0;j<cp->ESIchans.count();j++) if(cp->ESIchans[j]->SetValues(line)) break;
                }
                if(resList[0] == "ARB channels") for(int i=0;i<resList[1].toInt();i++)
                {
                     line = stream.readLine();
                     if(line.isNull()) break;
                     for(int j=0;j<cp->ARBchans.count();j++) if(cp->ARBchans[j]->SetValues(line)) break;
                }
                if(resList[0] == "Custom control channels") for(int i=0;i<resList[1].toInt();i++)
                {
                     line = stream.readLine();
                     if(line.isNull()) break;
                     for(int j=0;j<cp->Ccontrols.count();j++) if(cp->Ccontrols[j]->SetValues(line)) break;
                }
                if(resList[0] == "RFamp channels") while(true)
                {
                     line = stream.readLine();
                     if(line.isNull()) break;
                     if(line.contains("RFampEnd")) break;
                     for(int j=0;j<cp->rfa.count();j++) if(cp->rfa[j]->SetValues(line)) break;
                }
                if(resList[0] == "ModBus channels") while(true)
                {
                     line = stream.readLine();
                     if(line.isNull()) break;
                     if(line.contains("ModBusChannelsEnd")) break;
                     for(int j=0;j<cp->ModChans.count();j++) if(cp->ModChans[j]->SetValues(line)) break;
                }
            }
            else if(resList.count() == 1)
            {
                if(resList[0] == "DC bias groups")
                {
                    line = stream.readLine();
                    if(line.isNull()) break;
                    if(cp->DCBgroups != NULL) cp->DCBgroups->SetValues(line);
                }
                if(resList[0] == "Plot parameters") while(true)
                {
                    line = stream.readLine();
                    if(line.isNull()) break;
                    if(line.contains("PlotParametersEnd")) break;
                    for(int j=0;j<cp->plotData.count();j++) if(cp->plotData[j]->SetValues(line)) break;
                }
                if(resList[0] == "Timing control parameters")
                {
                    for(int i=0;i<cp->TC.count();i++) cp->TC[i]->TG->Events.clear();
                    while(true)
                    {
                        line = stream.readLine();
                        if(line.isNull()) break;
                        if(line.contains("TCparametersEnd")) break;
                        for(int i=0;i<cp->TC.count();i++)
                        {
                            cp->TC[i]->TG->SetValues(line);
                            for(int j=0;j<cp->TC[i]->TG->EC.count();j++) cp->TC[i]->TG->EC[j]->SetValues(line);
                        }
                    }
                }
                if(resList[0] == "Compression parameters")
                {
                    while(true)
                    {
                        line = stream.readLine();
                        if(line.isNull()) break;
                        if(line.contains("CompressionEnd")) break;
                        for(int i=0;i<cp->comp.count();i++) cp->comp[i]->SetValues(line);
                    }
                }
            }
        } while(!line.isNull());
        UpdateHoldOff = 1;
        file.close();
        return "Settings loaded from " + Filename;
    }
    return "Can't open file!";
}

// Called after data collection, save method to the filepath passed
void ControlPanel::slotDataAcquired(QString filepath)
{
    if(filepath != "")
    {
        if(properties != NULL) properties->Log("Data acquired: " + filepath);
        Save(filepath + "/Method.settings");
        // Send UDP message to allow reader to open the data file
        QString mess = "Load,"+filepath+"/U1084A.data";
        udpSocket->writeDatagram(mess.toLocal8Bit(),QHostAddress("127.0.0.1"), 7755);
    }
}

void ControlPanel::slotDataFileDefined(QString filepath)
{
    QString mess = "Data file defined,"+filepath;
    udpSocket->writeDatagram(mess.toLocal8Bit(),QHostAddress("127.0.0.1"), 7755);
}

// System shutdown
//   Disable ESI
//   Disable all DCbias modules
//   Disable all RF drives
void ControlPanel::pbSD(void)
{
    ShutdownFlag = true;
}

// System enable
void ControlPanel::pbSE(void)
{
    RestoreFlag = true;
}

void ControlPanel::pbSave(void)
{
    QFileDialog fileDialog;

    if(properties != NULL) fileDialog.setDirectory(properties->MethodesPath);
    QString fileName = fileDialog.getSaveFileName(this, tr("Save to Settings File"),"",tr("Settings (*.settings);;All files (*.*)"));
    if(fileName == "") return;
    statusBar->showMessage(Save(fileName), 5000);
}

void ControlPanel::pbLoad(void)
{
    QFileDialog fileDialog;

    if(properties != NULL) fileDialog.setDirectory(properties->MethodesPath);
    QString fileName = fileDialog.getOpenFileName(this, tr("Load Settings from File"),"",tr("Settings (*.settings);;All files (*.*)"));
    if(fileName == "") return;
    statusBar->showMessage(Load(fileName), 5000);
    if(!CommentText.isEmpty())
    {
        QMessageBox msgBox;

        QApplication::processEvents();
        msgBox.setText(CommentText);
        msgBox.setInformativeText("Setting file description");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
}

void ControlPanel::CloseMIPScomms(void)
{
    UpdateStop = false;
}

void ControlPanel::pbMIPScomms(void)
{
   StartMIPScomms = true;
}

void ControlPanel::pbARBcompressor(void)
{
    QObject*    obj = sender();

    for(int i=0; i<ARBcompressorButton.count();i++)
    {
        if((QPushButton *)obj == ARBcompressorButton[i])
        {
            ARBcompressorButton[i]->setDown(false);
            comp[i]->show();
            comp[i]->raise();
        }
    }
}

void ControlPanel::scriptShutdown(void)
{
    ShutdownFlag = true;
}

void ControlPanel::DCBgroupDisable(void)
{
    for(int i=0;i<DCBchans.count();i++) DCBchans[i]->LinkEnable = false;
}

DCBchannel * ControlPanel::FindDCBchannel(QString name)
{
    for(int i=0;i<DCBchans.count();i++)
    {
        //qDebug() << DCBchans[i]->Title.toUpper();
        if(DCBchans[i]->Title.toUpper() == name.toUpper()) return DCBchans[i];
    }
    return NULL;
}

void ControlPanel::DCBgroupEnable(void)
{
    DCBchannel            *dcb;
    QList<DCBchannel*>    DCBs;

    for(int i=0;i<DCBgroups->comboGroups->count();i++)
    {
        QStringList resList = DCBgroups->comboGroups->itemText(i).split(",");
        DCBs.clear();
        for(int j=0;j<resList.count();j++)
        {
            if((dcb=FindDCBchannel(resList[j])) != NULL)
            {
                DCBs.append(dcb);
            }
        }
        for(int j=0;j<DCBchans.count();j++)
        {
            if(DCBs.contains(DCBchans[j]))
            {
               DCBchans[j]->DCBs = DCBs;
               DCBchans[j]->LinkEnable = true;
            }
        }
    }
}

void ControlPanel::pbScript(void)
{
    if(scriptconsole == NULL) return;
    scriptconsole->show();
    scriptconsole->raise();
    scriptconsole->repaint();
}

QString ControlPanel::MakePathNameUnique(QString path)
{
    return MakePathUnique(path);
}

// Generate a unique file name using the properties path
// and base filename. Return empty string if we cannot generate a
// unique name.
QString ControlPanel::GenerateUniqueFileName(void)
{
    QString snum;
    QString fname = properties->DataFilePath;
    if((fname.right(1) != "/") && (fname.right(1) != "\\"))
    {
        #ifdef Q_OS_MAC
            fname += "/";
        #else
            fname += "\\";
        #endif
    }
    fname += properties->FileName;
    // Test if this file exists
    if(QFileInfo::exists(fname))
    {
        // Append a sequence number (-xxxx) and advance sequnce number until
        // a unique number is found
        for(int i=0;i<10000;i++)
        {
            snum = snum.asprintf("-%04d",i);
            if(!QFileInfo::exists(fname+snum)) return(fname+snum);
        }
        return("");
    }
    return fname;
}

// The following functions are for the scripting system
QString ControlPanel::GetLine(QString MIPSname)
{
   QApplication::processEvents();
   Comms *cp =  FindCommPort(MIPSname,Systems);
   if(cp==NULL) return "";
   cp->waitforline(500);
   return cp->getline();
}

bool ControlPanel::SendCommand(QString MIPSname, QString message)
{
   QApplication::processEvents();
   Comms *cp =  FindCommPort(MIPSname,Systems);
   if(cp==NULL) return false;
   return cp->SendCommand(message);
}

bool ControlPanel::SendString(QString MIPSname, QString message)
{
   QApplication::processEvents();
   Comms *cp =  FindCommPort(MIPSname,Systems);
   if(cp==NULL) return false;
   return cp->SendString(message);
}

QString ControlPanel::SendMess(QString MIPSname, QString message)
{
    QApplication::processEvents();
    Comms *cp =  FindCommPort(MIPSname,Systems);
    if(cp==NULL) return "MIPS not found!";
    return cp->SendMess(message);
}

void ControlPanel::SystemEnable(void)
{
    QApplication::processEvents();
    RestoreFlag = true;
}

void ControlPanel::SystemShutdown(void)
{
    QApplication::processEvents();
    ShutdownFlag = true;
}

bool ControlPanel::isShutDown(void)
{
    return(SystemIsShutdown);
}

void ControlPanel::Acquire(QString filePath)
{
   QApplication::processEvents();
   filePath.replace("\\","/");
   if(isAcquiring())
   {
       // If here the system is in the acquire state so create the
       // empty folder and log error
       if(properties->AutoFileName) filePath = MakePathUnique(filePath);
       QDir().mkdir(filePath);
       statusBar->showMessage("Already acquiring, request ignored but attempted folder creation!");
       return;
   }
   if(TC.count() > 0) TC.last()->AcquireData(filePath);
}

bool ControlPanel::isAcquiring(void)
{
  QApplication::processEvents();
  if(TC.count() > 0) return(TC.last()->AD->Acquiring);
  return(false);
}

void ControlPanel::DismissAcquire(void)
{
  QApplication::processEvents();
  if(TC.count() > 0)  TC.last()->AD->Dismiss();
  QApplication::processEvents();
  QApplication::processEvents();
}

void ControlPanel::msDelay(int ms)
{
    QElapsedTimer timer;

    timer.start();
    while(timer.elapsed() < ms) QApplication::processEvents();
}

void ControlPanel::statusMessage(QString message)
{
    QApplication::processEvents();
    statusBar->showMessage(message);
    QApplication::processEvents();
}

void ControlPanel::popupMessage(QString message)
{
    QMessageBox msgBox;

    QApplication::processEvents();
    msgBox.setText(message);
    msgBox.setInformativeText("");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

bool ControlPanel::popupYesNoMessage(QString message)
{
    QMessageBox msgBox;

    QApplication::processEvents();
    msgBox.setText(message);
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if(ret == QMessageBox::No) return false;
    return true;
}

QString ControlPanel::popupUserInput(QString title, QString message)
{
    bool ok;

    QApplication::processEvents();
    QString text = QInputDialog::getText(this, title, message, QLineEdit::Normal,QString(), &ok);
    if(!ok) text="";
    QApplication::processEvents();
    return text;
}

bool ControlPanel::UpdateHalted(bool stop)
{
    UpdateStop = stop;
    ProcessEvents = !stop;
    return UpdateStop;
}

void ControlPanel::WaitForUpdate(void)
{
    RequestUpdate = true;
    while(RequestUpdate) QApplication::processEvents();
}

QString ControlPanel::tcpSocket(QString cmd, QString arg1, QString arg2)
{
    static QTcpSocket *socket = new QTcpSocket(this);

    if(cmd == "Connect")
    {
        socket->connectToHost(arg1, arg2.toInt());
        if(socket->waitForConnected(3000))
        {
            return "Connected!";
        }
        else
        {
            return "Not connected!";
        }
    }
    if(cmd == "Write")
    {
        if(socket->isOpen())
        {
            socket->flush();
            socket->write(arg1.toLocal8Bit());
            socket->waitForBytesWritten();
        }
        return "Not connected!";
    }
    if(cmd == "Read")
    {
        if(socket->isOpen())
        {
            char res[80];
            for(int i=0;i<80;i++) res[i]=0;
            socket->read(res,80);
            return res;
        }
        return "Not connected!";
    }
    if(cmd == "Close")
    {
       if(socket->isOpen()) socket->close();
       return "Closed";
    }
    return "";
}

// This function is called with a command in the TCP server buffer.
// This function will process this command.
void ControlPanel::tcpCommand(void)
{
    QString cmd = tcp->readLine();
    tcp->sendMessage(Command(cmd));
    if(tcp->bytesAvailable() > 0) tcp->readData();
}

// This function is called by the TCPserver code to process commands and
// also by the script engine.
QString ControlPanel::Command(QString cmd)
{
   int     i,j;
   QStringList resList;
   QString res;

   res.clear();
   // Process global commands first.
   // Load,Save,Shutdown,Restore
   if(cmd.toUpper() == "SHUTDOWN")
   {
       ShutdownFlag = true;
       if(SD != NULL) SD->SetState(true);
       return res;
   }
   if(cmd.toUpper() == "RESTORE")
   {
       RestoreFlag  = true;
       if(SD != NULL) SD->SetState(false);
       return res;
   }
   if(cmd.startsWith("LOAD",Qt::CaseInsensitive))
   {
       resList = cmd.split(",");
       if(resList[0].trimmed().toUpper() == "LOAD")
       {
            if(resList.count()==2) Load(findFile(resList[1],QFileInfo(ControlPanelFile).canonicalPath()));
            return res;
       }
   }
   if(cmd.startsWith("SAVE",Qt::CaseInsensitive))
   {
       resList = cmd.split(",");
       if(resList[0].trimmed().toUpper() == "SAVE")
       {
            if(resList.count()==2) Save(resList[1]);
            return res;
       }
   }
   // Process commands that will allow communications with the MIPS hardware directly
   if(cmd.trimmed().toUpper().startsWith("SENDMESSAGE"))
   {
       resList = cmd.split(",");
       if(resList.count() > 2)
       {
           QString arg;
           arg.clear();
           for(int i=2;i<resList.count();i++)
           {
               if(i>2) arg += ",";
               arg += resList[i];
           }
           return(SendMess(resList[1],arg + "\n"));
       }
       else return("?\n");
   }
   if(cmd.trimmed().toUpper().startsWith("SENDCOMMAND"))
   {
       resList = cmd.split(",");
       if(resList.count() > 2)
       {
           QString arg;
           arg.clear();
           for(int i=2;i<resList.count();i++)
           {
               if(i>2) arg += ",";
               arg += resList[i];
           }
           if(SendCommand(resList[1],arg + "\n")) return("\n");
           else return("?\n");
       }
       else return("?\n");
   }
   // Send the command string to all the controls for someone to process!
   if(sl != NULL) if((res = sl->ProcessCommand(cmd)) != "?") return(res + "\n");
   if(tm != NULL) if((res = tm->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<DACchans.count();i++)    if((res = DACchans[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<ADCchans.count();i++)    if((res = ADCchans[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<DCBchans.count();i++)    if((res = DCBchans[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<RFchans.count();i++)     if((res = RFchans[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<RFCchans.count();i++)    if((res = RFCchans[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<DCBoffsets.count();i++)  if((res = DCBoffsets[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<DCBenables.count();i++)  if((res = DCBenables[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<DIOchannels.count();i++) if((res = DIOchannels[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<ESIchans.count();i++)    if((res = ESIchans[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<ARBchans.count();i++)    if((res = ARBchans[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<Ccontrols.count();i++)   if((res = Ccontrols[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<devices.count();i++)     if((res = devices[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<ScripButtons.count();i++) if((res = ScripButtons[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<Cpanels.count();i++)      if((res = Cpanels[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<CPbuttons.count();i++)    if((res = CPbuttons[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<ModChans.count();i++)     if((res = ModChans[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<plotData.count();i++)     if((res = plotData[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<TC.count();i++)
   {
       if((res = TC[i]->TG->ProcessCommand(cmd)) != "?") return(res + "\n");
       for(j=0;j<TC[i]->TG->EC.count();j++) if((res = TC[i]->TG->EC[j]->ProcessCommand(cmd)) != "?") return(res + "\n");
   }
   for(i=0;i<comp.count();i++) if((res = comp[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   return("?\n");
}

// This function allows the script system to popup a file selection dalog for file open
// or save.
QString ControlPanel::SelectFile(QString fType, QString Title, QString Ext)
{
    QString fileName = "";

    if(fType.toUpper() == "OPEN")
    {
        fileName = QFileDialog::getOpenFileName(this, Title,"",Ext + " (*." + Ext + ");;All files (*.*)");
    }
    if(fType.toUpper() == "SAVE")
    {
       fileName = QFileDialog::getSaveFileName(this, Title,"",Ext + " (*." + Ext + ");;All files (*.*)");
    }
    return fileName;
}

// Reads a CSV file into the CSVdata list and divides it into lines and elements.
int ControlPanel::ReadCSVfile(QString fileName, QString delimiter)
{
    CSVdata.clear();
    // Open the file for text read
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        // We're going to streaming the file to a QString
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            QStringList *qsl = new QStringList();
            *qsl = line.split(delimiter);
            CSVdata.append(qsl);
        } while(!line.isNull());
        file.close();
        return(CSVdata.count());
    }
    return -1;
}

// Returns the selected CSV line and entry in the selected line
QString ControlPanel::ReadCSVentry(int line, int entry)
{
    if(CSVdata.count() <= line) return("");
    if(CSVdata[line]->count() <= entry) return("");
    return (*CSVdata[line])[entry];
}

void ControlPanel::slotPlotDialogClosed(Plot *thisPlot)
{
    // Find this plot object in the list and then
    // delect the objet and remove from the list
    for(int i=0;i<plots.count();i++)
    {
        if(plots[i] == thisPlot)
        {
            delete plots[i];
            plots.removeAt(i);
        }
    }
}

void ControlPanel::CreatePlot(QString Title, QString Yaxis, QString Xaxis, int NumPlots)
{
    plots.append(new Plot(0, Title, Yaxis, Xaxis, NumPlots));
    plots.last()->show();
    plots.last()->activateWindow();
    plots.last()->raise();
    connect(plots.last(),SIGNAL(DialogClosed(Plot *)),this,SLOT(slotPlotDialogClosed(Plot *)));
    activePlot = plots.last();
}

void ControlPanel::PlotCommand(QString cmd)
{
    if(plots.count() < 1) return;
    if(plots.last() != activePlot) return;
    if(cmd.toUpper() == "DELETE")
    {
        delete plots.last();
        plots.removeLast();
    }
    else plots.last()->PlotCommand(cmd);
}

void ControlPanel::slotCPselected(QString CPfilename)
{
    emit DialogClosed(CPfilename);
}

int ControlPanel::elapsedTime(bool init)
{
    //static QTime time = QTime::currentTime();

    //if(init) time = QTime::currentTime();
    //return time.elapsed();
    static QElapsedTimer time;

    if(init) time.start();
    return time.elapsed();
}
// *************************************************************************************************
// Text box  ***************************************************************************************
// *************************************************************************************************

TextLabel::TextLabel(QWidget *parent, QString name, int s, int x, int y) : QWidget(parent)
{
    p      = parent;
    Title  = name;
    Size   = s;
    X      = x;
    Y      = y;
}

void TextLabel::Show(void)
{
    label = new QLabel(Title,p); label->setGeometry(X,Y,1,1);
    QFont font = label->font();
    font.setPointSize(Size);
    label->setFont(font);
    label->adjustSize();
    label->installEventFilter(this);
    label->setMouseTracking(true);
}

bool TextLabel::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, label, label , event)) return true;
    return false;
}

// *************************************************************************************************
// System shutdown and restore button  *************************************************************
// *************************************************************************************************

Shutdown::Shutdown(QWidget *parent, QString name, int x, int y) : QWidget(parent)
{
    p      = parent;
    Title  = name;
    X      = x;
    Y      = y;
}

void Shutdown::Show(void)
{
    pbShutdown = new QPushButton("System shutdown",p);
    pbShutdown->setGeometry(X,Y,150,32);
    pbShutdown->setAutoDefault(false);
    connect(pbShutdown,SIGNAL(pressed()),this,SLOT(pbPressed()));
    pbShutdown->installEventFilter(this);
    pbShutdown->setMouseTracking(true);
}

bool Shutdown::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, pbShutdown, pbShutdown , event)) return false;
    return false;
}

void Shutdown::SetState(bool ShutDown)
{
    if(ShutDown) pbShutdown->setText("System enable");
    else pbShutdown->setText("System shutdown");
}

void Shutdown::pbPressed(void)
{
    if(pbShutdown->text() == "System shutdown")
    {
        pbShutdown->setText("System enable");
        emit ShutdownSystem();
    }
    else
    {
        pbShutdown->setText("System shutdown");
        emit EnableSystem();
    }
}

// *************************************************************************************************
// Save and load buttons  **************************************************************************
// *************************************************************************************************

SaveLoad::SaveLoad(QWidget *parent, QString nameSave, QString nameLoad, int x, int y) : QWidget(parent)
{
    p          = parent;
    TitleSave  = nameSave;
    TitleLoad  = nameLoad;
    X          = x;
    Y          = y;
}

void SaveLoad::Show(void)
{
    pbSave = new QPushButton(TitleSave,p);
    pbSave->setGeometry(X,Y,150,32);
    pbSave->setAutoDefault(false);
    pbLoad = new QPushButton(TitleLoad,p);
    pbLoad->setGeometry(X,Y+40,150,32);
    pbLoad->setAutoDefault(false);
    pbSave->setMouseTracking(true);
    pbLoad->setMouseTracking(true);
    pbSave->installEventFilter(this);
    pbLoad->installEventFilter(this);
    connect(pbSave,SIGNAL(pressed()),this,SLOT(pbSavePressed()));
    connect(pbLoad,SIGNAL(pressed()),this,SLOT(pbLoadPressed()));
}

bool SaveLoad::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == pbSave){ if(moveWidget(obj, pbSave, pbSave , event)) return true; }
    if(obj == pbLoad){ if(moveWidget(obj, pbLoad, pbLoad , event)) return true; }
    return false;
}

void SaveLoad::pbSavePressed(void)
{
    pbSave->setDown(false);
    emit Save();
}

void SaveLoad::pbLoadPressed(void)
{
//    QApplication::processEvents();
    pbLoad->setDown(false);
    emit Load();
}

// *************************************************************************************************
// Load Control Panel button  **********************************************************************
// *************************************************************************************************

CPbutton::CPbutton(QWidget *parent, QString name, QString CPfilename, int x, int y) : QWidget(parent)
{
    p          = parent;
    Title      = name;
    FileName   = CPfilename;
    X          = x;
    Y          = y;
}

void CPbutton::Show(void)
{
    pbCPselect = new QPushButton(Title,p);
    pbCPselect->setGeometry(X,Y,150,32);
    pbCPselect->setAutoDefault(false);
    connect(pbCPselect,SIGNAL(pressed()),this,SLOT(pbCPselectPressed()));
    pbCPselect->installEventFilter(this);
    pbCPselect->setMouseTracking(true);
}

bool CPbutton::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, pbCPselect, pbCPselect , event)) return true;
    return false;
}

QString CPbutton::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(cmd == res)
    {
        pbCPselectPressed();
        return "";
    }
    return "?";
}

void CPbutton::pbCPselectPressed(void)
{
    pbCPselect->setDown(true);
    emit CPselected(FileName);
}

// *************************************************************************************************
// DAC channels    *********************************************************************************
// *************************************************************************************************

DACchannel::DACchannel(QWidget *parent, QString name, QString MIPSname, int x, int y) : QWidget(parent)
{
    p      = parent;
    Title  = name;
    MIPSnm = MIPSname;
    X      = x;
    Y      = y;
    comms  = NULL;
    Units = "V";
    m = 1;
    b = 0;
    Format = "%.3f";
    Updating = false;
    UpdateOff = false;
}

void DACchannel::Show(void)
{
    frmDAC = new QFrame(p); frmDAC->setGeometry(X,Y,180,21);
    Vdac = new QLineEdit(frmDAC); Vdac->setGeometry(70,0,70,21); Vdac->setValidator(new QDoubleValidator);
    labels[0] = new QLabel(Title,frmDAC); labels[0]->setGeometry(0,0,59,16);
    labels[1] = new QLabel(Units,frmDAC); labels[1]->setGeometry(150,0,31,16);
    Vdac->setToolTip("DAC output CH" +  QString::number(Channel) + ", "  + MIPSnm);
    connect(Vdac,SIGNAL(editingFinished()),this,SLOT(VdacChange()));
    frmDAC->installEventFilter(this);
    frmDAC->setMouseTracking(true);
    labels[0]->installEventFilter(this);
    labels[0]->setMouseTracking(true);
    Vdac->installEventFilter(this);
    Vdac->setMouseTracking(true);
}

bool DACchannel::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, frmDAC, labels[0] , event)) return true;
    if(Updating) return true;
    UpdateOff = true;
    if(adjustValue(obj,Vdac,event,1))
    {
        UpdateOff = false;
        return true;
    }
    UpdateOff = false;
    return QObject::eventFilter(obj, event);
 }

QString DACchannel::Report(void)
{
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title + "," + Vdac->text();
    return(res);
}

bool DACchannel::SetValues(QString strVals)
{
    QStringList resList;
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(!strVals.startsWith(res)) return false;
    resList = strVals.split(",");
    if(resList.count() < 2) return false;
    Vdac->setText(resList[1]);
    Vdac->setModified(true);
    emit Vdac->editingFinished();
    return true;
}

// The following commands are processed:
// title            return the offset value
// title=val        sets the offset value
// returns "?" if the command could not be processed
QString DACchannel::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(!cmd.startsWith(res)) return "?";
    if(cmd == Title) return Vdac->text();
    QStringList resList = cmd.split("=");
    if(resList.count()==2)
    {
       Vdac->setText(resList[1]);
       Vdac->setModified(true);
       emit Vdac->editingFinished();
       return "";
    }
    return "?";
}

// display = m*readValue + b
void DACchannel::Update(void)
{
    QString res;

    if(comms == NULL) return;
    if(UpdateOff) return;
    Updating = true;
    comms->rb.clear();
    res = "GDACV,CH"  + QString::number(Channel) + "\n";
    res = comms->SendMess(res);
    if(res == "")
    {
        Updating = false;
        return;
    }
    res = res.asprintf(Format.toStdString().c_str(),res.toFloat() * m + b);
    if(!Vdac->hasFocus()) Vdac->setText(res);
    Updating = false;
}

// writeValue = (display - b)/m
void DACchannel::VdacChange(void)
{
   QString val;

   if(comms == NULL) return;
   if(!Vdac->isModified()) return;
   val = val.asprintf("%.3f",(Vdac->text().toFloat() - b)/m);
   QString res = "SDACV,CH" + QString::number(Channel) + "," + val + "\n";
   comms->SendCommand(res.toStdString().c_str());
   Vdac->setModified(false);
}

// *************************************************************************************************
// DC bias groups  *********************************************************************************
// *************************************************************************************************

DCBiasGroups::DCBiasGroups(QWidget *parent, int x, int y)
{
    p      = parent;
    X      = x;
    Y      = y;
}

void DCBiasGroups::Show(void)
{
    gbDCBgroups = new QGroupBox("Define DC bias channel groups",p);
    gbDCBgroups->setGeometry(X,Y,251,86);
    gbDCBgroups->setToolTip("DC bias groups are sets of DC bias channels that will track, so when you change one channel all other channels in the group will change by the same about.");
    comboGroups = new QComboBox(gbDCBgroups);
    comboGroups->setGeometry(5,25,236,26);
    comboGroups->setEditable(true);
    comboGroups->setToolTip("Define a group by entering a list of channel names seperated by a comma. You can define many groups.");
    DCBenaGroups = new QCheckBox(gbDCBgroups); DCBenaGroups->setGeometry(5,55,116,20);
    DCBenaGroups->setText("Enable groups");
    DCBenaGroups->setToolTip("Check enable to process the groups and apply.");
    comboGroups->installEventFilter(this);
    connect(DCBenaGroups,SIGNAL(clicked(bool)),this,SLOT(slotEnableChange()));
    gbDCBgroups->installEventFilter(this);
    gbDCBgroups->setMouseTracking(true);
}

void DCBiasGroups::slotEnableChange(void)
{
    // If disable is selected then emit signal to disable;
    if(!DCBenaGroups->isChecked()) emit disable();
    // if emable is selected then emit signal to enable;
    if(DCBenaGroups->isChecked()) emit enable();
}

// One string is passed with all combobox entries. A semicolan seperates
// the entries.
bool DCBiasGroups::SetValues(QString strVals)
{
    DCBenaGroups->setChecked(false);
    comboGroups->clear();
    QStringList resList = strVals.split(";");
    for(int i=0;i<resList.count();i++)
    {
       comboGroups->addItem(resList[i]);
    }
    return true;
}

QString DCBiasGroups::Report(void)
{
    QString res = "";

    for(int i=0;i<comboGroups->count();i++)
    {
        if(res != "") res += ";";
        res += comboGroups->itemText(i);
    }
    return res;
}

bool DCBiasGroups::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, gbDCBgroups, gbDCBgroups , event)) return true;
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

// **********************************************************************************************
// ESI ******************************************************************************************
// **********************************************************************************************

ESI::ESI(QWidget *parent, QString name, QString MIPSname, int x, int y) : QWidget(parent)
{
    p      = parent;
    Title  = name;
    MIPSnm = MIPSname;
    X      = x;
    Y      = y;
    comms  = NULL;
    isShutdown = false;
}

void ESI::Show(void)
{
    frmESI = new QFrame(p); frmESI->setGeometry(X,Y,241,42);
    ESIsp = new QLineEdit(frmESI); ESIsp->setGeometry(70,0,70,21); ESIsp->setValidator(new QDoubleValidator);
    ESIrb = new QLineEdit(frmESI); ESIrb->setGeometry(140,0,70,21); ESIrb->setReadOnly(true);
    ESIena = new QCheckBox(frmESI); ESIena->setGeometry(70,22,70,21);
    ESIena->setText("Enable");
    labels[0] = new QLabel(Title,frmESI); labels[0]->setGeometry(0,0,59,16);
    labels[1] = new QLabel("V",frmESI);   labels[1]->setGeometry(220,0,21,16);
    ESIsp->setToolTip(MIPSnm + " ESI channel " + QString::number(Channel));
    connect(ESIsp,SIGNAL(editingFinished()),this,SLOT(ESIChange()));
    connect(ESIena,SIGNAL(checkStateChanged(Qt::CheckState)),this,SLOT(ESIenaChange()));
    frmESI->installEventFilter(this);
    frmESI->setMouseTracking(true);
    labels[0]->installEventFilter(this);
    labels[0]->setMouseTracking(true);
}

bool ESI::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, frmESI, frmESI , event)) return true;
    return false;
}

QString ESI::Report(void)
{
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title + ",";
    if(isShutdown)
    {
        if(activeEnableState) res += "ON,";
        else res += "OFF,";
    }
    else
    {
        if(ESIena->isChecked()) res += "ON,";
        else res += "OFF,";
    }
    res += ESIsp->text() + "," + ESIrb->text();
    return(res);
}

bool ESI::SetValues(QString strVals)
{
    QStringList resList;
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(!strVals.startsWith(res)) return false;
    resList = strVals.split(",");
    if(resList.count() < 3) return false;
    if(isShutdown)
    {
        if(resList[1].contains("ON")) activeEnableState = true;
        else  activeEnableState = false;
    }
    else
    {
        if(resList[1].contains("ON")) ESIena->setChecked(true);
        else ESIena->setChecked(false);
        if(resList[1].contains("ON")) emit ESIena->checkStateChanged(Qt::Checked);
        else emit ESIena->checkStateChanged(Qt::Unchecked);
    }
    ESIsp->setText(resList[2]);
    ESIsp->setModified(true);
    emit ESIsp->editingFinished();
    return true;
}

// title        setpoint
// title=val    setpoint
// title.readback
// title.ena
// title.ena= ON or OFF
QString ESI::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(!cmd.startsWith(res)) return "?";
    if(cmd == res) return ESIsp->text();
    if(cmd == res + ".readback") return ESIrb->text();
    if(cmd == res + ".ena")
    {
        if(ESIena->isChecked()) return "ON";
        return "OFF";
    }
    QStringList resList = cmd.split("=");
    if(resList.count()==2)
    {
       if(resList[0] == Title)
       {
           ESIsp->setText(resList[1]);
           ESIsp->setModified(true);
           emit ESIsp->editingFinished();
           return "";
       }
       if(resList[0] == res + ".ena")
       {
           if(resList[1] == "ON") ESIena->setChecked(true);
           else if(resList[1] == "OFF") ESIena->setChecked(false);
           else return "?";
           if(resList[1] == "ON") emit ESIena->checkStateChanged(Qt::Checked);
           else  emit ESIena->checkStateChanged(Qt::Unchecked);
           return "";
       }
    }
    return "?";
}

void ESI::Update(void)
{
    QString res;

    if(comms == NULL) return;

    comms->rb.clear();
    res = "GHV,"  + QString::number(Channel) + "\n";
    res = comms->SendMess(res);
    if(res == "") return;
    if(!ESIsp->hasFocus()) ESIsp->setText(res);
    res = "GHVV," + QString::number(Channel) + "\n";
    res = comms->SendMess(res);
    if(res=="") return;
    ESIrb->setText(res);
    res = comms->SendMess("GHVSTATUS," + QString::number(Channel) + "\n");
    bool oldState = ESIena->blockSignals(true);
    if(res.contains("ON")) ESIena->setChecked(true);
    if(res.contains("OFF")) ESIena->setChecked(false);
    ESIena->blockSignals(oldState);
}

void ESI::ESIChange(void)
{
    if(comms == NULL) return;
    if(!ESIsp->isModified()) return;
    QString res = "SHV," + QString::number(Channel) + "," + ESIsp->text() + "\n";
    comms->SendCommand(res.toStdString().c_str());
    ESIsp->setModified(false);
}

void ESI::ESIenaChange(void)
{
   QString res;

   if(comms == NULL) return;
   if(ESIena->checkState()) res ="SHVENA," + QString::number(Channel) + "\n";
   else res ="SHVDIS," + QString::number(Channel) + "\n";
   comms->SendCommand(res.toStdString().c_str());
}

void ESI::Shutdown(void)
{
    if(isShutdown) return;
    isShutdown = true;
    activeEnableState = ESIena->checkState();
    ESIena->setChecked(false);
    emit ESIena->checkStateChanged(Qt::Unchecked);
}

void ESI::Restore(void)
{
    if(!isShutdown) return;
    isShutdown = false;
    ESIena->setChecked(activeEnableState);
    emit ESIena->checkStateChanged(Qt::Unchecked);
}

// **********************************************************************************************
// Ccontrol *************************************************************************************
// **********************************************************************************************
//  Implements a generic control with the following features:
//      - TYPE, defines the control type
//              - LineEdit, CheckBox, Button, ComboBox
//      - Set command
//              to set the MIPS value from LineEdit box
//              checked state command for the CheckBox
//              pressed command for Button
//      - Get command
//              to read the MIPS value to update the LineEdit box
//              unchecked state command for the CheckBox
//              not used for Button
//      - Readback command
//              if set add a readback box and this command will read the value for LineEdit
//              if CheckBox has read command then format is COMMAND_CHK_UNCHK
//              not used for Button
//  Syntax example:
//      Ccontrol,name,mips name,type,get command,set command,readback command,units,X,Y

Ccontrol::Ccontrol(QWidget *parent, QString name, QString MIPSname,QString Type, QString Gcmd, QString Scmd, QString RBcmd, QString Units, int x, int y)
{
    p      = parent;
    Title  = name;
    MIPSnm = MIPSname;
    GetCmd = Gcmd.replace("_",",");
    SetCmd = Scmd.replace("_",",");
    ReadbackCmd = RBcmd.replace("_",",");
    UnitsText = Units;
    Ctype = Type;
    X      = x;
    Y      = y;
    comms  = NULL;
    Step = 1.0;
    isShutdown = false;
    Updating   = false;
    UpdateOff  = false;
    ShutdownValue.clear();
    Dtype = "Double";
    comboBox = NULL;
}

bool Ccontrol::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == frmCc) if(moveWidget(obj, frmCc, frmCc, event)) return true;
    if(obj == pbButton) if(moveWidget(obj, pbButton, pbButton, event)) return true;

    if(Dtype.toUpper() == "STRINGL") return QObject::eventFilter(obj, event);
    if(Updating) return true;
    UpdateOff = true;
    if(adjustValue(obj, Vsp, event, Step))
    {
        UpdateOff = false;
        return true;
    }
    UpdateOff = false;
    return QObject::eventFilter(obj, event);
}

void Ccontrol::Show(void)
{
    if(Ctype == "LineEdit")
    {
        frmCc = new QFrame(p);
        labels[0] = new QLabel(Title,frmCc); labels[0]->setGeometry(0,0,59,16);
        labels[1] = new QLabel(UnitsText,frmCc);
        // If Read back command or set command are empty then its only one lineEdit
        // box, else two.
        if(SetCmd.isEmpty() || ReadbackCmd.isEmpty())
        {
            // Only 1 line edit box
            if(Dtype.toUpper() == "STRINGL") frmCc->setGeometry(X,Y,400,21);
            else frmCc->setGeometry(X,Y,175,21);
            if(SetCmd.isEmpty())
            {
                Vrb = new QLineEdit(frmCc);
                Vrb->setGeometry(70,0,70,21);
                Vrb->setReadOnly(true);
            }
            else if(ReadbackCmd.isEmpty())
            {
                Vsp = new QLineEdit(frmCc);
                if(Dtype.toUpper() == "STRINGL") Vsp->setGeometry(70,0,300,21);
                else Vsp->setGeometry(70,0,70,21);
                if(Dtype.toUpper() == "DOUBLE") Vsp->setValidator(new QDoubleValidator);
                Vsp->setToolTip(MIPSnm + "," + SetCmd);
                connect(Vsp,SIGNAL(editingFinished()),this,SLOT(VspChange()));
            }
            labels[1]->setGeometry(150,0,30,16);
        }
        else
        {
            frmCc->setGeometry(X,Y,245,21);
            Vsp = new QLineEdit(frmCc); Vsp->setGeometry(70,0,70,21); //Vsp->setValidator(new QDoubleValidator);
            Vrb = new QLineEdit(frmCc); Vrb->setGeometry(140,0,70,21); Vrb->setReadOnly(true);
            labels[1]->setGeometry(220,0,30,16);
            Vsp->setToolTip(MIPSnm + "," + SetCmd);
            connect(Vsp,SIGNAL(editingFinished()),this,SLOT(VspChange()));
        }
    }
    if(Ctype == "CheckBox")
    {
        frmCc = new QFrame(p); frmCc->setGeometry(X,Y,241,21);
        chkBox = new QCheckBox(frmCc); chkBox->setGeometry(0,0,200,21);
        chkBox->setText(Title);
        connect(chkBox,SIGNAL(checkStateChanged(Qt::CheckState)),this,SLOT(chkBoxStateChanged(Qt::CheckState)));
    }
    if(Ctype == "Button")
    {
        pbButton = new QPushButton(Title,p);
        pbButton->setGeometry(X,Y,150,32);
        pbButton->setAutoDefault(false);
        connect(pbButton,SIGNAL(pressed()),this,SLOT(pbButtonPressed()));
    }
    if(Ctype == "ComboBox")
    {
        frmCc = new QFrame(p); frmCc->setGeometry(X,Y,241,21);
        labels[0] = new QLabel(Title,frmCc); labels[0]->setGeometry(0,0,59,16);
        comboBox = new QComboBox(frmCc); comboBox->setGeometry(70,0,70,21);
        labels[1] = new QLabel(UnitsText,frmCc); labels[1]->setGeometry(150,0,30,16);
        //connect(comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(comboBoxChanged(QString)));
        connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(comboBoxChanged(int)));
    }
    if(frmCc!=NULL)
    {
        frmCc->installEventFilter(this);
        frmCc->setMouseTracking(true);
    }
    if(labels[0]!=NULL)
    {
        labels[0]->installEventFilter(this);
        labels[0]->setMouseTracking(true);
    }
    if(pbButton!=NULL)
    {
        pbButton->installEventFilter(this);
        pbButton->setMouseTracking(true);
    }
    if(Vsp!=NULL)
    {
        Vsp->installEventFilter(this);
        Vsp->setMouseTracking(true);
    }
}

void Ccontrol::Update(void)
{
    QString res;

    if(comms == NULL) return;
    comms->rb.clear();
    if(Ctype == "LineEdit")
    {
        if(!GetCmd.isEmpty())
        {
            res = comms->SendMess(GetCmd + "\n");
            if(res == "") return;
            if(!Vsp->hasFocus()) Vsp->setText(res);
        }
        if(!ReadbackCmd.isEmpty())
        {
            res = comms->SendMess(ReadbackCmd + "\n");
            if(res == "") return;
            Vrb->setText(res);
        }
    }
    if(Ctype == "CheckBox")
    {
        // If there is a readback string then this is the update command. The
        // will contain the check and unchecked responses that are , delimited.
        if(!ReadbackCmd.isEmpty())
        {
            QStringList resList = ReadbackCmd.split(",");
            if(resList.count() >= 3)
            {
                if(resList.count() == 3) res =  comms->SendMess(resList[0] + "\n");
                if(resList.count() == 4) res =  comms->SendMess(resList[0] + "," + resList[1] + "\n");
                chkBox->setUpdatesEnabled(false);
                if(res == resList[resList.count() - 2]) chkBox->setChecked(true);
                else if(res == resList[resList.count() - 1]) chkBox->setChecked(false);
                chkBox->setUpdatesEnabled(true);
            }
        }
    }
    if(Ctype == "ComboBox")
    {
        if(!GetCmd.isEmpty())
        {
            res = comms->SendMess(GetCmd + "\n");
            if(res == "") return;
            int i = comboBox->findText(res.trimmed());
            if(i < 0) return;
            comboBox->setCurrentIndex(i);
        }
    }
}

QString Ccontrol::Report(void)
{
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(Ctype == "LineEdit")
    {
        if(!GetCmd.isEmpty() || !SetCmd.isEmpty())
        {
            res += "," + Vsp->text();
            if(isShutdown) res += "," + ActiveValue;
            else res += "," + Vsp->text();
        }
        if(!ReadbackCmd.isEmpty()) res += "," + Vrb->text();
        return(res);
    }
    if(Ctype == "CheckBox")
    {
        if(!ReadbackCmd.isEmpty())
        {
            QStringList resList = ReadbackCmd.split("_");
            if(resList.count() == 3)
            {
                if(chkBox->isChecked()) return resList[1];
                else return resList[2];
            }
        }
        if(chkBox->isChecked()) return(res + "," + "TRUE");
        else return(res + "," + "FALSE");
    }
    if(Ctype == "ComboBox")
    {
       return(res + "," + comboBox->currentText());
    }
    return("");
}

bool Ccontrol::SetValues(QString strVals)
{
    QStringList resList;
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(Ctype == "LineEdit")
    {
        if(!strVals.startsWith(res)) return false;
        resList = strVals.split(",");
        if(resList.count() < 2) return false;
        if(resList[0] != res) return false;
        if(SetCmd.isEmpty()) return false;
        if(isShutdown) ActiveValue = resList[1];
        else
        {
            Vsp->setText(resList[1]);
            Vsp->setModified(true);
            emit Vsp->editingFinished();
        }
        return true;
    }
    if(Ctype == "CheckBox")
    {
        if(!strVals.startsWith(res)) return false;
        resList = strVals.split(",");
        if(resList.count() < 2) return false;
        if(resList[0] != res) return false;
        if(!ReadbackCmd.isEmpty())
        {
            QStringList resCmd = ReadbackCmd.split(",");
            if(resCmd.count() == 3)
            {
                bool returnstate = true;
                chkBox->setUpdatesEnabled(false);
                if(resCmd[1] == resList[1]) chkBox->setChecked(true);
                else if(resCmd[2] == resList[1]) chkBox->setChecked(false);
                else returnstate = false;
                chkBox->setUpdatesEnabled(true);
                return returnstate;
            }
        }
        bool returnstate = true;
        chkBox->setUpdatesEnabled(false);
        if(resList[1] == "TRUE") chkBox->setChecked(true);
        else if(resList[1] == "FALSE") chkBox->setChecked(false);
        else returnstate = false;
        chkBox->setUpdatesEnabled(true);
        return returnstate;
    }
    if(Ctype == "ComboBox")
    {
        if(!strVals.startsWith(res)) return false;
        resList = strVals.split(",");
        if(resList.count() < 2) return false;
        if(resList[0] != res) return false;
        int i = comboBox->findText(resList[1].trimmed());
        if(i<0) return false;
        comboBox->setCurrentIndex(i);
        comboBoxChanged(i);
        return true;
    }
    return false;
 }

void Ccontrol::Shutdown(void)
{
    if(Ctype == "LineEdit")
    {
        if(ShutdownValue.isEmpty()) return;
        if(isShutdown) return;
        isShutdown = true;
        ActiveValue = Vsp->text();
        Vsp->setText(ShutdownValue);
        Vsp->setModified(true);
        emit Vsp->editingFinished();
    }
}

void Ccontrol::Restore(void)
{
    if(Ctype == "LineEdit")
    {
        if(!isShutdown) return;
        isShutdown = false;
        Vsp->setText(ActiveValue);
        Vsp->setModified(true);
        emit Vsp->editingFinished();
    }
}

void Ccontrol::SetList(QString strOptions)
{
    if(comboBox != NULL)
    {
        comboBox->clear();
        QStringList resList = strOptions.split(",");
        for(int i=0;i<resList.count();i++)
        {
            comboBox->addItem(resList[i]);
        }
    }
}


// The following commands are processed:
// title            return the setpoint
// title=val        sets the setpoint
// title.readback   returns the readback voltage
// returns "?" if the command could not be processed
QString Ccontrol::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(Ctype == "LineEdit")
    {
        if(!cmd.startsWith(res)) return "?";
        if(cmd == res)
        {
            if(!SetCmd.isEmpty() || !GetCmd.isEmpty()) return Vsp->text();
            if(!ReadbackCmd.isEmpty()) return Vrb->text();
        }
        if(cmd == res + ".readback")
        {
            if((!SetCmd.isEmpty() || !GetCmd.isEmpty()) && !ReadbackCmd.isEmpty()) return Vrb->text();
            return "?";
        }
        QStringList resList = cmd.split("=");
        if((resList.count()==2) && !SetCmd.isEmpty() && (resList[0] == res))
        {
           Vsp->setText(resList[1].trimmed());
           Vsp->setModified(true);
           emit Vsp->editingFinished();
           return "";
        }
        return "?";
    }
    if(Ctype == "CheckBox")
    {
        if(!cmd.startsWith(res)) return "?";
        if(cmd == res)
        {
            if(!ReadbackCmd.isEmpty())
            {
                QStringList resCmd = ReadbackCmd.split("_");
                if(resCmd.count() == 3)
                {
                    if(chkBox->isChecked()) return resCmd[1];
                    else return resCmd[2];
                }
            }
            if(chkBox->isChecked()) return "TRUE";
            else return "FALSE";
        }
        QStringList resList = cmd.split("=");
        if((resList.count()==2) && !SetCmd.isEmpty() && (resList[0] == res))
        {
            if(!ReadbackCmd.isEmpty())
            {
                QStringList resCmd = ReadbackCmd.split("_");
                if(resCmd.count() == 3)
                {
                    if(resCmd[1] == resList[1]) chkBox->setChecked(true);
                    else if(resCmd[2] == resList[1]) chkBox->setChecked(false);
                    else return "?";
                    return "";
                }
            }
            if(resList[1] == "TRUE") chkBox->setChecked(true);
            else if(resList[1] == "FALSE") chkBox->setChecked(false);
            else return "?";
            return "";
        }
    }
    if(Ctype == "Button")
    {
        if(!cmd.startsWith(res)) return "?";
        if(cmd == res)
        {
            pbButtonPressed();
            return "";
        }
    }
    if(Ctype == "ComboBox")
    {
        if(!cmd.startsWith(res)) return "?";
        if(cmd == res) return comboBox->currentText();
        QStringList resList = cmd.split("=");
        int i = comboBox->findText(resList[1].trimmed());
        if((i < 0) || (resList[0] != res))  return "?";
        comboBox->setCurrentIndex(i);
        comboBoxChanged(i);
        return "";
    }
    return "?";
}

void Ccontrol::VspChange(void)
{
   if(!Vsp->isModified()) return;
   QString res = SetCmd + "," + Vsp->text() + "\n";
   if(comms != NULL) comms->SendCommand(res.toStdString().c_str());
   Vsp->setModified(false);
}

void Ccontrol::pbButtonPressed(void)
{
    pbButton->setFocus();
    if(comms == NULL) return;
    pbButton->setDown(false);
    comms->SendCommand(SetCmd + "\n");
}

void Ccontrol::chkBoxStateChanged(Qt::CheckState state)
{
    chkBox->setFocus();
    if(comms == NULL) return;
    if(state == Qt::Checked) comms->SendCommand(SetCmd + "\n");
    else if(state == Qt::Unchecked) comms->SendCommand(GetCmd + "\n");
}

void Ccontrol::comboBoxChanged(int i)
{
    if(comms == NULL) return;
    comms->SendCommand(SetCmd +"," + comboBox->itemText(i) + "\n");
}

// **********************************************************************************************
// Cpanel   *************************************************************************************
// **********************************************************************************************

Cpanel::Cpanel(QWidget *parent, QString name, QString CPfileName, int x, int y, QList<Comms*> S, Properties *prop, ControlPanel *pcp)
{
    cp = new ControlPanel(0,CPfileName,S,prop,pcp);
    p      = parent;
    X      = x;
    Y      = y;
    Title = name;
    connect(cp,SIGNAL(DialogClosed(QString)),this,SLOT(slotDialogClosed()));
}

void Cpanel::Show(void)
{
    pbButton = new QPushButton(Title,p);
    pbButton->setGeometry(X,Y,150,32);
    pbButton->setAutoDefault(false);
    connect(pbButton,SIGNAL(pressed()),this,SLOT(pbButtonPressed()));
    pbButton->installEventFilter(this);
    pbButton->setMouseTracking(true);
}

void Cpanel::Update(void)
{
    if(cp->isVisible()) cp->Update();
}

bool Cpanel::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, pbButton, pbButton , event)) return true;
    return false;
}

QString Cpanel::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(cmd == res)
    {
        pbButtonPressed();
        return "";
    }
    res = Title + ".";
    if(!cmd.startsWith(res)) return "?";
    return cp->Command(cmd.mid(res.length()));
}

void Cpanel::pbButtonPressed(void)
{
    pbButton->setDown(false);
    cp->show();
    cp->raise();
}

void Cpanel::slotDialogClosed(void)
{
    cp->hide();
}

// *************************************************************************************************
// Status Light  ***********************************************************************************
// *************************************************************************************************

StatusLight::StatusLight(QWidget *parent, QString name, int x, int y)
{
    p      = parent;
    Title  = name;
    X      = x;
    Y      = y;
    Status.clear();
    Mode.clear();
}

void StatusLight::Show(void)
{
    Message = new QLabel(p); Message->setGeometry(X,Y,1,1);
    label = new QLabel(Title,p); label->setGeometry(X,Y,1,1);
    label->adjustSize();
    label->setGeometry(X + 20 - (label->width()/2),Y,label->width(),label->height());
    TL = new QLabel(p);
    TL->setGeometry(X,Y + label->height(),40,96);
    widget = new TrafficLightWidget(TL);
    widget->resize(40, 96);
    widget->show();
    label->installEventFilter(this);
    label->setMouseTracking(true);
}

bool StatusLight::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, label, label , event))
    {
        X = label->x();
        Y = label->y();
        Message->setGeometry(X,Y,1,1);
        label->setGeometry(X + 20 - (label->width()/2),Y,label->width(),label->height());
        TL->setGeometry(X,Y + label->height(),40,96);
        //Message->setGeometry(X + 20 - (Message->width()/2),Y + 96 + label->height() ,Message->width(),Message->height());
        ShowMessage();
        return true;
    }
    return false;
}

void StatusLight::ShowMessage(void)
{
    QString mes;

    mes = Status;
    if(!Mode.isEmpty()) mes = mes +"\n" + Mode;
    Message->setText(mes);
    Message->adjustSize();
    Message->setGeometry(X + 20 - (Message->width()/2),Y + 96 + label->height() ,Message->width(),Message->height());
}

QString StatusLight::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(!cmd.startsWith(res)) return "?";
    if((res + ".message") == cmd.trimmed()) return Status;
    if((res + ".mode") == cmd.trimmed()) return Mode;
    if(cmd.trimmed() == res)
    {
        if(widget->greenLight()->isOn()) return "GREEN";
        if(widget->redLight()->isOn()) return "RED";
        if(widget->yellowLight()->isOn()) return "YELLOW";
        return "NONE";
    }
    QStringList resList = cmd.split("=");
    if(resList.count()==2)
    {
       if((res + ".message") == resList[0].trimmed())
       {
           Status = resList[1];
           ShowMessage();
           return "";
       }
       else if((res + ".mode") == resList[0].trimmed())
       {
           Mode = resList[1];
           ShowMessage();
           return "";
       }
       widget->greenLight()->turnOff();
       widget->redLight()->turnOff();
       widget->yellowLight()->turnOff();
       if(resList[1].trimmed() == "GREEN") widget->greenLight()->turnOn();
       if(resList[1].trimmed() == "RED") widget->redLight()->turnOn();
       if(resList[1].trimmed() == "YELLOW") widget->yellowLight()->turnOn();
       return "";
    }
    return "?";
}

// *************************************************************************************************
// Text message  ***********************************************************************************
// *************************************************************************************************

TextMessage::TextMessage(QWidget *parent, QString name, int x, int y)
{
    p      = parent;
    Title  = name;
    X      = x;
    Y      = y;
    MessageText.clear();
}

void TextMessage::Show(void)
{
    Message = new QLabel(p); Message->setGeometry(X,Y,1,1);
    label = new QLabel(Title,p); label->setGeometry(X,Y,1,1);
    label->adjustSize();
    TL = new QLabel(p);
    TL->setGeometry(X,Y,1,1);
    label->installEventFilter(this);
    label->setMouseTracking(true);
}

bool TextMessage::eventFilter(QObject *obj, QEvent *event)
{
    if(moveWidget(obj, label, label , event)) return true;
    return false;
}

void TextMessage::ShowMessage(void)
{
    Message->setText(MessageText);
    Message->adjustSize();
    Message->setGeometry(X + label->width(),Y,Message->width(),Message->height());
}

QString TextMessage::ProcessCommand(QString cmd)
{
    QString res;

    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(!cmd.startsWith(res)) return "?";
    if(cmd.trimmed() == res) return MessageText;
    QStringList resList = cmd.split("=");
    if(resList.count()==2)
    {
       if((res) == resList[0].trimmed())
       {
          MessageText = resList[1];
          ShowMessage();
          return "";
       }
    }
    return "?";
}

