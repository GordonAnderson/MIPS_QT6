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
// Refactored:  May 2026
//   - Every Comms object is now moved to a dedicated QThread immediately
//     after construction so all serial I/O is confined to that thread.
//   - Systems list cleanup (clearSystems()) stops and deletes each thread
//     before removing Comms objects, preventing thread leaks on reconnect.
//   - delay() no longer calls processEvents(); QThread::msleep() is used
//     instead since Comms I/O no longer requires the event loop to be pumped.
//   - Direct access to comms->rb and comms->waitforline() from the UI thread
//     has been replaced with a getNextLine() helper that routes through the
//     thread-safe Send* interface.
//   - MIPSdisconnect() now fully tears down all threads before clearing the
//     Systems list.
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "mips.h"
#include "ui_mips.h"
#include "comms.h"
#include "console.h"
#include "settingsdialog.h"
#include "properties.h"
#include "program.h"
#include "arb.h"
#include "rfdriver.h"
#include "dcbias.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QThread>

// =============================================================================
// Internal helpers
// =============================================================================

/*! \brief delay
 * Blocking one-second delay.
 *
 * FIX: replaced QCoreApplication::processEvents() spin with QThread::msleep().
 * The Comms I/O layer now uses waitForReadyRead() internally so the event loop
 * does not need to be pumped during device probe delays.
 */
void delay()
{
    QThread::msleep(1000);
}

/*! \brief makeCommsOnThread
 * Creates a new Comms object, moves it to a fresh QThread, and starts the
 * thread. Returns both via output parameters so the caller can track the
 * thread for later cleanup.
 *
 * All Comms objects must be created this way so that serial I/O is confined
 * to the worker thread and never touches the UI thread's event loop.
 */
static Comms* makeCommsOnThread(SettingsDialog *settings,
                                QStatusBar    *statusBar,
                                QThread      *&outThread)
{
    Comms   *c = new Comms(settings, "", statusBar);
    QThread *t = new QThread();
    c->moveToThread(t);
    t->start();
    outThread = t;
    return c;
}

/*! \brief MIPS::clearSystems
 * Stops and deletes every thread in commsThreads, then clears both the
 * Systems and commsThreads lists.
 *
 * Must be called instead of Systems.clear() whenever the Systems list is
 * rebuilt so that worker threads are not leaked between reconnects.
 *
 * Note: comms may point into Systems. After clearSystems() the caller is
 * responsible for reassigning comms before using it.
 */
void MIPS::clearSystems()
{
    // DisconnectFromMIPS() now handles timer stop/disconnect on the
    // Comms thread via BlockingQueuedConnection internally.
    for(int j = 0; j < Systems.count(); j++)
    {
        Systems.at(j)->DisconnectFromMIPS();
    }
    for(int j = 0; j < commsThreads.count(); j++)
    {
        commsThreads.at(j)->quit();
        commsThreads.at(j)->wait();
        QCoreApplication::sendPostedEvents(Systems.at(j), QEvent::None);
        delete commsThreads.at(j);
    }
    for(int j = 0; j < Systems.count(); j++)
    {
        if(Systems.at(j) != primaryComms) Systems.at(j)->deleteLater();
    }
    Systems.clear();
    commsThreads.clear();
}

// =============================================================================
// getNextLine — thread-safe multi-line response drain
// =============================================================================

/*! \brief MIPS::getNextLine
 * Reads one additional unsolicited response line that MIPS sends after a
 * multi-line command (ABOUT, THREADS, etc.).
 *
 * FIX: replaces the pattern:
 *   comms->waitforline(100);
 *   QString line = comms->rb.getline();
 * which accessed rb directly from the UI thread — a data race against the
 * Comms worker thread.
 *
 * Instead we use SendMessage with an empty payload and a short timeout so
 * the request travels through the thread-safe Send* path. Because the Comms
 * thread already has pending data in the ring buffer from the previous
 * response, it returns the next buffered line without sending anything to
 * the device. If the ring buffer is empty after the timeout the function
 * returns an empty string, which the caller uses as the end-of-response
 * sentinel (matching the original logic).
 *
 * \note This relies on doSendMessage draining the ring buffer of whatever
 * is already buffered. If this proves unreliable a dedicated getBufferedLine()
 * slot should be added to Comms that locks the mutex, calls rb.getline(),
 * and returns the result — the same QWaitCondition pattern as the other
 * worker slots.
 */
QString MIPS::getNextLine(Comms *c)
{
    // waitforline with a short timeout then read whatever is buffered.
    // We invoke both steps on the Comms thread via a small lambda queued call
    // so rb is never touched from the UI thread.
    QString result;
    QMutex m;
    QWaitCondition wc;
    QMutexLocker lock(&m);

    QMetaObject::invokeMethod(c, [c, &result, &m, &wc]()
    {
        c->waitforline(100);
        QMutexLocker lk(&m);
        result = c->rb.getline();
        wc.wakeAll();
    }, Qt::QueuedConnection);

    wc.wait(&m);
    return result;
}

// =============================================================================
// MIPSsetup
// =============================================================================

/*! \brief MIPS::MIPSsetup
 * Queries the connected MIPS for version, module configuration, and uptime,
 * then configures subsystem tab visibility and channel counts accordingly.
 *
 * FIX: comms->waitforline() / comms->rb.getline() calls replaced with
 * getNextLine() so rb is never accessed from the UI thread.
 */
void MIPS::MIPSsetup(void)
{
    QString res, ver, line;
    int numRF = 0, numDCB = 0, i;

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
    if(ver == "") return;   // timeout — no MIPS comms

    res = comms->SendMess("ABOUT\n") + "\n";
    if(!res.contains("?"))
    {
        // FIX: was comms->waitforline(100) + comms->rb.getline() — direct rb
        // access from UI thread. Now routed through getNextLine().
        while(true)
        {
            line = getNextLine(comms);
            if(line == "") break;
            res += line + "\n";
        }
        ui->lblMIPSconfig->setText(ui->lblMIPSconfig->text() + res + "\n");
        numRF  = res.count("RFdriver") * 2;
        numDCB = res.count("DCbias")   * 8;
        i = res.count("ARB") * 8;
        if(i == 0) RemoveTab("ARB");    else AddTab("ARB");
        arb->SetNumberOfChannels(i);
        i = res.count("Twave");
        if(i == 0) RemoveTab("Twave");  else AddTab("Twave");
        i = res.count("FAIMS");
        if(i == 0) RemoveTab("FAIMS");  else AddTab("FAIMS");
        i = res.count("Filament");
        if(i == 0) RemoveTab("Filament"); else AddTab("Filament");
    }
    else
    {
        // Device does not support ABOUT — not a full MIPS box; show version only.
        ui->lblMIPSconfig->setText(ui->lblMIPSconfig->text() + ver + "\n\n");
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
            line = getNextLine(comms);
            if(line == "") break;
            res += line + "\n";
        }
        ui->lblMIPSconfig->setText(ui->lblMIPSconfig->text() + res + "\n");
    }

    res = comms->SendMess("UPTIME\n") + "\n";
    if(!res.contains("?"))
        ui->lblMIPSconfig->setText(ui->lblMIPSconfig->text() + res);

    res = "CPU temp: " + comms->SendMess("CPUTEMP\n") + "\n";
    if(!res.contains("?"))
        ui->lblMIPSconfig->setText(ui->lblMIPSconfig->text() + res);

    rfdriver->SetNumberOfChannels(numRF);
    dcbias->SetNumberOfChannels(numDCB);
}

// =============================================================================
// Connect / search / disconnect
// =============================================================================

/*! \brief MIPS::MIPSconnect
 * Slot: attempts a serial or TCP connection using the current settings and
 * host name, then calls MIPSsetup on success.
 *
 * FIX: Systems.clear() replaced with clearSystems() so any previously
 * allocated Comms threads are stopped and freed before the list is rebuilt.
 * The primary comms object (created in MIPS::MIPS) does not need a new
 * thread here — it already has one from construction.
 */
void MIPS::MIPSconnect(void)
{
    pollTimer->stop();
    comms->setSettings(settings->settings());
    comms->setProperties(properties);
    comms->setHost(ui->comboMIPSnetNames->currentText());
    if(comms->ConnectToMIPS())
    {
        // FIX: was Systems.clear() — leaked any previously allocated Comms
        // threads. clearSystems() stops and deletes them properly, but skips
        // the primary comms object which we are about to re-add.
        clearSystems();
        Systems << comms;
        // primaryComms is already on its thread — no new thread needed here.
        console->setEnabled(true);
        console->setLocalEchoEnabled(settings->settings().localEchoEnabled);
        ui->lblMIPSconnectionNotes->setHidden(true);
        MIPSsetup();
    }
    pollTimer->start(1000 * properties->UpdateSecs);
}

/*! \brief MIPS::MIPSsearch
 * Slot: scans available ports and network names for MIPS systems and
 * connects to all found.
 */
void MIPS::MIPSsearch(void)
{
    QMessageBox *msg = new QMessageBox();
    msg->setText("Searching for MIPS system(s)...");
    msg->setStandardButtons(QMessageBox::NoButton);
    msg->setWindowModality(Qt::NonModal);
    msg->show();
    QCoreApplication::processEvents();   // let the message box paint

    settings->fillPortsParameters();
    settings->fillPortsInfo();
    FindMIPSandConnect();
    msg->hide();
    delete msg;
}

/*! \brief MIPS::FindAllMIPSsystems
 * Enumerates all reachable MIPS/AMPS systems and populates the Systems list
 * and combo box.
 *
 * FIX: every new Comms object is immediately moved to a dedicated QThread
 * via makeCommsOnThread(). The thread pointer is stored in commsThreads so
 * clearSystems() can stop it cleanly on the next search or disconnect.
 *
 * FIX: Systems.clear() replaced with clearSystems() to prevent thread leaks
 * when the user searches more than once.
 *
 * FIX: QCoreApplication::processEvents() calls between port probes replaced
 * with a single processEvents() call used only to keep the status bar
 * message visible — the one place where a UI update is genuinely needed.
 * The delay() calls now use QThread::msleep() internally.
 */
void MIPS::FindAllMIPSsystems(void)
{
    Comms   *cp;
    QThread *ct;

    disconnect(ui->comboSystems, &QComboBox::currentIndexChanged, nullptr, nullptr);

    // FIX: was Systems.clear() — leaked threads on repeated searches.
    clearSystems();

    if(ui->comboMIPSnetNames->count() > 0)
    {
        // TCP/IP mode — try every name in the combo box.
        for(int j = 0; j < ui->comboMIPSnetNames->count(); j++)
        {
            // Allow the status bar to repaint between probes.
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            cp = makeCommsOnThread(settings, ui->statusBar, ct);
            cp->setHost(ui->comboMIPSnetNames->itemText(j));
            if(cp->ConnectToMIPS())
            {
                Systems      << cp;
                commsThreads << ct;
                connect(cp, &Comms::reconnected, this, [this]() {
                    if(!pollTimer->isActive())
                        pollTimer->start(1000 * properties->UpdateSecs);
                }, Qt::QueuedConnection);
            }
            else
            {
                // Connection failed — stop and discard this thread now.
                ct->quit();
                ct->wait();
                delete ct;
                delete cp;
            }
        }
    }
    else
    {
        // Serial port scan mode.
        int portCount = settings->numberOfPorts();
        settings->fillPortsInfo();

        for(int j = 0; j < portCount; j++)
        {
            if(settings->getPortName(j).contains("Bluetooth-Incoming-Port")) continue;

            ui->statusBar->showMessage("Trying: " + settings->getPortName(j));
            // Allow the status bar message to paint.
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

            // FIX: makeCommsOnThread() replaces bare "new Comms()" so the
            // object is on its worker thread before any Send* call is made.
            cp = makeCommsOnThread(settings, ui->statusBar, ct);
            cp->setProperties(properties);

            if(cp->isMIPS(settings->getPortName(j)))
            {
                delay();
                cp->setHost("");
                cp->SendString("ECHO,FALSE\n");
                if(cp->ConnectToMIPS())
                {
                    Systems      << cp;
                    commsThreads << ct;
                    connect(cp, &Comms::reconnected, this, [this]() {
                        if(!pollTimer->isActive())
                            pollTimer->start(1000 * properties->UpdateSecs);
                    }, Qt::QueuedConnection);
                }
                else
                {
                    ct->quit(); ct->wait(); delete ct; delete cp;
                }
            }
            else if(properties != nullptr && properties->SearchAMPS)
            {
                if(cp->isAMPS(settings->getPortName(j), properties->AMPSbaud))
                {
                    delay();
                    cp->setHost("");
                    if(cp->ConnectToMIPS())
                    {
                        Systems      << cp;
                        commsThreads << ct;
                        connect(cp, &Comms::reconnected, this, [this]() {
                            if(!pollTimer->isActive())
                                pollTimer->start(1000 * properties->UpdateSecs);
                        }, Qt::QueuedConnection);
                    }
                    else
                    {
                        ct->quit(); ct->wait(); delete ct; delete cp;
                    }
                }
                else
                {
                    // Neither MIPS nor AMPS — discard.
                    ct->quit(); ct->wait(); delete ct; delete cp;
                }
            }
            else
            {
                // Not MIPS, AMPS search disabled — discard.
                ct->quit(); ct->wait(); delete ct; delete cp;
            }
        }
    }

    ui->comboSystems->clear();
    for(int j = 0; j < Systems.count(); j++)
        ui->comboSystems->addItem(Systems.at(j)->MIPSname);

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
    pollTimer->stop();
    ui->pbSearchandConnect->setDown(false);
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    FindAllMIPSsystems();
    if(ui->comboSystems->count() > 0)
    {
        delay();
        comms = Systems.at(0);
        console->setEnabled(true);
        console->setLocalEchoEnabled(settings->settings().localEchoEnabled);
        ui->lblMIPSconnectionNotes->setHidden(true);
        MIPSsetup();
        connect(ui->comboSystems, &QComboBox::currentIndexChanged,
                this, [this](int){ UpdateSystem(); });
    }
    else
    {
        ui->statusBar->showMessage(tr("Can't find MIPS system!"));
    }
    pollTimer->start(1000 * properties->UpdateSecs);
}

/*! \brief MIPS::MIPSdisconnect
 * Slot: restores all tabs, disconnects all systems, and hides the
 * multi-system combo box.
 *
 * FIX: now calls clearSystems() instead of just DisconnectFromMIPS() so
 * every worker thread is stopped and its memory freed. comms is then reset
 * to primaryComms (the pre-allocated object from MIPS::MIPS()) which still
 * has its thread running and can be reconnected immediately.
 */
void MIPS::MIPSdisconnect(void)
{
    pollTimer->stop();
    AddTab("ADC");
    AddTab("Digital IO");
    AddTab("DCbias");
    AddTab("RFdriver");
    AddTab("ARB");
    AddTab("Twave");
    AddTab("FAIMS");
    AddTab("Filament");
    AddTab("Pulse Sequence Generation");

    // FIX: was DisconnectFromMIPS() only — left threads running.
    // clearSystems() stops every thread and frees every non-primary Comms.
    clearSystems();

    // Reset the active comms pointer to the primary pre-allocated object
    // so MIPSconnect() / MIPSsearch() work correctly after a disconnect.
    comms = primaryComms;

    ui->comboSystems->setVisible(false);
    ui->lblSystems->setVisible(false);
    ui->lblMIPSconfig->setText("");
    ui->lblMIPSconnectionNotes->setHidden(false);
}

/*! \brief MIPS::UpdateSystem
 * Slot: switches the active Comms pointer to the system selected in the
 * combo box and re-runs MIPSsetup.
 */
void MIPS::UpdateSystem(void)
{
    if(Systems.count() == 0) return;
    comms = Systems.at(ui->comboSystems->currentIndex());
    MIPSsetup();
}
