// =============================================================================
// properties.cpp
//
// Implements the Properties dialog — persistent application settings stored
// in a CSV-format Properties.mips file. Saves to / loads from the application
// directory first, falling back to the user home directory.
//
// Settings managed:
//   DataFilePath, MethodesPath, ScriptPath, LoadControlPanel, FileName,
//   MinMIPS, UpdateSecs, LogFile, SysFontSize, AMPSbaud, SearchAMPS,
//   AutoFileName, AutoConnect, AutoRestore, MIPS_TCPIP (list)
//
// setSystemFontSize() applies the font globally on macOS (point size) and
// Windows/Linux (Tahoma at the specified size).
//
// Depends on:  ui_properties.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "properties.h"
#include "ui_properties.h"

// Define the pointer (it starts as null)
Properties *pProps = nullptr;

// setSystemFontSize — applies a global application font at the given point
// size. Uses the system font on macOS; Tahoma on Windows/Linux.
void setSystemFontSize(int fs)
{
#if defined(Q_OS_MAC)
    QFont font = QApplication::font();
    font.setPointSize(fs);
    QApplication::setFont(font);
#else
    QFont _Font("Tahoma", fs);
    QApplication::setFont(_Font);
#endif
}

// Properties — constructor. Sets default field values, attempts to load
// Properties.mips from the application directory then the home directory,
// calls UpdateVars() to apply the loaded settings, and wires button signals.
Properties::Properties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Properties)
{
    ui->setupUi(this);

    HomePath        = QDir::homePath();
    ApplicationPath = QApplication::applicationFilePath();

    // Set defaults in case the properties file cannot be loaded
    ui->leDataFilePath->setText(QDir::currentPath());
    ui->leMethodesPath->setText(QDir::currentPath());
    ui->leScriptPath->setText(QDir::currentPath());
    ui->comboTCPIPlist->clear();
#if defined(Q_OS_MAC)
    sysFontSize = "15";
    ui->leSysFontSize->setText("15");
#else
    sysFontSize = "8";
    ui->leSysFontSize->setText("8");
#endif

    // Load from app directory first, fall back to home directory
    if(!Load(ApplicationPath + "/Properties.mips"))
        Load(HomePath + "/Properties.mips");
    UpdateVars();

    connect(ui->pbDataFilePath,     SIGNAL(pressed()), this, SLOT(slotDataFilePath()));
    connect(ui->pbMethodesPath,     SIGNAL(pressed()), this, SLOT(slotMethodesPath()));
    connect(ui->pbScriptPath,       SIGNAL(pressed()), this, SLOT(slotScriptPath()));
    connect(ui->pbLoadControlPanel, SIGNAL(pressed()), this, SLOT(slotLoadControlPanel()));
    connect(ui->pbClear,            SIGNAL(pressed()), this, SLOT(slotClear()));
    connect(ui->pbOK,               SIGNAL(pressed()), this, SLOT(slotOK()));
    connect(ui->pbLogFile,          SIGNAL(pressed()), this, SLOT(slotLogFile()));

    // When created, point the global pointer to THIS instance
    pProps = this;
}

// ~Properties — destructor. Releases the UI form.
Properties::~Properties()
{
    delete ui;
}

// Log — appends a timestamped message to the log file (if one is configured).
// Empty messages and an empty LogFile path are silently ignored.
void Properties::Log(QString Message)
{
    if(Message.isEmpty() || LogFile.isEmpty()) return;
    QFile file(LogFile);
    if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << Message.replace("\n", "") + "," + QDateTime::currentDateTime().toString() + "\n";
        file.close();
    }
}

// UpdateVars — reads all UI field values into the public member variables
// and applies the system font size.
void Properties::UpdateVars(void)
{
    DataFilePath     = ui->leDataFilePath->text();
    MethodesPath     = ui->leMethodesPath->text();
    ScriptPath       = ui->leScriptPath->text();
    LoadControlPanel = ui->leControlPanel->text();
    FileName         = ui->leFileName->text();
    LogFile          = ui->leLogFile->text();
    MinMIPS          = ui->leMinMIPS->text().toInt();
    UpdateSecs       = ui->leUpdateSecs->text().toFloat();
    sysFontSize      = ui->leSysFontSize->text();
    AMPSbaud         = ui->leAMPSbaud->text();
    SearchAMPS       = ui->chkSearchAMPS->isChecked();
    AutoConnect      = ui->chkAutoConnect->isChecked();
    AutoRestore      = ui->chkAutoRestore->isChecked();
    AutoFileName     = ui->chkAutoFileName->isChecked();
    ScrollEdit       = ui->chkEnableScrollChange->isChecked();
    ControlPanelEdit = ui->chkEnableControlEdit->isChecked();
    LassoZoom        = ui->chkEnableLasso->isChecked();
    MIPS_TCPIP.clear();
    for(int i = 0; i < ui->comboTCPIPlist->count(); i++)
        MIPS_TCPIP.append(ui->comboTCPIPlist->itemText(i));
    setSystemFontSize(sysFontSize.toInt());
}

// slotDataFilePath — opens a directory browser and updates the data file path field.
void Properties::slotDataFilePath(void)
{
    ui->pbDataFilePath->setDown(false);
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select data file path"),
        ui->leDataFilePath->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->leDataFilePath->setText(dir);
}

// slotMethodesPath — opens a directory browser and updates the methodes path field.
void Properties::slotMethodesPath(void)
{
    ui->pbMethodesPath->setDown(false);
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select methode file path"),
        ui->leMethodesPath->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->leMethodesPath->setText(dir);
}

// slotScriptPath — opens a directory browser and updates the script path field.
void Properties::slotScriptPath(void)
{
    ui->pbScriptPath->setDown(false);
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select script file path"),
        ui->leScriptPath->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->leScriptPath->setText(dir);
}

// slotLoadControlPanel — opens a file dialog and updates the control panel
// configuration file path field.
void Properties::slotLoadControlPanel(void)
{
    ui->pbLoadControlPanel->setDown(false);
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Load Configuration from File"), "", tr("cfg (*.cfg);;All files (*.*)"));
    ui->leControlPanel->setText(fileName);
}

// slotLogFile — opens a save-file dialog and updates the log file path field.
void Properties::slotLogFile(void)
{
    ui->pbLogFile->setDown(false);
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setOptions(QFileDialog::DontConfirmOverwrite);
    QString fileName = dialog.getSaveFileName(this, tr("Select log file name"), "",
        tr("log (*.log);;All files (*.*)"), 0, QFileDialog::DontConfirmOverwrite);
    ui->leLogFile->setText(fileName);
}

// slotClear — removes all entries from the TCP/IP address list.
void Properties::slotClear(void)
{
    ui->comboTCPIPlist->clear();
}

// slotOK — commits UI values, saves the properties file, and hides the dialog.
void Properties::slotOK(void)
{
    UpdateVars();
    if(!Save(ApplicationPath + "/Properties.mips"))
        Save(HomePath + "/Properties.mips");
    this->hide();
}

// Save — writes all settings to a CSV-format .mips file.
// Returns true on success.
bool Properties::Save(QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QDateTime dateTime = QDateTime::currentDateTime();
        stream << "# MIPS properties data, " + dateTime.toString() + "\n";
        stream << "DataFilePath,"     + DataFilePath     + "\n";
        stream << "MethodesPath,"     + MethodesPath     + "\n";
        stream << "ScriptPath,"       + ScriptPath       + "\n";
        stream << "LoadControlPanel," + LoadControlPanel + "\n";
        stream << "FileName,"         + FileName         + "\n";
        stream << "MinMIPS,"          + QString::number(MinMIPS)      + "\n";
        stream << "UpdateSecs,"       + QString::number(UpdateSecs)   + "\n";
        stream << "LogFile,"          + LogFile          + "\n";
        stream << "SysFontSize,"      + sysFontSize      + "\n";
        stream << "AMPSbaud,"         + AMPSbaud         + "\n";
        stream << "SearchAMPS,"       + QString(SearchAMPS       ? "TRUE" : "FALSE") + "\n";
        stream << "AutoFileName,"     + QString(AutoFileName     ? "TRUE" : "FALSE") + "\n";
        stream << "AutoConnect,"      + QString(AutoConnect      ? "TRUE" : "FALSE") + "\n";
        stream << "AutoRestore,"      + QString(AutoRestore      ? "TRUE" : "FALSE") + "\n";
        stream << "ScrollEdit,"       + QString(ScrollEdit       ? "TRUE" : "FALSE") + "\n";
        stream << "ControlPanelEdit," + QString(ControlPanelEdit ? "TRUE" : "FALSE") + "\n";
        stream << "LassoZoom,"        + QString(LassoZoom        ? "TRUE" : "FALSE") + "\n";
        stream << "MIPS_TCPIP";
        for(int i = 0; i < MIPS_TCPIP.count(); i++) stream << "," + MIPS_TCPIP[i];
        stream << "\n";
        file.close();
        setSystemFontSize(sysFontSize.toInt());
        return true;
    }
    return false;
}

// Load — reads a CSV-format .mips properties file and populates the UI fields.
// Returns true on success.
bool Properties::Load(QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QString line;
        do
        {
            line = stream.readLine();
            QStringList r = line.split(",");
            if(r.count() == 2)
            {
                if(r[0] == "DataFilePath")           ui->leDataFilePath->setText(r[1]);
                else if(r[0] == "MethodesPath")      ui->leMethodesPath->setText(r[1]);
                else if(r[0] == "ScriptPath")        ui->leScriptPath->setText(r[1]);
                else if(r[0] == "LoadControlPanel")  ui->leControlPanel->setText(r[1]);
                else if(r[0] == "FileName")          ui->leFileName->setText(r[1]);
                else if(r[0] == "MinMIPS")           ui->leMinMIPS->setText(r[1]);
                else if(r[0] == "UpdateSecs")        ui->leUpdateSecs->setText(r[1]);
                else if(r[0] == "LogFile")           ui->leLogFile->setText(r[1]);
                else if(r[0] == "SysFontSize")       ui->leSysFontSize->setText(r[1]);
                else if(r[0] == "AMPSbaud")          ui->leAMPSbaud->setText(r[1]);
                else if(r[0] == "SearchAMPS")        ui->chkSearchAMPS->setChecked(r[1]   == "TRUE");
                else if(r[0] == "AutoFileName")      ui->chkAutoFileName->setChecked(r[1] == "TRUE");
                else if(r[0] == "AutoConnect")       ui->chkAutoConnect->setChecked(r[1]  == "TRUE");
                else if(r[0] == "AutoRestore")       ui->chkAutoRestore->setChecked(r[1]  == "TRUE");
                else if(r[0] == "ScrollEdit")        ui->chkEnableScrollChange->setChecked(r[1] == "TRUE");
                else if(r[0] == "ControlPanelEdit")  ui->chkEnableControlEdit->setChecked(r[1]  == "TRUE");
                else if(r[0] == "LassoZoom")         ui->chkEnableLasso->setChecked(r[1]        == "TRUE");
            }
            else if(r.count() >= 1 && r[0] == "MIPS_TCPIP")
            {
                ui->comboTCPIPlist->clear();
                for(int i = 1; i < r.count(); i++)
                    ui->comboTCPIPlist->addItem(r[i]);
            }
        } while(!line.isNull());
        file.close();
        return true;
    }
    return false;
}

