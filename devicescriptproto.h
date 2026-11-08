// =============================================================================
// devicescriptproto.h
//
// Wire-protocol helpers for GAACE_Script, the on-device bytecode scripting
// system (https://github.com/GordonAnderson/GAACE_Script). Everything here is
// pure data handling -- no Comms, no widgets -- so it can be unit-tested
// without hardware (see tests/devicescriptproto_test).
//
// What it covers:
//   - parsing the replies to GSYSCALLS, GCMDS, GSCRIPTLIMITS and GSCRIPTST
//   - the syscall-table hash (FNV-1a), which MUST match syscallTableHash() in
//     the firmware (GAACEScript.cpp) and syscall_table_hash() in gsc.py; all
//     three are pinned to the same test vector
//   - the JSON files gsc.py reads for --syscalls and --commands
//   - the SCRIPTLOAD / SCRIPTAPPEND / SCRIPTCOMMIT command sequence, mirroring
//     gsc.py's _upload_on()
//
// Replies are passed in as the lines Comms::Transact() collects: the ACK byte
// and line endings are already removed, so the first element of a GSYSCALLS
// reply is its "count,HASH" header.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     November 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef DEVICESCRIPTPROTO_H
#define DEVICESCRIPTPROTO_H

#include <QByteArray>
#include <QList>
#include <QString>
#include <QStringList>

namespace DeviceScriptProto
{

//! One entry of a device's syscall table; its id is its index in the table.
struct SyscallEntry
{
    QString name;
    int     argc = 0;
    bool    returnsFloat = false;
};

//! A device's syscall table plus the hash it reported (and we verified).
struct SyscallTable
{
    QList<SyscallEntry> entries;
    quint32             hash = 0;
};

//! One command the device's command processor accepts (from GCMDS).
struct CommandEntry
{
    QString name;
    QString help;
};

//! Compile-time limits of the device's script runtime (GSCRIPTLIMITS).
struct Limits
{
    int slotCount = 0;
    int maxCodeLen = 0;   //!< bytes per slot: length prefix + code + string pool
    int stackSize = 0;
    int varSlots = 0;
    int maxSyscalls = 0;
};

//! State of one script slot (GSCRIPTST).
struct SlotStatus
{
    bool loaded = false;
    int  lastStatus = 0;  //!< the VM's Status code from the last run
};

//! A compile error or warning from gsc.py, located in the source. `line` and
//! `column` are 1-based; line 0 means the message has no location.
struct Diagnostic
{
    QString message;
    int     line = 0;
    int     column = 0;
    int     offset = -1;      //!< 0-based character offset into the source, or -1
    QString sourceLine;       //!< the text of that line
};

//! What `gsc.py --json` reported (see tools/README.md in GAACE_Script).
struct CompileResult
{
    bool              ok = false;
    Diagnostic        error;          //!< the script's error, when !ok (message empty if none)
    QString           failure;        //!< set when the compiler could not be run or answered nonsense
    QList<Diagnostic> warnings;
    QByteArray        payload;        //!< [codeLen][code][pool], ready for SCRIPTLOAD
    bool              usesSyscalls = false;
    quint32           syscallHash = 0;  //!< to send with the script; 0 = none (script uses no syscalls)
    int               codeLen = 0;
    int               poolLen = 0;
    QString           disasm;
    QString           gscVersion;
};

//! Parses the JSON object `gsc.py --json` prints. Returns false, with a
//! message in *error, if it is not that.
bool parseGscJson(const QByteArray &json, CompileResult &out, QString *error = nullptr);

//! True if "major.minor.patch" in `version` is at least the given version.
bool versionAtLeast(const QString &version, int major, int minor, int patch);

//! FNV-1a (32-bit) over "name:argc:i|f;" per entry, in order. Never 0.
quint32 syscallTableHash(const QList<SyscallEntry> &entries);

//! Parses a GSYSCALLS reply ("count,HASH", then "index,name,argc,int|float"
//! lines). Fails, with a message in *error, if the reply is malformed or
//! incomplete or if the entries do not hash to the hash the device reported.
bool parseSyscalls(const QStringList &lines, SyscallTable &out, QString *error = nullptr);

//! Parses a GCMDS reply ("NAME,help" per line; help may contain commas).
QList<CommandEntry> parseCommands(const QStringList &lines);

//! Parses a GSCRIPTLIMITS reply ("slots,maxCodeLen,stackSize,varSlots,maxSyscalls").
bool parseLimits(const QString &line, Limits &out);

//! Parses a GSCRIPTST reply ("loaded,lastStatus").
bool parseSlotStatus(const QString &line, SlotStatus &out);

//! The first non-empty line of a reply, or an empty string.
QString firstLine(const QStringList &lines);

//! The JSON `gsc.py --syscalls FILE` reads.
QByteArray syscallTableJson(const SyscallTable &table);

//! The JSON `gsc.py --commands FILE` reads.
QByteArray commandListJson(const QList<CommandEntry> &commands);

//! Reads the hex text `gsc.py --scriptload --format hex` writes: whitespace is
//! ignored, the rest must be an even number of hex digits. Fails with a
//! message in *error otherwise.
bool parseHexPayload(const QString &text, QByteArray &out, QString *error = nullptr);

//! Parses up to 8 hex digits into a non-zero hash (0 means "unchecked").
bool parseHashText(const QString &text, quint32 &out);

//! The command lines (without line endings) that load `payload` into `slot`:
//! a single SCRIPTLOAD if it fits in `chunkBytes`, otherwise SCRIPTAPPEND
//! chunks followed by SCRIPTCOMMIT. `hash` (0 = none) is appended to
//! SCRIPTLOAD / SCRIPTCOMMIT so the device can refuse a script compiled
//! against a different syscall table. Same output as gsc.py --upload.
QStringList buildUploadCommands(int slot, const QByteArray &payload, int chunkBytes = 200,
                                quint32 hash = 0);

} // namespace DeviceScriptProto

#endif // DEVICESCRIPTPROTO_H
