// =============================================================================
// connection.cpp
//
// MIPS connection management — serial/TCP discovery, connect, disconnect, and
// multi-system setup. Extracted from mips.cpp during Phase 3 refactoring.
//
// Depends on:  mips.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "mips.h"
#include "ui_mips.h"
#include "comms.h"
#include "settingsdialog.h"
#include "properties.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QMessageBox>

/*! \brief delay
 * Blocking one-second delay that keeps the event loop alive.
 */
void delay()
{
    QElapsedTimer t;
    t.start();
    while (t.elapsed() < 1000)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

/*! \brief MIPS::MIPSsetup
 * Queries the connected MIPS for version, module configuration, and uptime, then
 * configures subsystem tab visibility and channel counts accordingly.
 */
void MIPS::MIPSsetup(void)
{
    QString res,ver;
    int numRF=0,numDCB=0,i;

    pgm->comms = comms;

    AddTab("ADC");
    AddTab("Digital IO");
    AddTab("DCbias");
    AddTab("RFdriver");
    AddTab("ARB");
    AddTab("Twave");
    AddTab("FAIMS");
    AddTab("Filament");
    AddTab("Pulse Sequence Generation");
    res = "Serial port: " + comms->serialPort()->portName() + "\n\n";
    ui->lblMIPSconfig->setFixedHeight(471);
    ui->lblMIPSconfig->clear();
    ui->lblMIPSconfig->setText(ui->lblMIPSconfig->text() + res);
    comms->SendString("\n");
    comms->SendCommand("ECHO,FALSE\n");
    ver = comms->SendMess("GVER\n");
    if(ver=="") return;  // Exit if we timeout, no MIPS comms

    res = comms->SendMess("ABOUT\n") + "\n";
    if(!res.contains("?"))
    {
        while(true)
        {
            comms->waitforline(100);
            QString line = comms->rb.getline();
            if(line == "") break;
            res += line + "\n";
        }
        ui->lblMIPSconfig->setText(ui->lblMIPSconfig->text() + res + "\n");
        numRF = res.count("RFdriver") * 2;
        numDCB = res.count("DCbias") * 8;
        i = res.count("ARB") * 8;
        if(i==0) RemoveTab("ARB");
        else AddTab("ARB");
        arb->SetNumberOfChannels(i);
        i = res.count("Twave");
        if(i==0) RemoveTab("Twave");
        else AddTab("Twave");
        i = res.count("FAIMS");
        if(i==0) RemoveTab("FAIMS");
        else AddTab("FAIMS");
        i = res.count("Filament");
        if(i==0) RemoveTab("Filament");
        else AddTab("Filament");
    }
    else
    {
        // Here if the connected device does not support About command,
        // This indicates its not a MIPS box so just print the version string
        ui->lblMIPSconfig->setText(ui->lblMIPSconfig->text() + ver + "\n\n");
        // Remove the unused tabs
        RemoveTab("ADC");
        RemoveTab("Digital IO");
        RemoveTab("DCbias");
        RemoveTab("RFdriver");
        RemoveTab("ARB");
        RemoveTab("Twave");
        RemoveTab("FAIMS");
        RemoveTab("Filament");
        RemoveTab("Pulse Sequence Generation");
    }
    res = comms->SendMess("THREADS\n") + "\n";
    if(!res.contains("?"))
    {
        while(true)
        {
            comms->waitforline(100);
            QString line = comms->rb.getline();
            if(line == "") break;
            res += line + "\n";
        }
        ui->lblMIPSconfig->setText(ui->lblMIPSconfig->text() + res + "\n");
    }
    res = comms->SendMess("UPTIME\n") + "\n";
    if(!res.contains("?")) ui->lblMIPSconfig->setText(ui->lblMIPSconfig->text() + res);
    res = "CPU temp: " + comms->SendMess("CPUTEMP\n") + "\n";
    if(!res.contains("?")) ui->lblMIPSconfig->setText(ui->lblMIPSconfig->text() + res);
    rfdriver->SetNumberOfChannels(numRF);
    dcbias->SetNumberOfChannels(numDCB);
}

/*! \brief MIPS::MIPSconnect
 * Slot: attempts a serial or TCP connection using the current settings and host name,
 * then calls MIPSsetup on success.
 */
void MIPS::MIPSconnect(void)
{
    comms->setSettings(settings->settings());
    comms->setProperties(properties);
    comms->setHost(ui->comboMIPSnetNames->currentText());
    if(comms->ConnectToMIPS())
    {
       Systems.clear();
       Systems << comms;
       console->setEnabled(true);
       console->setLocalEchoEnabled(settings->settings().localEchoEnabled);
       ui->lblMIPSconnectionNotes->setHidden(true);
       MIPSsetup();
    }
}

/*! \brief MIPS::MIPSsearch
 * Slot: scans available ports and network names for MIPS systems and connects to all found.
 */
void MIPS::MIPSsearch(void)
{
    QMessageBox *msg = new QMessageBox();
    msg->setText("Searching for MIPS system(s)...");
    msg->setStandardButtons(QMessageBox::NoButton);
    msg->setWindowModality(Qt::NonModal);
    msg->show();

    settings->fillPortsParameters();
    settings->fillPortsInfo();
    FindMIPSandConnect();
    msg->hide();
}

/*! \brief MIPS::FindAllMIPSsystems
 * Enumerates all reachable MIPS/AMPS systems and populates the Systems list and combo box.
 */
void MIPS::FindAllMIPSsystems(void)
{
    Comms *cp;

    disconnect(ui->comboSystems, &QComboBox::currentIndexChanged, nullptr, nullptr);
    Systems.clear();
    // If there are a defined list of net names or IP addresses
    // then use them and do not search for USB connected systems
    if(ui->comboMIPSnetNames->count() > 0)
    {
        for(int j=0;j<ui->comboMIPSnetNames->count();j++)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            cp = new Comms(settings,"",ui->statusBar);
            cp->setHost(ui->comboMIPSnetNames->itemText(j));
            if(cp->ConnectToMIPS())
            {
               Systems << (cp);
            }
        }
    }
    else
    {
        int i = settings->numberOfPorts();
        settings->fillPortsInfo();
        for(int j=0;j<i;j++)
        {
          if(settings->getPortName(j).contains("Bluetooth-Incoming-Port")) continue;
          ui->statusBar->showMessage("Trying: " + settings->getPortName(j));
          QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
          cp = new Comms(settings,"",ui->statusBar);
          cp->setProperties(properties);
          if(cp->isMIPS(settings->getPortName(j)))
          {
            delay();
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            cp->setHost("");
            cp->SendString("ECHO,FALSE\n");
            if(cp->ConnectToMIPS())
            {
               Systems << (cp);
            }
          }
          else if(properties != nullptr)
          {
              if(properties->SearchAMPS)
              {
                  if(cp->isAMPS(settings->getPortName(j),properties->AMPSbaud))
                  {
                    delay();
                    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                    cp->setHost("");
                    if(cp->ConnectToMIPS())
                    {
                       Systems << (cp);
                            }
                  }
              }
          }
        }
    }
    ui->comboSystems->clear();
    for(int j=0;j<Systems.count();j++)
    {
        ui->comboSystems->addItem(Systems.at(j)->MIPSname);
    }
    if(ui->comboSystems->count() > 1)
    {
        ui->comboSystems->setVisible(true);
        ui->lblSystems->setVisible(true);
    }
}

/*! \brief MIPS::FindMIPSandConnect
 * Finds all MIPS systems, connects to the first one, and starts MIPSsetup.
 */
void MIPS::FindMIPSandConnect(void)
{
  ui->pbSearchandConnect->setDown(false);
  QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  FindAllMIPSsystems();
  if(ui->comboSystems->count() > 0)
  {
      delay();
      comms = Systems.at(0);
      console->setEnabled(true);
      console->setLocalEchoEnabled(settings->settings().localEchoEnabled);
      ui->lblMIPSconnectionNotes->setHidden(true);
      MIPSsetup();
      connect(ui->comboSystems, &QComboBox::currentIndexChanged, this, [this](int){ UpdateSystem(); });
  }
  else ui->statusBar->showMessage(tr("Can't find MIPS system!"));
  return;
}

/*! \brief MIPS::MIPSdisconnect
 * Slot: restores all tabs, disconnects all systems, and hides the multi-system combo box.
 */
void MIPS::MIPSdisconnect(void)
{
    AddTab("ADC");
    AddTab("Digital IO");
    AddTab("DCbias");
    AddTab("RFdriver");
    AddTab("ARB");
    AddTab("Twave");
    AddTab("FAIMS");
    AddTab("Filament");
    AddTab("Pulse Sequence Generation");
    comms->DisconnectFromMIPS();
    for(int j=0;j<Systems.count();j++)
    {
        Systems.at(j)->DisconnectFromMIPS();
    }
    ui->comboSystems->setVisible(false);
    ui->lblSystems->setVisible(false);

    ui->lblMIPSconfig->setText("");
    ui->lblMIPSconnectionNotes->setHidden(false);
}

/*! \brief MIPS::UpdateSystem
 * Slot: switches the active Comms pointer to the system selected in the combo box
 * and re-runs MIPSsetup.
 */
void MIPS::UpdateSystem(void)
{
   if(Systems.count() == 0) return;
   comms = Systems.at(ui->comboSystems->currentIndex());
   MIPSsetup();
}
