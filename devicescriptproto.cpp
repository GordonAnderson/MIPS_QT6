// =============================================================================
// devicescriptproto.cpp
//
// Wire-protocol helpers for GAACE_Script. See devicescriptproto.h.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     November 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "devicescriptproto.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace DeviceScriptProto
{

// FNV-1a over "name:argc:i|f;" per entry. Keep in step with syscallTableHash()
// in GAACE_Script's src/GAACEScript.cpp and syscall_table_hash() in
// tools/gsc.py -- all three are pinned to the same test vector.
static QString hex8(quint32 v)
{
    return QString("%1").arg(v, 8, 16, QChar('0')).toUpper();
}

quint32 syscallTableHash(const QList<SyscallEntry> &entries)
{
    quint32 h = 0x811C9DC5u;
    for(const SyscallEntry &e : entries)
    {
        const QByteArray text = (e.name + ":" + QString::number(e.argc) + ":" +
                                 (e.returnsFloat ? "f" : "i") + ";").toLatin1();
        for(char c : text)
        {
            h ^= (quint8)c;
            h *= 0x01000193u;
        }
    }
    return h ? h : 1;
}

bool parseSyscalls(const QStringList &lines, SyscallTable &out, QString *error)
{
    auto fail = [&](const QString &msg)
    {
        if(error != nullptr) *error = msg;
        return false;
    };

    out = SyscallTable();
    if(lines.isEmpty()) return fail("empty GSYSCALLS reply");

    const QStringList header = lines.at(0).trimmed().split(',');
    bool okCount = false, okHash = false;
    const int     count    = header.count() == 2 ? header.at(0).toInt(&okCount) : 0;
    const quint32 reported = header.count() == 2 ? header.at(1).toUInt(&okHash, 16) : 0;
    if(!okCount || !okHash || count < 0)
        return fail("unrecognised GSYSCALLS header: " + lines.at(0));
    if(lines.count() - 1 < count)
        return fail(QString("GSYSCALLS reply is incomplete: expected %1 syscalls, got %2")
                    .arg(count).arg(lines.count() - 1));

    for(int i = 0; i < count; i++)
    {
        const QString line = lines.at(i + 1).trimmed();
        const QStringList f = line.split(',');
        bool okIndex = false, okArgc = false;
        if(f.count() != 4 || f.at(0).toInt(&okIndex) != i || !okIndex ||
           (f.at(3) != "int" && f.at(3) != "float"))
            return fail("unrecognised GSYSCALLS line: " + line);
        SyscallEntry e;
        e.name = f.at(1);
        e.argc = f.at(2).toInt(&okArgc);
        if(!okArgc || e.argc < 0 || e.name.isEmpty())
            return fail("unrecognised GSYSCALLS line: " + line);
        e.returnsFloat = (f.at(3) == "float");
        out.entries.append(e);
    }

    out.hash = syscallTableHash(out.entries);
    if(out.hash != reported)
        return fail(QString("device reported syscall table hash %1 but its entries hash to %2")
                    .arg(hex8(reported), hex8(out.hash)));
    return true;
}

static Diagnostic diagnosticFromJson(const QJsonObject &o)
{
    Diagnostic d;
    d.message    = o["message"].toString();
    d.line       = o["line"].isNull() ? 0 : o["line"].toInt();
    d.column     = o["column"].isNull() ? 0 : o["column"].toInt();
    d.offset     = o["offset"].isNull() ? -1 : o["offset"].toInt(-1);
    d.sourceLine = o["source_line"].toString();
    return d;
}

bool parseGscJson(const QByteArray &json, CompileResult &out, QString *error)
{
    auto fail = [&](const QString &msg)
    {
        if(error != nullptr) *error = msg;
        return false;
    };

    out = CompileResult();
    QJsonParseError pe;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &pe);
    if(doc.isNull() || !doc.isObject())
        return fail("gsc.py did not print a JSON object" +
                    (pe.error != QJsonParseError::NoError ? ": " + pe.errorString() : QString()));
    const QJsonObject o = doc.object();
    if(!o.contains("ok") || !o["ok"].isBool()) return fail("gsc.py's JSON has no \"ok\" field");

    out.gscVersion = o["version"].toString();
    out.ok = o["ok"].toBool();
    if(!out.ok)
    {
        out.error = diagnosticFromJson(o["error"].toObject());
        return true;
    }

    const QString hex = o["payload"].toString();
    QString perr;
    if(!parseHexPayload(hex, out.payload, &perr)) return fail("gsc.py's payload: " + perr);
    out.codeLen      = o["code_len"].toInt();
    out.poolLen      = o["pool_len"].toInt();
    out.usesSyscalls = o["uses_syscalls"].toBool();
    out.disasm       = o["disasm"].toString();
    const QJsonValue hashValue = o["syscall_hash"];    // null or absent: the script uses no syscalls
    if(!hashValue.isNull() && !hashValue.isUndefined() && !parseHashText(hashValue.toString(), out.syscallHash))
        return fail("gsc.py's syscall_hash is not a hash: " + o["syscall_hash"].toString());
    for(const QJsonValue &w : o["warnings"].toArray())
        out.warnings.append(diagnosticFromJson(w.toObject()));
    return true;
}

bool versionAtLeast(const QString &version, int major, int minor, int patch)
{
    const QStringList f = version.trimmed().split('.');
    if(f.count() != 3) return false;
    int v[3];
    for(int i = 0; i < 3; i++)
    {
        bool ok = false;
        v[i] = f.at(i).toInt(&ok);
        if(!ok) return false;
    }
    if(v[0] != major) return v[0] > major;
    if(v[1] != minor) return v[1] > minor;
    return v[2] >= patch;
}

QList<CommandEntry> parseCommands(const QStringList &lines)
{
    QList<CommandEntry> out;
    for(const QString &raw : lines)
    {
        const QString line = raw.trimmed();
        if(line.isEmpty()) continue;
        const int comma = line.indexOf(',');
        CommandEntry e;
        e.name = (comma < 0 ? line : line.left(comma)).trimmed();
        e.help = (comma < 0 ? QString() : line.mid(comma + 1)).trimmed();
        if(!e.name.isEmpty()) out.append(e);
    }
    return out;
}

bool parseLimits(const QString &line, Limits &out)
{
    const QStringList f = line.trimmed().split(',');
    if(f.count() != 5) return false;
    int v[5];
    for(int i = 0; i < 5; i++)
    {
        bool ok = false;
        v[i] = f.at(i).toInt(&ok);
        if(!ok || v[i] < 0) return false;
    }
    out.slotCount = v[0]; out.maxCodeLen = v[1]; out.stackSize = v[2];
    out.varSlots = v[3]; out.maxSyscalls = v[4];
    return true;
}

bool parseSlotStatus(const QString &line, SlotStatus &out)
{
    const QStringList f = line.trimmed().split(',');
    if(f.count() != 2) return false;

    // The firmware prints the loaded flag with commandProcessor::print(bool),
    // i.e. "TRUE" / "FALSE" (found on real hardware; its own comments and
    // README said 0|1). Accept either spelling.
    const QString word = f.at(0).trimmed().toUpper();
    bool loaded;
    if(word == "TRUE" || word == "1")        loaded = true;
    else if(word == "FALSE" || word == "0")  loaded = false;
    else return false;

    bool okStatus = false;
    const int status = f.at(1).toInt(&okStatus);
    if(!okStatus) return false;
    out.loaded = loaded;
    out.lastStatus = status;
    return true;
}

QString firstLine(const QStringList &lines)
{
    for(const QString &l : lines)
        if(!l.trimmed().isEmpty()) return l.trimmed();
    return QString();
}

QByteArray syscallTableJson(const SyscallTable &table)
{
    QJsonArray list;
    for(const SyscallEntry &e : table.entries)
    {
        QJsonObject o;
        o["name"]    = e.name;
        o["argc"]    = e.argc;
        o["returns"] = e.returnsFloat ? "float" : "int";
        list.append(o);
    }
    QJsonObject root;
    // gsc.py checks that this agrees with the hash of the entries.
    root["hash"]     = hex8(syscallTableHash(table.entries));
    root["syscalls"] = list;
    return QJsonDocument(root).toJson(QJsonDocument::Indented);
}

QByteArray commandListJson(const QList<CommandEntry> &commands)
{
    QJsonArray list;
    for(const CommandEntry &c : commands)
    {
        QJsonObject o;
        o["name"] = c.name;
        o["help"] = c.help;
        list.append(o);
    }
    QJsonObject root;
    root["commands"] = list;
    return QJsonDocument(root).toJson(QJsonDocument::Indented);
}

bool parseHexPayload(const QString &text, QByteArray &out, QString *error)
{
    QString digits;
    for(const QChar &c : text)
    {
        if(c.isSpace()) continue;
        const ushort u = c.unicode();
        const bool hex = (u >= '0' && u <= '9') || (u >= 'a' && u <= 'f') || (u >= 'A' && u <= 'F');
        if(!hex)
        {
            if(error != nullptr) *error = QString("not a hex payload: unexpected character '%1'").arg(c);
            return false;
        }
        digits += c;
    }
    if(digits.isEmpty() || (digits.length() % 2) != 0)
    {
        if(error != nullptr)
            *error = digits.isEmpty() ? "the file contains no hex digits"
                                      : "the hex payload has an odd number of digits";
        return false;
    }
    out = QByteArray::fromHex(digits.toLatin1());
    return true;
}

bool parseHashText(const QString &text, quint32 &out)
{
    const QString t = text.trimmed();
    if(t.isEmpty() || t.length() > 8) return false;
    bool ok = false;
    const quint32 v = t.toUInt(&ok, 16);
    if(!ok || v == 0) return false;
    out = v;
    return true;
}

QStringList buildUploadCommands(int slot, const QByteArray &payload, int chunkBytes, quint32 hash)
{
    QStringList cmds;
    const QString hashArg = hash ? "," + hex8(hash) : QString();
    const int total = payload.size();
    if(chunkBytes < 1) chunkBytes = 1;

    if(total <= chunkBytes)
    {
        cmds << QString("SCRIPTLOAD,%1,%2%3").arg(slot).arg(QString::fromLatin1(payload.toHex())).arg(hashArg);
        return cmds;
    }
    for(int offset = 0; offset < total; offset += chunkBytes)
    {
        cmds << QString("SCRIPTAPPEND,%1,%2,%3").arg(slot).arg(offset)
                    .arg(QString::fromLatin1(payload.mid(offset, chunkBytes).toHex()));
    }
    cmds << QString("SCRIPTCOMMIT,%1,%2%3").arg(slot).arg(total).arg(hashArg);
    return cmds;
}

} // namespace DeviceScriptProto
