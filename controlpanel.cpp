//
// Controlpanel.cpp
//
// This file contains the classes used to create a user defined control panel. The control panels
// display MIPS controls for most features supported by MIPS. The user can define a background
// image and then place controls such as RF channels and DC bias channels at the locations the user
// defines on the background image.
//
// This feature also supports multiple MIPS systems connected to your PC at the same time. Use the
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
// Gordon Anderson
//
#include "controlpanel.h"
#include "ui_controlpanel.h"
#include "cdirselectiondlg.h"
#include "scriptingconsole.h"
#include "Utilities.h"

#include <QPixmap>
#include <QString>
#include <QTextEdit>
#include <QTreeView>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTabWidget>

QSemaphore UpdateSemaphore(1);

/*! \brief ControlPanel::ControlPanel
 *  Constructor for the ControlPanel class. This constructor will read the configuration file
 *  and create the control panel.
 *  \param parent The parent widget of this control panel.
 *  \param CPfileName The name of the control panel configuration file to load.
 *  \param S The list of MIPS systems connected to this control panel.
 *  \param prop Pointer to the Properties object for logging.
 *  \param pcp Pointer to the parent control panel, if any.
 */
ControlPanel::ControlPanel(QWidget *parent, QString CPfileName, QList<Comms*> S, Properties *prop, ControlPanel *pcp) :
    QDialog(parent),
    ui(new Ui::ControlPanel)
{
    m_storage.clear();
    QString readOption;
    QStringList resList;
    qint64 filePos = -1,optionsFilePos = -1;
    int NextRecord;
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
    Tables.clear();
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
    scriptObj.clear();
    plots.clear();
    Containers.clear();
    CPbuttons.clear();
    ModChans.clear();
    cpObjects.clear();
    iBox.clear();
    Sliders.clear();
    extProcs.clear();
    plotData.clear();
    UpdateHoldOff  = 0;
    UpdateStop     = false;
    ShutdownFlag   = false;
    RestoreFlag    = false;
    StartMIPScomms = false;
    SystemIsShutdown = false;
    isUpdating = false;
    scriptconsole = NULL;
    tcp = NULL;
    sl.clear();
    tm.clear();
    properties = prop;
    help = new Help();
    comments = new Help();
    LogFile.clear();
    updateCount = generateRandomInt(1, skipCount);
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
            // First look for sections in double quotes
            // Added Jan 4, 2025, this code will only work for
            // a single double quoted token on a line
            QStringList lineList = line.trimmed().split("\"");
            resList.clear();
            if(lineList.length() > 1) for(int i=0;i<lineList.length();i++)
            {
                // all odd index elements are within double quotes
                if((i&1) == 1)  resList.append(lineList[i].trimmed());
                else if(i == 0) resList += lineList[i].chopped(1).split(",");
                else resList += lineList[i].remove(0,1).split(",");
            }
            else resList = line.trimmed().split(",");

            // Look for the Options command. This command defines an Options file on
            // the defined MIPS system. This file contins config file options to allow
            // the cfg file to change based on the file found on the attched MIPS system.
            // Options,filename,MIPSname
            if((resList[0].toUpper() == "OPTIONS") && (resList.length()==3))
            {
                readOption = "READSD," + resList[1].trimmed() + ",";
                QString option = SendMess(resList[2].trimmed(),readOption + "1\n").trimmed();
                if(option.contains("?")) continue;
                if(option.contains("not found!")) continue;
                if(option.toUpper() == "EOL") continue;
                line = option;
                resList = line.trimmed().split(",");
                // Save the current file pointer for the NEXT command
                optionsFilePos = stream.pos();
                NextRecord = 2;
            }
            if((resList[0].toUpper() == "NEXT") && (resList.length()==2))
            {
                if(optionsFilePos >= 0)
                {
                    QString option = SendMess(resList[1].trimmed(),readOption + QString::number(NextRecord) + "\n").trimmed();
                    if((option.contains("?")) || (option.contains("not found!")) || (option.toUpper() == "EOL"))
                    {
                        optionsFilePos = -1;
                        continue;
                    }
                    line = option;
                    resList = line.trimmed().split(",");
                    NextRecord++;
                    stream.seek(optionsFilePos);
                }
            }
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
                   if(line.isNull()) { stream.seek(filePos); filePos = -1; line = "not empty"; break;}
                } while(!line.isNull());               
             }
            // End of Call processing
            if((resList[0].toUpper() == "ENDSUBROUTINE") && (resList.length()==1))
            {
                if(filePos >= 0)
                {
                    stream.seek(filePos);
                    //line = stream.readLine();   // This was causing the system to miss the next line after a Call
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
                // Now read the config file until the ENDSCRIPT statement
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
            // scriptObj,name,fileName (or script text name)
            if((resList[0].toUpper() == "SCRIPTOBJ") && (resList.length()==3))
            {
                scriptObj.append(new Script(this,resList[1],resList[2],properties,statusBar));
            }
            // scriptObj,name,fileName (or script text name), call(arguments)
            if((resList[0].toUpper() == "SCRIPTOBJ") && (resList.length()==4))
            {
                scriptObj.append(new Script(this,resList[1],resList[2],properties,statusBar));
                scriptObj.last()->scriptCall = resList[3];
            }
            // PlotData,name,yaxis name, x axis name, num graphs, width, height, x, y
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
            if((resList[0].toUpper() == "SKIPCOUNT") && (resList.length()==2))
            {
                skipCount = resList[1].toInt();
                if(skipCount <= 0) skipCount = 1;
                if(skipCount > 100) skipCount = 100;
                updateCount = generateRandomInt(1, skipCount);
            }
            // ImageBox,filename,width,height,x,y
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
            if((resList[0].toUpper() == "TABLE") && (resList.length()==8))
            {
                Tables.append(new Table(Containers.last(),resList[1],resList[2].toInt(),resList[3].toInt(),resList[4].toInt(),resList[5].toInt(),resList[6].toInt(),resList[7].toInt()));
                Tables.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("Table");
                cpObjects.append(QVariant::fromValue(Tables.last()));
                connect(Tables.last(),SIGNAL(change(QString)),this,SLOT(controlChange(QString)));
            }
            if((resList[0].toUpper() == "SLIDER") && (resList.length()==8))
            {
                Sliders.append(new Slider(Containers.last(),resList[1],resList[2],resList[3].toInt(),resList[4].toInt(),resList[5].toInt(),resList[6].toInt(),resList[7].toInt()));
                Sliders.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("Slider");
                cpObjects.append(QVariant::fromValue(Sliders.last()));
                connect(Sliders.last(),SIGNAL(change(QString)),this,SLOT(controlChange(QString)));
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
                Ccontrols.last()->skipCount = skipCount;
                Ccontrols.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("Ccontrol");
                cpObjects.append(QVariant::fromValue(Ccontrols.last()));
                connect(Ccontrols.last(),SIGNAL(change(QString)),this,SLOT(controlChange(QString)));
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
                QWidget *widget = new QWidget(Containers.last());
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
                //Containers.push_back(rfa.last()); // Add RFamp to Containers
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
            if((resList[0].toUpper() == "SCRIPTBUTTON") && (resList.length()==6))
            {
                QString sfn = findFile(resList[2],QFileInfo(ControlPanelFile).canonicalPath());
                if(sfn == "") sfn = resList[2];
                ScripButtons.append(new ScriptButton(Containers.last(), resList[1], sfn, resList[4].toInt(), resList[5].toInt(), properties, statusBar));
                ScripButtons.last()->Show();
                ScripButtons.last()->scriptCall = resList[3];
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
                sl.append(new StatusLight(Containers.last(),resList[1],resList[2].toInt(), resList[3].toInt()));
                sl.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("StatusLight");
                cpObjects.append(QVariant::fromValue(sl.last()));
            }
            if((resList[0].toUpper() == "TEXTMESSAGE") && (resList.length()==4))
            {
                tm.append(new TextMessage(Containers.last(),resList[1],resList[2].toInt(), resList[3].toInt()));
                tm.last()->Show();
                cpObjects.append(stream.pos());
                cpObjects.append("TextMessage");
                cpObjects.append(QVariant::fromValue(tm.last()));
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
                    //qDebug() << tab->objectName();
                    if(tab->objectName().endsWith(resList[1]))
                    {
                        // Now find the proper tab
                        for(int i = 0; i<tab->count(); i++)
                        {
                            //qDebug() << tab->tabText(i);
                            //qDebug() << resList[2];
                            //if(tab->tabText(i) == resList[2]) Containers.last() = tab->widget(i);

                            if(Containers.last()->objectName() != "") tab->widget(i)->setObjectName(Containers.last()->objectName() + "." + tab->tabText(i));
                            else tab->widget(i)->setObjectName(tab->tabText(i));

                            if(tab->tabText(i) == resList[2])
                            {
                                //qDebug() << "Tab widget name: " << tab->widget(i)->objectName();
                                Containers.append(tab->widget(i));
                            }
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
            if((resList[0].toUpper() == "GROUPBOX") && ((resList.length()==6) || (resList.length()==8)))
            {
                if(resList.length()==8)
                {
                    QScrollArea *scrollArea = new QScrollArea(Containers.last());
                    scrollArea->setGeometry(resList[4].toInt(),resList[5].toInt(),resList[6].toInt(),resList[7].toInt());
                    scrollArea->setStyleSheet("QScrollArea { border: 0px; }");
                    GroupBoxes.append(new QGroupBox(Containers.last()));
                    GroupBoxes.last()->setGeometry(resList[4].toInt(),resList[5].toInt(),resList[2].toInt(),resList[3].toInt());
                    GroupBoxes.last()->setTitle(resList[1]);
                    if(resList[1] != "")
                    {
                        if(Containers.last()->objectName() != "") GroupBoxes.last()->setObjectName(Containers.last()->objectName() + "." + resList[1]);
                        else GroupBoxes.last()->setObjectName(resList[1]);
                    }
                    else if(Containers.last()->objectName() != "") GroupBoxes.last()->setObjectName(Containers.last()->objectName());
                    Containers.append(GroupBoxes.last());
                    GroupBoxes.last()->installEventFilter(this);
                    GroupBoxes.last()->setMouseTracking(true);
                    GroupBoxes.last()->setMinimumSize(resList[2].toInt(),resList[3].toInt());
                    scrollArea->setWidget(GroupBoxes.last());
                }
                else
                {
                    GroupBoxes.append(new QGroupBox(Containers.last()));
                    GroupBoxes.last()->setGeometry(resList[4].toInt(),resList[5].toInt(),resList[2].toInt(),resList[3].toInt());
                    GroupBoxes.last()->setTitle(resList[1]);
                    if(resList[1] != "")
                    {
                        if(Containers.last()->objectName() != "") GroupBoxes.last()->setObjectName(Containers.last()->objectName() + "." + resList[1]);
                        else GroupBoxes.last()->setObjectName(resList[1]);
                    }
                    else if(Containers.last()->objectName() != "") GroupBoxes.last()->setObjectName(Containers.last()->objectName());
                    Containers.append(GroupBoxes.last());
                    GroupBoxes.last()->installEventFilter(this);
                    GroupBoxes.last()->setMouseTracking(true);
                }
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
                connect(TC.last()->TG,SIGNAL(change(QString)),this,SLOT(controlChange(QString)));
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
                if(parentCP == NULL)
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
                tc->TG->AddSignal("ARB1aux","101");
                tc->TG->AddSignal("ARB2aux","102");
                tc->TG->AddSignal("ARB3aux","103");
                tc->TG->AddSignal("ARB4aux","104");
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
        foreach(ScriptString *SS, scriptStr)
        {
            if(SS->Name == scrpt->FileName)
            {
                scrpt->ScriptText = SS->ScriptText;
                scrpt->ScriptTextFixed = true;
                // Look for a script string named Common and append it
                // to the current script
                foreach(ScriptString *CS, scriptStr)
                {
                    if(CS->Name == "Common")
                    {
                        scrpt->ScriptText += CS->ScriptText;
                    }
                }
            }
        }
    }
    // Look at all the script objects and if the file name matches a script
    // string then place script text in the scriptObj
    foreach(Script *scrpt, scriptObj)
    {
        foreach(ScriptString *SS, scriptStr)
        {
            if(SS->Name == scrpt->fileName)
            {
                scrpt->scriptText = SS->ScriptText;
                // Look for a script string named Common and append it
                // to the current script
                foreach(ScriptString *CS, scriptStr)
                {
                    if(CS->Name == "Common")
                    {
                        scrpt->scriptText += CS->ScriptText;
                    }
                }
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

/*! \brief ControlPanel::eventFilter
 * This function is called when an event occurs in the control panel.
 * It will move the widget to the correct position if it is being moved
 * and will also handle the right click event to display the help popup.
 */
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

    if(event->type() == QEvent::KeyPress)
    {
        // This code stops the control panel from unloading when the escape key is pressed
        QKeyEvent *key = static_cast<QKeyEvent *>(event);
        if(key->key() == 16777216) return true;
    }
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

/*! \brief ControlPanel::~ControlPanel
 * This function is called when the control panel is unloaded. It will
 * send the exit script to the MIPS system and then delete all the
 * objects created by this control panel.
 */
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
    for(int i=0;i<scriptObj.count();i++) delete scriptObj[i];
    for(int i=0;i<ModChans.count();i++) delete ModChans[i];
    if(modbus != NULL) delete(modbus);
    for(int i=0;i<plots.count();i++) delete plots[i];
    delete tcp;
    delete ui;
    this->deleteLater();
}

/*! \brief ControlPanel::reject
 * This function is called when the control panel is closed. It will
 * check if the system is shutdown and if so, it will warn the user
 * that all voltages are disabled and all RF drive levels are set to 0.
 * The user will be asked to confirm if they want to exit.
 */
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

// This slot is called when a control issues a change event with the
// name of a script to execute as a result of the change. This allows
// a control panel object to create an event when the user changes a
// value.
void ControlPanel::controlChange(QString scriptName)
{
    foreach(Script *scrpt, scriptObj)
    {
        if(scrpt->scriptName == scriptName)
        {
            scrpt->scriptCall = "";
            QObject* callingObject = sender();
            Ccontrol *cc = qobject_cast<Ccontrol*>(callingObject);
            if(cc != NULL) scrpt->scriptCall = cc->scriptCall;
            Table *tbl = qobject_cast<Table*>(callingObject);
            if(tbl != NULL) scrpt->scriptCall = tbl->scriptCall;
            scrpt->run();
        }
    }
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

/*! \brief ControlPanel::isComms
 * This function checks if all the MIPS systems are connected.
 * It returns true if all systems are connected, otherwise false.
 */
bool ControlPanel::isComms(void)
{
    if(Systems.count() == 0) return false;
    for(int i=0;i<Systems.count();i++)
    {
        if(!Systems[i]->isConnected()) return false;
    }
    return true;
}

/*! \brief ControlPanel::popupHelp
 * This function is called when the user right clicks on the control panel.
 * It will display a popup menu with options for help, saving the control
 * panel, and other actions.
 */
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

/*! \brief ControlPanel::slotComments
 * This function is called when the user selects the Comments option
 * from the popup menu. It will display a dialog box where the user
 * can enter comments about the control panel.
 */
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

/*! \brief ControlPanel::slotGeneralHelp
 * This function is called when the user selects the General help option
 * from the popup menu. It will display the general help file.
 */
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
                            if(cctrl->Ctype == "Button")
                            {
                                resList[8] = QString::number(cctrl->pbButton->x());
                                resList[9] = QString::number(cctrl->pbButton->y());
                            }
                            else
                            {
                                resList[8] = QString::number(cctrl->frmCc->x());
                                resList[9] = QString::number(cctrl->frmCc->y());
                            }
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
                            resList[4] = QString::number(tab->x());
                            resList[5] = QString::number(tab->y());
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
            else streamOut << resList.join(",") + "\n";
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

// This Update function is called by the MIPS class. This function will read all the controls
// from the attached MIPS system and update the control panel with the values.
void ControlPanel::Update(void)
{
    if(updateState == 0) UpdateStateMachine();
}

/*! \brief UpdateStateMachine is the main state machine for the control panel.
 *
 * This function is called by the MIPS class to update the control panel.
 * It will read all the controls from the attached MIPS system and update the
 * control panel with the values. It also handles the MIPScomms dialog and
 * other control panel functions.
 */
void ControlPanel::UpdateStateMachine(void)
{
    QMessageBox msgBox;
    QString res;
    int i,j,k;

    // Make sure the UpdateSemaphore is avaliable, if not exit
    //if(UpdateSemaphore.available()==0) return;
    //MySemaphoreLocker locker(&UpdateSemaphore, 1);

    switch (updateState)
    {
    case 0:
        if(++updateCount > skipCount) updateCount = 1;
        if(firstCall)
        {
            foreach(Script *scrpt, scriptObj)
            {
                if(scrpt->scriptName == "StartUp") scrpt->run();
            }
            firstCall = false;
        }
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
        if(scriptconsole!=NULL) scriptconsole->UpdateStatus();
        if(UpdateStop) return;
        // loop so it can be called from the MIPScomms dialog
        for(i=0;i<TC.count();i++) if(TC[i]->Downloading) return;
        // This flag is set in a pushbutton event, this code
        // will start the MIPScomms dialog if its not already running.
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
        isUpdating = true;
        res.clear();
        for(i=0;i<Systems.count();i++)
        {
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
        break;
    case 1:
        for(i=0;i<ScripButtons.count();i++) ScripButtons[i]->Update();
        // For each MIPS system present if there are RF channels for the selected
        // MIPS system then read all values using the read all commands to speed up the process.
        for(i=0;i<Systems.count();i++)
        {
            for(j=0;j<RFchans.count();j++) if(RFchans[j]->comms == Systems[i])
                {
                    // Read all the RF parameters
                    QString RFallRes = Systems[i]->SendMess("GRFALL\n");
                    QStringList RFallResList = RFallRes.split(",");
                    if(RFallResList.count() < 1)
                    {
                        // Here with group read error so process one at a time
                        for(k=0;k<RFchans.count();k++) if(RFchans[k]->comms == Systems[i]) RFchans[k]->Update();
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
                            }
                    }
                    break;
                }
        }
        break;
    case 2:
        for(i=0;i<Systems.count();i++)
        {
            for(j=0;j<RFCchans.count();j++) if(RFCchans[j]->comms == Systems[i])
                {
                    // Read all the RF parameters
                    QString RFallRes = Systems[i]->SendMess("GRFALL\n");
                    QStringList RFallResList = RFallRes.split(",");
                    if(RFallResList.count() < 1)
                    {
                        // Here with group read error so process one at a time
                        for(k=0;k<RFCchans.count();k++) if(RFCchans[k]->comms == Systems[i]) RFCchans[k]->Update();
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
                            }
                    }
                    break;
                }
        }
        break;
    case 3:
        for(i=0;i<ADCchans.count();i++)    ADCchans[i]->Update();
        for(i=0;i<DACchans.count();i++)    DACchans[i]->Update();
        // For each MIPS system present if there are DCBchannels for the selected
        // MIPS system then read all setpoints and values using the read all
        // commands to speed up the process.
        // Updated March 30, 2025
        // The readbacks are read on every update cycle, the setpoints are read based on the
        // SKIPCOUNT, this is done to reduce traffic.
        for(i=0;i<Systems.count();i++)
        {
            for(j=0;j<DCBchans.count();j++) if(DCBchans[j]->comms == Systems[i])
                {
                    // Read all the setpoints and readbacks and parse the strings
                    QString VspRes;
                    if(updateCount == 1)
                    {
                        VspRes = Systems[i]->SendMess("GDCBALL\n");
                    }
                    QStringList VspResList = VspRes.split(",");
                    if(VspResList.count() <= 1) VspResList.clear();
                    QString VrbRes = Systems[i]->SendMess("GDCBALLV\n");
                    QStringList VrbResList = VrbRes.split(",");
                    if(VrbResList.count() <= 1) VrbResList.clear();
                    if((VspResList.count() == 0) && (VrbResList.count() == 0))
                    {
                        // Here with group read error so process one at a time.
                        // This support old MIPS firmware that did not have the DCB group commands
                        for(k=0;k<DCBchans.count();k++) if(DCBchans[k]->comms == Systems[i]) DCBchans[k]->Update();
                    }
                    else if((VspResList.count() == 0))
                    {
                        // build strings and update the readbacks for all channels that use this comm port
                        for(k=0;k<DCBchans.count();k++) if(DCBchans[k]->comms == Systems[i])
                            {
                                DCBchans[k]->Update("," + VrbResList[DCBchans[k]->Channel - 1]);
                            }
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
                    break;
                }
        }
        break;
    case 4:
        for(i=0;i<DCBoffsets.count();i++)       DCBoffsets[i]->Update();
        for(i=0;i<DCBenables.count();i++)       DCBenables[i]->Update();
        for(i=0;i<DIOchannels.count();i++)      DIOchannels[i]->Update();
        break;
    case 5:
        for(i=0;i<ESIchans.count();i++)         ESIchans[i]->Update();
        for(i=0;i<ARBchans.count();i++)         ARBchans[i]->Update();
        break;
    case 6:
        if(Ccontrols.count() > updateIndex) while(true)
        {
            Ccontrols[updateIndex++]->Update();
            if(updateIndex >= Ccontrols.count()) break;
            if(updateIndex > 0 && updateIndex % 10 == 0)
            {
                QTimer::singleShot(10, this, &ControlPanel::UpdateStateMachine);
                return;
            }
        }
        break;
    case 7:
        for(i=0;i<Cpanels.count();i++)          Cpanels[i]->Update();
        break;
    case 8:
        for(i=0;i<rfa.count();i++)              rfa[i]->Update();
        for(i=0;i<devices.count();i++)          devices[i]->Update();
        for(i=0;i<TC.count();i++)               for(j=0;j<TC[i]->TG->EC.count();j++) TC[i]->TG->EC[j]->Update();
        for(i=0;i<comp.count();i++)             comp[i]->Update();
        for(i=0;i<ModChans.count();i++)         ModChans[i]->Update();
        isUpdating = false;
        LogDataFile();
        updateState = 0;
        return;
    default:
        updateState = 0;
        return;
    }
    // Advance to next state and restart this function after a delay
    updateState++;
    updateIndex=0;
    QTimer::singleShot(10, this, &ControlPanel::UpdateStateMachine);
}

/*! \brief Save is the main save method for the control panel.
 *
 * This function saves the control panel settings to a file.
 * The file format is a text file with a header and the control
 * panel settings.
 */
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
            if(cp->Tables.count() > 0)
            {
                stream << "Tables parameters\n";
                for(int i=0; i<cp->Tables.count(); i++) stream << cp->Tables[i]->Report() + "\n";
                stream << "TablesEnd\n";
            }
            if(cp->Sliders.count() > 0)
            {
                stream << "Slider parameters\n";
                for(int i=0; i<cp->Sliders.count(); i++) stream << cp->Sliders[i]->Report() + "\n";
                stream << "SlidersEnd\n";
            }
        }
        file.close();
        UpdateHoldOff = 1;
        return "Settings saved to " + Filename;
    }
    UpdateHoldOff = 1;
    return "Can't open file!";
}

/*! \brief Load is the main load method for the control panel.
 *
 * This function loads the control panel settings from a file.
 * The file format is a text file with a header and the control
 * panel settings.
 */
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
                if(resList[0] == "Tables parameters") while(true)
                {
                    line = stream.readLine();
                    if(line.isNull()) break;
                    if(line.contains("TablesEnd")) break;
                    for(int j=0;j<cp->Tables.count();j++) if(cp->Tables[j]->SetValues(line)) break;
                }
                if(resList[0] == "Slider parameters") while(true)
                {
                    line = stream.readLine();
                    if(line.isNull()) break;
                    if(line.contains("SlidersEnd")) break;
                    for(int j=0;j<cp->Sliders.count();j++) if(cp->Sliders[j]->SetValues(line)) break;
                }
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
    UpdateHoldOff = 1;
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
void ControlPanel::pbSD(void)
{
    ShutdownFlag = true;
}

// System enable
void ControlPanel::pbSE(void)
{
    RestoreFlag = true;
}

/*! \brief Save the settings to a file.
 *
 * This function is called when the user clicks the Save button
 * on the control panel. It opens a file dialog to select the file
 * to save the settings to and then calls the Save method.
 */
void ControlPanel::pbSave(void)
{
    QFileDialog fileDialog;

    if(properties != NULL) fileDialog.setDirectory(properties->MethodesPath);
    QString fileName = fileDialog.getSaveFileName(this, tr("Save to Settings File"),"",tr("Settings (*.settings);;All files (*.*)"));
    if(fileName == "") return;
    statusBar->showMessage(Save(fileName), 5000);
}

/*! \brief Load the settings from a file.
 *
 * This function is called when the user clicks the Load button
 * on the control panel. It opens a file dialog to select the file
 * to load the settings from and then calls the Load method.
 */
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

QString ControlPanel::MakeFileNameUnique(QString fileName)
{
    QFileInfo fileInfo(fileName);
    QString path = fileInfo.path();
    QString baseName = fileInfo.baseName();

    if (!QDir(path).exists()) return "Error: Path does not exist:" + path;

    QString suffix = fileInfo.suffix();
    if(suffix.isEmpty()) suffix = "0000";
    if(suffix.length()!=4)
    {
        baseName += "." + suffix;
        suffix = "0000";
    }
    for(int i=0;i<4;i++)
    {
        if(!suffix[i].isDigit())
        {
            baseName += "." + suffix;
            suffix = "0000";
            break;
        }
    }
    // suffix is now .xxxx where xxxx is a number, scan this number and used it to
    // initalize counter
    int counter = suffix.right(4).toInt();
    QString uniqueFileName;
    do {
        uniqueFileName = QString("%1/%2.%3").arg(path, baseName, QString::number(counter, 'f', 0).rightJustified(4, '0'));
        counter++;
        if (counter > 9999) return "Error: Too many attempts to create unique filename.";
    } while (QFile::exists(uniqueFileName));

    return uniqueFileName;
}

// Used by scripting system, the record should be \n terminated, this is not checked!
QString ControlPanel::AppendToFile(QString fileName, QString record)
{
    QFile file(fileName);
    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        // Open in Append mode and Text mode
        QTextStream out(&file);
        out << record; // Append the text
        file.close();
        return "";
    }
    else return "Error opening file for appending:" + file.errorString();
}

QString ControlPanel::ReadFile(QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        QString res = stream.readAll();
        file.close();
        return res;
    }
    return "";
}

QString ControlPanel::ReadFileLine(QString fileName, int line)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        for(int i=0;i<line;i++)
        {
            file.readLine();
            if(file.atEnd())
            {
                file.close();
                return "Error, line number not found!";
            }
        }
        QString record;
        record = file.readLine();
        file.close();
        return record;
    }
    else return "Error opening file.";
}

// The following functions are for the scripting system
QString ControlPanel::GetLine(QString MIPSname)
{
   Comms *cp =  FindCommPort(MIPSname,Systems);
   if(cp==NULL) return "";
   cp->waitforline(500);
   return cp->getline();
}

bool ControlPanel::SendCommand(QString MIPSname, QString message)
{
   Comms *cp =  FindCommPort(MIPSname,Systems);
   if(cp==NULL) return false;
   return cp->SendCommand(message);
}

bool ControlPanel::SendString(QString MIPSname, QString message)
{
   Comms *cp =  FindCommPort(MIPSname,Systems);
   if(cp==NULL) return false;
   return cp->SendString(message);
}

QString ControlPanel::SendMess(QString MIPSname, QString message)
{
    Comms *cp =  FindCommPort(MIPSname,Systems);
    if(cp==NULL) return "MIPS not found!";
    return cp->SendMess(message);
}

void ControlPanel::SystemEnable(void)
{
    RestoreFlag = true;
}

void ControlPanel::SystemShutdown(void)
{
    ShutdownFlag = true;
}

bool ControlPanel::isShutDown(void)
{
    return(SystemIsShutdown);
}

void ControlPanel::Acquire(QString filePath)
{
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
  if(TC.count() > 0) return(TC.last()->AD->Acquiring);
  return(false);
}

void ControlPanel::DismissAcquire(void)
{
  if(TC.count() > 0)  TC.last()->AD->Dismiss();
}

void ControlPanel::msDelay(int ms)
{
    QThread::msleep(ms);
}

void ControlPanel::statusMessage(QString message)
{
    statusBar->showMessage(message);
}

void ControlPanel::popupMessage(QString message)
{
    QMessageBox msgBox;

    msgBox.setText(message);
    msgBox.setInformativeText("");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

bool ControlPanel::popupYesNoMessage(QString message)
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

QString ControlPanel::popupUserInput(QString title, QString message)
{
    bool ok;

    QString text = QInputDialog::getText(this, title, message, QLineEdit::Normal,QString(), &ok);
    if(!ok) text="";
    return text;
}

/*! \brief sysUpdating is used to check if the system is updating.
 *
 * This function will return true if the system is updating, otherwise false.
 * It will traverse the parent control panels to find the top level control panel
 * and check its isUpdating flag.
 */
bool ControlPanel::sysUpdating(void)
{
    ControlPanel *cp=this;

    while(cp->parentCP != NULL) cp = cp->parentCP;
    return cp->isUpdating;
}

bool ControlPanel::UpdateHalted(bool stop)
{
    ControlPanel *cp=this;

    while(cp->parentCP != NULL) cp = cp->parentCP;
    cp->UpdateStop = stop;
    //cp->ProcessEvents = !stop;
    return cp->UpdateStop;
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

/*! \brief Command is the main command processing function for the control panel.
 *
 * This function processes commands sent to the control panel. It will
 * process global commands first, then it will process commands for the
 * various channels and controls in the control panel.
 */
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
   if(cmd.trimmed().toUpper().startsWith("SIZE"))
   {
       resList = cmd.split(",");
       if(resList.count() == 3)
       {
           this->setFixedSize(resList[1].toInt(),resList[2].toInt());
           statusBar->setGeometry(0,resList[2].toInt()-18,resList[1].toInt(),18);
           return "\n";
       }
   }
   // Send the command string to all the controls for someone to process!
   for(i=0;i<Systems.count();i++)
   {
       // Valid commands support asycn messages sent from MIPS
       // .enableAsync=true|false
       // .getMessage
       if(cmd.trimmed().startsWith(Systems[i]->MIPSname))
       {
           if(cmd.trimmed() == Systems[i]->MIPSname + ".getMessage")
           {
               return Systems[i]->getAsyncMessage() + "\n";
           }
           resList = cmd.split("=");
           if(resList.count() == 2)
           {
                if(resList[0] == Systems[i]->MIPSname + ".enableAsync")
                {
                    if(resList[1].toUpper() == "TRUE") Systems[i]->enableAsyncMessages(true);
                    else if(resList[1].toUpper() == "FALSE") Systems[i]->enableAsyncMessages(false);
                    else return("?");
                    return("\n");
                }
           }
        }
   }
   for(i=0;i<sl.count();i++)          if((res = sl[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<tm.count();i++)          if((res = tm[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
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
   for(i=0;i<ScripButtons.count();i++)if((res = ScripButtons[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<Cpanels.count();i++)     if((res = Cpanels[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<CPbuttons.count();i++)   if((res = CPbuttons[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<ModChans.count();i++)    if((res = ModChans[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<plotData.count();i++)    if((res = plotData[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<rfa.count();i++)         if((res = rfa[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<scriptObj.count();i++)   if((res = scriptObj[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<Tables.count();i++)      if((res = Tables[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<Sliders.count();i++)     if((res = Sliders[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<extProcs.count();i++)    if((res = extProcs[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   for(i=0;i<TC.count();i++)
   {
       if((res = TC[i]->TG->ProcessCommand(cmd)) != "?") return(res + "\n");
       for(j=0;j<TC[i]->TG->EC.count();j++) if((res = TC[i]->TG->EC[j]->ProcessCommand(cmd)) != "?") return(res + "\n");
   }
   for(i=0;i<comp.count();i++) if((res = comp[i]->ProcessCommand(cmd)) != "?") return(res + "\n");
   // Process groupbox commands, support color and hide commands
   for(i=0;i<GroupBoxes.count();i++)
   {
       QStringList cmdList = cmd.split("=");
       if(cmdList[0] == GroupBoxes[i]->objectName()+".color")
       {
            // The following code sets the background color of the group box
            QPalette palette = GroupBoxes[i]->palette();
            palette.setColor(QPalette::Window, cmdList[1]); // A light green
            GroupBoxes[i]->setPalette(palette);
            GroupBoxes[i]->setAutoFillBackground(true);
            return "\n";
       }
       if(cmdList[0] == GroupBoxes[i]->objectName()+".hide")
       {
           if(cmdList[1].toUpper() == "TRUE") GroupBoxes[i]->setVisible(false);
           else if(cmdList[1].toUpper() == "FALSE") GroupBoxes[i]->setVisible(true);
           else return "?\n";
           return "\n";
       }
   }
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
        //fileName = QFileDialog::getSaveFileName(nullptr, Title,"",Ext + " (*." + Ext + ");;All files (*.*)");
        fileName = QFileDialog::getSaveFileName(this, Title,"", Ext);
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

void ControlPanel::setValue(const QString &key, const QVariant &value)
{
    ControlPanel *cp=this;

    // Find the top level control panel
    while(cp->parentCP != NULL) cp = cp->parentCP;

    if (key.isEmpty()) {
        //qWarning() << "Cannot save key-value pair: Key is empty.";
        return;
    }

    // Insert or update the value in the QMap
    cp->m_storage.insert(key, value);
    //qDebug() << "Saved key to QMap:" << key << "with value:" << value;
}

QVariant ControlPanel::getValue(const QString &key)
{
    ControlPanel *cp=this;

    // Find the top level control panel
    while(cp->parentCP != NULL) cp = cp->parentCP;

    if (key.isEmpty()) {
        //qWarning() << "Cannot retrieve key-value pair: Key is empty.";
        return "no key";
    }

    // QMap::value() returns the value associated with the key, or defaultValue if the key is not found.
    QVariant value = cp->m_storage.value(key, "no key");
    //qDebug() << "Retrieved key from QMap:" << key << "with value:" << value;
    return value;
}

/*! \brief CreateProcess creates an external process control.
 *
 * This function creates an external process control with the given name
 * and program path. If a control with the same name already exists, it
 * returns false. Otherwise, it creates the control and returns true.
 */
bool ControlPanel::CreateProcess(QString name, QString program)
{
    // If this name is in use then exit and return false
    for(int i=0;i<extProcs.count();i++)
    {
        if(extProcs[i]->name == name) return false;
    }
    extProcs.append(new extProcess(this,name,program));
    connect(extProcs.last(),SIGNAL(DialogClosed(QString)),this,SLOT(slotExtProcessClosed(QString)));
    connect(extProcs.last(),SIGNAL(change(QString)),this,SLOT(slotExternalProcessChange(QString)));
    return true;
}

void ControlPanel::slotExtProcessClosed(QString name)
{
    // It is safer to iterate backward when removing items by index.
    // This ensures that when an item is removed (and the list shifts),
    // the index 'i' still points to the next correct item in the original list.

    // Start from the end of the list and go down to 0
    for(int i = extProcs.count() - 1; i >= 0; i--)
    {
        // Check if the name matches the process we are looking for
        if(extProcs[i]->name == name)
        {
            // CRITICAL STEP 1: Delete the object pointed to by the pointer (prevent memory leak)
            delete extProcs[i];

            // CRITICAL STEP 2: Remove the pointer from the QList (shrink the list)
            extProcs.removeAt(i);

            // DO NOT return here if you want to remove all potential duplicates.
            // Continue the loop to check the rest of the list.
        }
    }
}

void ControlPanel::slotExternalProcessChange(QString script)
{
    // Call the scripting system to process this script
    controlChange(script);
}

QString  ControlPanel::ZMQ(QString command)
{
    return(zmq.command(command));
}












// *************************************************************************************************
// Slider class implementation  *********************************************************************
// *************************************************************************************************


Slider::Slider(QWidget *parent, QString name, QString orentation, int min, int max, int width, int x, int y)
{
    p      = parent;
    Title  = name;
    X      = x;
    Y      = y;
    Orientation = orentation;
    Min    = min;
    Max    = max;
    Width  = width;
}

void Slider::Show(void)
{
    if(Orientation.toUpper().contains("H"))
    {
        slider = new QSlider(Qt::Horizontal,p);
        slider->setGeometry(X, Y, Width, 21);
    }
    else
    {
        slider = new QSlider(Qt::Vertical,p);
        slider->setGeometry(X, Y, 21, Width);
    }
    if(slider == NULL) return;
    slider->setMinimum(Min);
    slider->setMaximum(Max);
    slider->setValue(0);
    QString res;
    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    slider->setToolTip(Title);
    slider->show();
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(sliderChange(int)));
}

QString Slider::ProcessCommand(QString cmd)
{
    QString res;

    if(slider == NULL) return "?";
    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(!cmd.startsWith(res)) return "?";
    QStringList cmdList = cmd.split("=");
    if(cmdList.count() == 1)
    {
        if(cmdList[0] == res) return QString::number(slider->value());
    }
    if(cmdList.count() == 2)
    {
        if(cmdList[0] == res)
        {
            int val = cmdList[1].toInt();
            if(val < Min || val > Max) return "?";
            slider->setValue(val);
            return "";
        }
        if(cmdList[0] == res + ".script")
        {
            scriptName = cmdList[1].trimmed();
            return "";
        }
        if(cmdList[0] == res + ".scriptCall")
        {
            scriptCall = cmdList[1].trimmed();
            return "";
        }
        if(cmdList[0] == res + ".toolTip")
        {
            slider->setToolTip(cmdList[1].trimmed());
            return "";
        }
        if(cmdList[0] == res + ".color")
        {
            slider->setStyleSheet("background-color: " + cmdList[1].trimmed());
            return "";
        }
    }
    return "?";
}

QString Slider::Report(void)
{
    QString res;

    if(slider == NULL) return "";
    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    res += "," + QString::number(slider->value());
    return res;
}

bool Slider::SetValues(QString strVals)
{
    QStringList resList;
    QString res;
    if(slider == NULL) return false;
    res.clear();
    if(p->objectName() != "") res = p->objectName() + ".";
    res += Title;
    if(!strVals.startsWith(res)) return false;
    resList = strVals.split(",");
    if(resList.count() != 2) return false;
    if(resList[0] != res) return false;
    int val = resList[1].toInt();
    if(val < Min || val > Max) return false;
    slider->setValue(val);
    return true;
 }

void Slider::sliderChange(int value)
{
    Q_UNUSED(value);
    if(!scriptName.isEmpty()) emit change(scriptName);
}
