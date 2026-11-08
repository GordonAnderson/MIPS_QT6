// Unit tests for DeviceScriptProto. Expected values were produced by
// GAACE_Script's tools/gsc.py (the reference implementation) and the firmware's
// syscallTableHash(); the hash vector is shared by all three.
#include <QtTest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "devicescriptproto.h"

using namespace DeviceScriptProto;

// JSON written with ` in place of " so the test sources stay readable.
static QByteArray J(const char *text)
{
    QByteArray b(text);
    b.replace('`', '"');
    return b;
}

class DeviceScriptProtoTest : public QObject
{
    Q_OBJECT

    static QList<SyscallEntry> sharedVector()
    {
        return { {"read_adc", 0, false}, {"set_volts", 1, false}, {"get_volts", 1, true} };
    }

private slots:
    // ---- hash ------------------------------------------------------------
    void hashMatchesSharedVector()    { QCOMPARE(syscallTableHash(sharedVector()), 0x892E7C6Fu); }
    void hashOfEmptyTable()           { QCOMPARE(syscallTableHash({}), 0x811C9DC5u); }
    void hashHandlesMultiDigitArgc()  { QCOMPARE(syscallTableHash({ {"wide", 10, false} }), 0x87223431u); }
    void hashDependsOnOrderAndType()
    {
        QList<SyscallEntry> swapped = { {"set_volts", 1, false}, {"read_adc", 0, false}, {"get_volts", 1, true} };
        QVERIFY(syscallTableHash(swapped) != syscallTableHash(sharedVector()));
        QList<SyscallEntry> retyped = sharedVector();
        retyped[2].returnsFloat = false;
        QVERIFY(syscallTableHash(retyped) != syscallTableHash(sharedVector()));
    }

    // ---- GSYSCALLS ---------------------------------------------------------
    void parsesSyscallReply()
    {
        SyscallTable t;
        QString err;
        QVERIFY2(parseSyscalls({ "3,892E7C6F", "0,read_adc,0,int", "1,set_volts,1,int", "2,get_volts,1,float" },
                               t, &err), qPrintable(err));
        QCOMPARE(t.entries.count(), 3);
        QCOMPARE(t.entries[2].name, QString("get_volts"));
        QVERIFY(t.entries[2].returnsFloat);
        QCOMPARE(t.entries[1].argc, 1);
        QCOMPARE(t.hash, 0x892E7C6Fu);
    }
    void parsesEmptyTable()
    {
        SyscallTable t;
        QVERIFY(parseSyscalls({ "0,811C9DC5" }, t));
        QCOMPARE(t.entries.count(), 0);
    }
    void rejectsWrongHash()
    {
        SyscallTable t;
        QString err;
        QVERIFY(!parseSyscalls({ "3,892E7C6E", "0,read_adc,0,int", "1,set_volts,1,int", "2,get_volts,1,float" }, t, &err));
        QVERIFY(err.contains("892E7C6E"));
        QVERIFY(err.contains("892E7C6F"));
    }
    void rejectsMalformedReplies()
    {
        SyscallTable t;
        QVERIFY(!parseSyscalls({}, t));
        QVERIFY(!parseSyscalls({ "garbage" }, t));
        QVERIFY(!parseSyscalls({ "1,892E7C6F" }, t));                          // truncated
        QVERIFY(!parseSyscalls({ "1,892E7C6F", "0,read_adc,0,double" }, t));   // bad type
        QVERIFY(!parseSyscalls({ "1,892E7C6F", "5,read_adc,0,int" }, t));      // wrong index
        QVERIFY(!parseSyscalls({ "1,892E7C6F", "0,read_adc,x,int" }, t));      // bad argc
    }

    // ---- JSON for gsc.py -----------------------------------------------------
    void syscallJsonMatchesGscFormat()
    {
        SyscallTable t;
        t.entries = sharedVector();
        const QJsonObject o = QJsonDocument::fromJson(syscallTableJson(t)).object();
        QCOMPARE(o["hash"].toString(), QString("892E7C6F"));
        const QJsonArray a = o["syscalls"].toArray();
        QCOMPARE(a.count(), 3);
        QCOMPARE(a[2].toObject()["name"].toString(), QString("get_volts"));
        QCOMPARE(a[2].toObject()["argc"].toInt(), 1);
        QCOMPARE(a[2].toObject()["returns"].toString(), QString("float"));
        QCOMPARE(a[0].toObject()["returns"].toString(), QString("int"));
    }

    // ---- GCMDS ---------------------------------------------------------------
    void parsesCommandList()
    {
        const QList<CommandEntry> c = parseCommands({ "GCMDS,List all commands", "", "SDRV,Set drive, volts, 0 to 100", "NOHELP" });
        QCOMPARE(c.count(), 3);
        QCOMPARE(c[1].name, QString("SDRV"));
        QCOMPARE(c[1].help, QString("Set drive, volts, 0 to 100"));   // commas in help survive
        QCOMPARE(c[2].name, QString("NOHELP"));
        QCOMPARE(c[2].help, QString());
    }
    void commandJsonMatchesGscFormat()
    {
        const QJsonObject o = QJsonDocument::fromJson(commandListJson(parseCommands({ "SDRV,Set drive" }))).object();
        const QJsonArray a = o["commands"].toArray();
        QCOMPARE(a.count(), 1);
        QCOMPARE(a[0].toObject()["name"].toString(), QString("SDRV"));
        QCOMPARE(a[0].toObject()["help"].toString(), QString("Set drive"));
    }

    // ---- limits / slot status --------------------------------------------------
    void parsesLimits()
    {
        Limits l;
        QVERIFY(parseLimits("4,2048,32,16,32", l));
        QCOMPARE(l.slotCount, 4);
        QCOMPARE(l.maxCodeLen, 2048);
        QCOMPARE(l.maxSyscalls, 32);
        QVERIFY(!parseLimits("4,2048,32,16", l));
        QVERIFY(!parseLimits("a,b,c,d,e", l));
    }
    void parsesSlotStatus()
    {
        SlotStatus s;
        QVERIFY(parseSlotStatus("1,1", s));
        QVERIFY(s.loaded);
        QCOMPARE(s.lastStatus, 1);
        QVERIFY(parseSlotStatus("0,0", s));
        QVERIFY(!s.loaded);
        QVERIFY(!parseSlotStatus("2,0", s));
        QVERIFY(!parseSlotStatus("1", s));
    }
    void firstLineSkipsBlanks()
    {
        QCOMPARE(firstLine({ "", "  ", " 4,2048,32,16,32 ", "x" }), QString("4,2048,32,16,32"));
        QCOMPARE(firstLine({}), QString());
    }

    // ---- hex payload files -------------------------------------------------------
    void parsesHexPayload()
    {
        QByteArray b;
        QVERIFY(parseHexPayload("0a0B\n ff 00\r\n", b));
        QCOMPARE(b, QByteArray::fromHex("0a0bff00"));
    }
    void rejectsBadHexPayload()
    {
        QByteArray b;
        QString err;
        QVERIFY(!parseHexPayload("abc", b, &err));      QVERIFY(err.contains("odd"));
        QVERIFY(!parseHexPayload("zz", b, &err));       QVERIFY(err.contains("unexpected"));
        QVERIFY(!parseHexPayload("  \n", b, &err));     QVERIFY(err.contains("no hex"));
    }
    void parsesHashText()
    {
        quint32 h = 0;
        QVERIFY(parseHashText(" 892e7c6f ", h));
        QCOMPARE(h, 0x892E7C6Fu);
        QVERIFY(!parseHashText("", h));
        QVERIFY(!parseHashText("0", h));            // 0 means "unchecked"
        QVERIFY(!parseHashText("123456789", h));    // more than 8 digits
        QVERIFY(!parseHashText("xyz", h));
    }

    // ---- gsc.py --json output (real output from gsc.py 1.2.0) --------------------
    void parsesSuccessWithSyscalls()
    {
        CompileResult r;
        QString err;
        const QByteArray j = J("{`version`: `1.2.0`, `ok`: true, `payload`: `0a0001030000001801010300`, `code_len`: 10, `pool_len`: 0, `uses_syscalls`: true, `syscall_hash`: `892E7C6F`, `disasm`: `   0: PUSH_I32 3\\n   9: HALT`, `warnings`: []}");
        QVERIFY2(parseGscJson(j, r, &err), qPrintable(err));
        QVERIFY(r.ok);
        QCOMPARE(r.payload, QByteArray::fromHex("0a0001030000001801010300"));
        QVERIFY(r.usesSyscalls);
        QCOMPARE(r.syscallHash, 0x892E7C6Fu);
        QCOMPARE(r.codeLen, 10);
        QCOMPARE(r.gscVersion, QString("1.2.0"));
        QVERIFY(r.warnings.isEmpty());
        QVERIFY(r.disasm.contains("HALT"));
    }
    void parsesSuccessWithWarningAndNoHash()
    {
        CompileResult r;
        const QByteArray j1 = J("{`version`: `1.2.0`, `ok`: true, `payload`: `0b00`, `code_len`: 11, `pool_len`: 5, `uses_syscalls`: false, `syscall_hash`: null, `disasm`: ``, `warnings`: [{`message`: `the device has no command 'SDRW'`, `offset`: 4, `line`: 1, `column`: 5, `source_line`: `cmd(\\`SDRW\\`, 1);`}]}");
        QVERIFY(parseGscJson(j1, r));
        QVERIFY(r.ok);
        QCOMPARE(r.syscallHash, 0u);
        QCOMPARE(r.warnings.count(), 1);
        QCOMPARE(r.warnings[0].line, 1);
        QCOMPARE(r.warnings[0].column, 5);
        QCOMPARE(r.warnings[0].sourceLine, QString("cmd(\"SDRW\", 1);"));
    }
    void parsesLocatedError()
    {
        CompileResult r;
        const QByteArray j2 = J("{`version`: `1.2.0`, `ok`: false, `error`: {`message`: `unexpected token ';' (';')`, `offset`: 4, `line`: 1, `column`: 5, `source_line`: `x = ;`}}");
        QVERIFY(parseGscJson(j2, r));
        QVERIFY(!r.ok);
        QCOMPARE(r.error.line, 1);
        QCOMPARE(r.error.column, 5);
        QCOMPARE(r.error.offset, 4);
        QVERIFY(r.error.message.contains("unexpected token"));
    }
    void parsesUnlocatedError()
    {
        CompileResult r;
        const QByteArray j3 = J("{`version`: `1.2.0`, `ok`: false, `error`: {`message`: `cannot read x`}}");
        QVERIFY(parseGscJson(j3, r));
        QVERIFY(!r.ok);
        QCOMPARE(r.error.line, 0);
        QCOMPARE(r.error.message, QString("cannot read x"));
        const QByteArray j4 = J("{`ok`: false, `error`: {`message`: `m`, `line`: null, `column`: null, `offset`: null, `source_line`: null}}");
        QVERIFY(parseGscJson(j4, r));
        QCOMPARE(r.error.line, 0);
        QCOMPARE(r.error.offset, -1);
    }
    void missingOptionalFieldsAreFine()
    {
        CompileResult r;
        QVERIFY(parseGscJson(J("{`ok`: true, `payload`: `0a00`}"), r));
        QVERIFY(r.ok);
        QCOMPARE(r.syscallHash, 0u);
        QVERIFY(r.warnings.isEmpty());
    }
    void rejectsNonGscOutput()
    {
        CompileResult r;
        QString err;
        QVERIFY(!parseGscJson("Traceback (most recent call last):", r, &err));
        QVERIFY(!parseGscJson("[1,2]", r));
        const QByteArray j5 = J("{`nope`: 1}");
        QVERIFY(!parseGscJson(j5, r));
        const QByteArray j6 = J("{`ok`: true, `payload`: `zz`}");
        QVERIFY(!parseGscJson(j6, r, &err));
        const QByteArray j7 = J("{`ok`: true, `payload`: `0a00`, `syscall_hash`: `xyz`}");
        QVERIFY(!parseGscJson(j7, r, &err));
    }
    void comparesVersions()
    {
        QVERIFY(versionAtLeast("1.2.0", 1, 2, 0));
        QVERIFY(versionAtLeast("1.2.1", 1, 2, 0));
        QVERIFY(versionAtLeast("1.10.0", 1, 2, 0));
        QVERIFY(versionAtLeast("2.0.0", 1, 9, 9));
        QVERIFY(!versionAtLeast("1.1.9", 1, 2, 0));
        QVERIFY(!versionAtLeast("0.9.0", 1, 0, 0));
        QVERIFY(!versionAtLeast("", 1, 0, 0));
        QVERIFY(!versionAtLeast("1.2", 1, 0, 0));
        QVERIFY(!versionAtLeast("a.b.c", 1, 0, 0));
    }

    // ---- upload sequence (same as gsc.py --upload) -----------------------------
    void smallPayloadIsOneScriptLoad()
    {
        QCOMPARE(buildUploadCommands(1, QByteArray::fromHex("0a0b")), QStringList({ "SCRIPTLOAD,1,0a0b" }));
    }
    void scriptLoadCarriesTheHash()
    {
        QCOMPARE(buildUploadCommands(1, QByteArray::fromHex("0a0b"), 200, 0x892E7C6Fu),
                 QStringList({ "SCRIPTLOAD,1,0a0b,892E7C6F" }));
    }
    void largePayloadIsChunkedAndCommitted()
    {
        QCOMPARE(buildUploadCommands(2, QByteArray::fromHex("00010203040506070809"), 4, 0x892E7C6Fu),
                 QStringList({ "SCRIPTAPPEND,2,0,00010203", "SCRIPTAPPEND,2,4,04050607",
                               "SCRIPTAPPEND,2,8,0809", "SCRIPTCOMMIT,2,10,892E7C6F" }));
    }
    void commitWithoutHashHasNoTrailingComma()
    {
        const QStringList c = buildUploadCommands(0, QByteArray(5, 'x'), 2);
        QCOMPARE(c.last(), QString("SCRIPTCOMMIT,0,5"));
    }
    void payloadExactlyChunkSizeIsSingle()
    {
        QCOMPARE(buildUploadCommands(0, QByteArray(200, 'x'), 200).count(), 1);
        QCOMPARE(buildUploadCommands(0, QByteArray(201, 'x'), 200).count(), 3);  // 2 appends + commit
    }
};

QTEST_APPLESS_MAIN(DeviceScriptProtoTest)
#include "tst_devicescriptproto.moc"
