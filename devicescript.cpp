// =============================================================================
// devicescript.cpp
//
// DeviceScriptClient. See devicescript.h.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     November 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "devicescript.h"
#include "comms.h"

using namespace DeviceScriptProto;

namespace
{

DeviceScriptClient::Result fail(const QString &message)
{
    DeviceScriptClient::Result r;
    r.ok = false;
    r.message = message;
    return r;
}

DeviceScriptClient::Result success()
{
    DeviceScriptClient::Result r;
    r.ok = true;
    return r;
}

DeviceScriptClient::Result notConnected()
{
    return fail("Not connected to a device.");
}

// Why a command that was NAKed or got no reply failed, in terms of what the
// user can do about it.
QString describeFailure(Comms::ReplyStatus status, const QString &step)
{
    if(status == Comms::ReplyTimeout)
        return QString("No response from the device after %1 (is the control port the one this "
                       "app is connected to, and is the device still running?)").arg(step);
    return QString("The device rejected %1.").arg(step);
}

} // namespace

DeviceScriptClient::Result DeviceScriptClient::queryLimits(Comms *comms, Limits &out)
{
    if(comms == nullptr) return notConnected();
    QStringList lines;
    const Comms::ReplyStatus st = comms->Transact("GSCRIPTLIMITS\n", &lines, 1000, 60);
    if(st == Comms::ReplyNak)
        return fail("This device does not support GAACE_Script (it rejected GSCRIPTLIMITS).");
    if(st == Comms::ReplyTimeout) return fail(describeFailure(st, "GSCRIPTLIMITS"));
    if(!parseLimits(firstLine(lines), out))
        return fail("Unrecognised GSCRIPTLIMITS reply: " + firstLine(lines));
    return success();
}

DeviceScriptClient::Result DeviceScriptClient::querySyscalls(Comms *comms, SyscallTable &out)
{
    if(comms == nullptr) return notConnected();
    QStringList lines;
    const Comms::ReplyStatus st = comms->Transact("GSYSCALLS\n", &lines, 1500, 150);
    if(st == Comms::ReplyNak)
        return fail("The device has no syscall table (its firmware predates GSYSCALLS, or it "
                    "never registered one).");
    if(st == Comms::ReplyTimeout) return fail(describeFailure(st, "GSYSCALLS"));
    QString err;
    if(!parseSyscalls(lines, out, &err)) return fail(err);
    return success();
}

DeviceScriptClient::Result DeviceScriptClient::queryCommands(Comms *comms, QList<CommandEntry> &out)
{
    if(comms == nullptr) return notConnected();
    QStringList lines;
    const Comms::ReplyStatus st = comms->Transact("GCMDS\n", &lines, 2000, 400);
    if(st != Comms::ReplyAck) return fail(describeFailure(st, "GCMDS"));
    out = parseCommands(lines);
    return success();
}

DeviceScriptClient::Result DeviceScriptClient::slotStatus(Comms *comms, int slot, SlotStatus &out)
{
    if(comms == nullptr) return notConnected();
    QStringList lines;
    const QString cmd = QString("GSCRIPTST,%1").arg(slot);
    const Comms::ReplyStatus st = comms->Transact(cmd + "\n", &lines, 1000, 60);
    if(st != Comms::ReplyAck) return fail(describeFailure(st, cmd));
    if(!parseSlotStatus(firstLine(lines), out))
        return fail("Unrecognised GSCRIPTST reply: " + firstLine(lines));
    return success();
}

DeviceScriptClient::Result DeviceScriptClient::setSlotRunning(Comms *comms, int slot, bool running)
{
    if(comms == nullptr) return notConnected();
    const QString cmd = QString("STENA,Script%1,%2").arg(slot).arg(running ? "TRUE" : "FALSE");
    const Comms::ReplyStatus st = comms->Transact(cmd + "\n", nullptr, 1000, 0);
    if(st != Comms::ReplyAck) return fail(describeFailure(st, cmd));
    return success();
}

DeviceScriptClient::Result DeviceScriptClient::upload(Comms *comms, int slot, const QByteArray &payload,
                                                      quint32 hash, const ProgressFn &progress,
                                                      int chunkBytes)
{
    if(comms == nullptr) return notConnected();
    const QStringList cmds = buildUploadCommands(slot, payload, chunkBytes, hash);
    for(int i = 0; i < cmds.count(); i++)
    {
        const QString &cmd = cmds.at(i);
        const QString step = cmd.section(',', 0, 0) + (cmds.count() > 1 && i + 1 < cmds.count()
                                 ? QString(" (chunk %1 of %2)").arg(i + 1).arg(cmds.count() - 1) : QString());

        Comms::ReplyStatus st = comms->Transact(cmd + "\n", nullptr, 2000, 0);
        if(st == Comms::ReplyTimeout) st = comms->Transact(cmd + "\n", nullptr, 2000, 0);  // one retry
        if(st == Comms::ReplyNak)
        {
            QString why = "The device rejected " + step + ". Likely causes: a bad slot number, a "
                          "malformed payload, a payload over the slot's size limit (see GSCRIPTLIMITS)";
            if(hash != 0)
                why += ", or the device's syscall table differs from the one the script was "
                       "compiled against (recompile against this device)";
            return fail(why + ".");
        }
        if(st == Comms::ReplyTimeout) return fail(describeFailure(st, step));
        if(progress) progress(i + 1, cmds.count());
    }
    return success();
}
