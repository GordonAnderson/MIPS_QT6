// =============================================================================
// devicescript.h
//
// DeviceScriptClient -- talks to a GAACE_Script runtime on a connected device
// over a Comms object: discovers its limits / syscall table / command list,
// loads a compiled script into a slot, and reads or controls a slot.
//
// All calls are synchronous and run on the GUI thread, like the rest of Comms.
// The reply parsing and command sequences live in devicescriptproto.* (which
// has unit tests); this file only does the I/O and turns failures into
// messages a user can act on.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     November 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef DEVICESCRIPT_H
#define DEVICESCRIPT_H

#include <functional>

#include <QByteArray>
#include <QString>

#include "devicescriptproto.h"

class Comms;

class DeviceScriptClient
{
public:
    //! Outcome of a call: ok, or a message explaining what went wrong.
    struct Result
    {
        bool    ok = false;
        QString message;
    };

    //! Called after each command of an upload with (commands done, commands total).
    typedef std::function<void(int done, int total)> ProgressFn;

    //! GSCRIPTLIMITS. A NAK means the device has no GAACE_Script runtime.
    static Result queryLimits(Comms *comms, DeviceScriptProto::Limits &out);

    //! GSYSCALLS. The table is verified against the hash the device reports.
    static Result querySyscalls(Comms *comms, DeviceScriptProto::SyscallTable &out);

    //! GCMDS: every command the device's command processor accepts.
    static Result queryCommands(Comms *comms, QList<DeviceScriptProto::CommandEntry> &out);

    //! GSCRIPTST,<slot>.
    static Result slotStatus(Comms *comms, int slot, DeviceScriptProto::SlotStatus &out);

    //! STENA,Script<slot>,TRUE|FALSE: start or stop the slot's thread.
    static Result setSlotRunning(Comms *comms, int slot, bool running);

    //! Loads `payload` ([codeLen][code][pool], as `gsc.py --scriptload` writes
    //! it) into `slot`. `hash` (0 = none) is the syscall table hash the script
    //! was compiled against; the device refuses the load if its own differs.
    //! Each command is retried once on a timeout (safe: SCRIPTLOAD and the
    //! offset-addressed SCRIPTAPPEND / SCRIPTCOMMIT can be repeated).
    static Result upload(Comms *comms, int slot, const QByteArray &payload, quint32 hash,
                         const ProgressFn &progress = ProgressFn(), int chunkBytes = 200);
};

#endif // DEVICESCRIPT_H
