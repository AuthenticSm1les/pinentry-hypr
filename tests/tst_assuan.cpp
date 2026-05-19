#include <QtTest>
#include "assuan_protocol.h"

class TestAssuanProtocol : public QObject {
    Q_OBJECT

private slots:
    void testParseSETPROMPT() {
        auto result = parseAssuanLine("SETPROMPT Enter passphrase:");
        QCOMPARE(result.cmd, AssuanCmd::SETPROMPT);
        QCOMPARE(result.value, "Enter passphrase:");
    }

    void testParseSETPROMPTNoValue() {
        auto result = parseAssuanLine("SETPROMPT");
        QCOMPARE(result.cmd, AssuanCmd::SETPROMPT);
        QVERIFY(result.value.empty());
    }

    void testParseSETDESC() {
        auto result = parseAssuanLine("SETDESC Please enter your PIN");
        QCOMPARE(result.cmd, AssuanCmd::SETDESC);
        QCOMPARE(result.value, "Please enter your PIN");
    }

    void testParseSETDESCNoValue() {
        auto result = parseAssuanLine("SETDESC");
        QCOMPARE(result.cmd, AssuanCmd::SETDESC);
        QVERIFY(result.value.empty());
    }

    void testParseSETDESCMultiWord() {
        auto result = parseAssuanLine("SETDESC This is a multi-word\n description with\n newlines");
        QCOMPARE(result.cmd, AssuanCmd::SETDESC);
        QCOMPARE(result.value, "This is a multi-word\n description with\n newlines");
    }

    void testParseSETKEYINFO() {
        auto result = parseAssuanLine("SETKEYINFO mysubkey");
        QCOMPARE(result.cmd, AssuanCmd::SETKEYINFO);
        QCOMPARE(result.value, "mysubkey");
    }

    void testParseSETKEYINFONoValue() {
        auto result = parseAssuanLine("SETKEYINFO");
        QCOMPARE(result.cmd, AssuanCmd::SETKEYINFO);
        QVERIFY(result.value.empty());
    }

    void testParseSETKEYINFOHash() {
        auto result = parseAssuanLine("SETKEYINFO D41D8CD98F00B204E9800998ECF8427E");
        QCOMPARE(result.cmd, AssuanCmd::SETKEYINFO);
        QCOMPARE(result.value, "D41D8CD98F00B204E9800998ECF8427E");
    }

    void testParseGETPIN() {
        auto result = parseAssuanLine("GETPIN");
        QCOMPARE(result.cmd, AssuanCmd::GETPIN);
        QVERIFY(result.value.empty());
    }

    void testParseCONFIRM() {
        auto result = parseAssuanLine("CONFIRM");
        QCOMPARE(result.cmd, AssuanCmd::CONFIRM);
        QVERIFY(result.value.empty());
    }

    void testParseMESSAGE() {
        auto result = parseAssuanLine("MESSAGE");
        QCOMPARE(result.cmd, AssuanCmd::MESSAGE);
        QVERIFY(result.value.empty());
    }

    void testParseSETTIMEOUT() {
        auto result = parseAssuanLine("SETTIMEOUT 30");
        QCOMPARE(result.cmd, AssuanCmd::SETTIMEOUT);
        QCOMPARE(result.value, "30");
    }

    void testParseSETTIMEOUTNoValue() {
        auto result = parseAssuanLine("SETTIMEOUT");
        QCOMPARE(result.cmd, AssuanCmd::SETTIMEOUT);
        QVERIFY(result.value.empty());
    }

    void testParseSETTIMEOUTNegative() {
        auto result = parseAssuanLine("SETTIMEOUT -1");
        QCOMPARE(result.cmd, AssuanCmd::SETTIMEOUT);
        QCOMPARE(result.value, "-1");
    }

    void testParseSETTIMEOUTZero() {
        auto result = parseAssuanLine("SETTIMEOUT 0");
        QCOMPARE(result.cmd, AssuanCmd::SETTIMEOUT);
        QCOMPARE(result.value, "0");
    }

    void testParseSETTIMEOUTLarge() {
        auto result = parseAssuanLine("SETTIMEOUT 3600");
        QCOMPARE(result.cmd, AssuanCmd::SETTIMEOUT);
        QCOMPARE(result.value, "3600");
    }

    void testParseSETTIMEOUTText() {
        auto result = parseAssuanLine("SETTIMEOUT abc");
        QCOMPARE(result.cmd, AssuanCmd::SETTIMEOUT);
        QCOMPARE(result.value, "abc");
    }

    void testParseSETOK() {
        auto result = parseAssuanLine("SETOK Yes");
        QCOMPARE(result.cmd, AssuanCmd::SETOK);
        QCOMPARE(result.value, "Yes");
    }

    void testParseSETOKNoValue() {
        auto result = parseAssuanLine("SETOK");
        QCOMPARE(result.cmd, AssuanCmd::SETOK);
        QVERIFY(result.value.empty());
    }

    void testParseSETOKMultiWord() {
        auto result = parseAssuanLine("SETOK Yes, I agree to the terms");
        QCOMPARE(result.cmd, AssuanCmd::SETOK);
        QCOMPARE(result.value, "Yes, I agree to the terms");
    }

    void testParseSETOKWithSpecialChars() {
        auto result = parseAssuanLine("SETOK OK (Y/n)");
        QCOMPARE(result.cmd, AssuanCmd::SETOK);
        QCOMPARE(result.value, "OK (Y/n)");
    }

    void testParseSETCANCEL() {
        auto result = parseAssuanLine("SETCANCEL No");
        QCOMPARE(result.cmd, AssuanCmd::SETCANCEL);
        QCOMPARE(result.value, "No");
    }

    void testParseSETCANCELNoValue() {
        auto result = parseAssuanLine("SETCANCEL");
        QCOMPARE(result.cmd, AssuanCmd::SETCANCEL);
        QVERIFY(result.value.empty());
    }

    void testParseSETCANCELMultiWord() {
        auto result = parseAssuanLine("SETCANCEL No, I do not agree");
        QCOMPARE(result.cmd, AssuanCmd::SETCANCEL);
        QCOMPARE(result.value, "No, I do not agree");
    }

    void testParseSETNOTSET() {
        auto result = parseAssuanLine("SETNOTSET Extra info");
        QCOMPARE(result.cmd, AssuanCmd::SETNOTSET);
        QCOMPARE(result.value, "Extra info");
    }

    void testParseSETNOTSETNoValue() {
        auto result = parseAssuanLine("SETNOTSET");
        QCOMPARE(result.cmd, AssuanCmd::SETNOTSET);
        QVERIFY(result.value.empty());
    }

    void testParseSETERROR() {
        auto result = parseAssuanLine("SETERROR Something went wrong");
        QCOMPARE(result.cmd, AssuanCmd::SETERROR);
        QCOMPARE(result.value, "Something went wrong");
    }

    void testParseSETERRORNoValue() {
        auto result = parseAssuanLine("SETERROR");
        QCOMPARE(result.cmd, AssuanCmd::SETERROR);
        QVERIFY(result.value.empty());
    }

    void testParseSETERRORLong() {
        auto result = parseAssuanLine("SETERROR The passphrase does not match the expected format; please try again with a longer passphrase containing special characters");
        QCOMPARE(result.cmd, AssuanCmd::SETERROR);
        QVERIFY(!result.value.empty());
    }

    void testParseSETTITLE() {
        auto result = parseAssuanLine("SETTITLE My Dialog");
        QCOMPARE(result.cmd, AssuanCmd::SETTITLE);
        QCOMPARE(result.value, "My Dialog");
    }

    void testParseSETTITLENoValue() {
        auto result = parseAssuanLine("SETTITLE");
        QCOMPARE(result.cmd, AssuanCmd::SETTITLE);
        QVERIFY(result.value.empty());
    }

    void testParseOPTION() {
        auto result = parseAssuanLine("OPTION allow-external-password-cache");
        QCOMPARE(result.cmd, AssuanCmd::OPTION);
        QCOMPARE(result.value, "allow-external-password-cache");
    }

    void testParseOPTIONWithEquals() {
        auto result = parseAssuanLine("OPTION default-pwmngr=My Manager");
        QCOMPARE(result.cmd, AssuanCmd::OPTION);
        QCOMPARE(result.value, "default-pwmngr=My Manager");
    }

    void testParseOPTIONNoValue() {
        auto result = parseAssuanLine("OPTION");
        QCOMPARE(result.cmd, AssuanCmd::OPTION);
        QVERIFY(result.value.empty());
    }

    void testParseOPTIONMultipleEquals() {
        auto result = parseAssuanLine("OPTION default-pwmngr=a=b=c");
        QCOMPARE(result.cmd, AssuanCmd::OPTION);
        QCOMPARE(result.value, "default-pwmngr=a=b=c");
    }

    void testParseOPTIONCacheWithTrailingEquals() {
        auto result = parseAssuanLine("OPTION allow-external-password-cache=1");
        QCOMPARE(result.cmd, AssuanCmd::OPTION);
        QCOMPARE(result.value, "allow-external-password-cache=1");
    }

    void testParseBYE() {
        auto result = parseAssuanLine("BYE");
        QCOMPARE(result.cmd, AssuanCmd::BYE);
        QVERIFY(result.value.empty());
    }

    void testParseUnknown() {
        auto result = parseAssuanLine("SOMETHING");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
        QCOMPARE(result.value, "SOMETHING");
    }

    void testParseEmptyLine() {
        auto result = parseAssuanLine("");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
        QVERIFY(result.value.empty());
    }

    void testParseWhitespaceLine() {
        auto result = parseAssuanLine("   ");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
        QCOMPARE(result.value, "   ");
    }

    void testParseNumericLine() {
        auto result = parseAssuanLine("12345");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
        QCOMPARE(result.value, "12345");
    }

    void testParseSpecialCharsLine() {
        auto result = parseAssuanLine("!@#$%^&*()");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
        QCOMPARE(result.value, "!@#$%^&*()");
    }

    void testParseSETPROMPTCaseSensitive() {
        auto result = parseAssuanLine("setprompt test");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
    }

    void testParseAllCommandsCaseSensitive() {
        QCOMPARE(parseAssuanLine("getpin").cmd, AssuanCmd::UNKNOWN);
        QCOMPARE(parseAssuanLine("confirm").cmd, AssuanCmd::UNKNOWN);
        QCOMPARE(parseAssuanLine("message").cmd, AssuanCmd::UNKNOWN);
        QCOMPARE(parseAssuanLine("bye").cmd, AssuanCmd::UNKNOWN);
        QCOMPARE(parseAssuanLine("option").cmd, AssuanCmd::UNKNOWN);
        QCOMPARE(parseAssuanLine("settimeout").cmd, AssuanCmd::UNKNOWN);
    }

    void testParseSubstringPrefix() {
        auto result = parseAssuanLine("SETPROMPTING would not match");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
    }

    void testParseCONFIRMSubstring() {
        auto result = parseAssuanLine("CONFIRMATION");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
    }

    void testParseMESSAGESubstring() {
        auto result = parseAssuanLine("MESSAGING");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
    }

    void testParseOPTIONSubstring() {
        auto result = parseAssuanLine("OPTIONS extra");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
    }

    void testParseBYESubstring() {
        auto result = parseAssuanLine("BYEING");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
    }

    void testParseSETTIMEOUTSubstring() {
        auto result = parseAssuanLine("SETTIMEOUTS");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
    }

    void testParseSETOKSubstring() {
        auto result = parseAssuanLine("SETOKAY");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
    }

    void testParseSETCANCELSubstring() {
        auto result = parseAssuanLine("SETCANCELED");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
    }

    void testParseSETERRORSubstring() {
        auto result = parseAssuanLine("SETERRORS");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
    }

    void testParseSETTITLESubstring() {
        auto result = parseAssuanLine("SETTITLED");
        QCOMPARE(result.cmd, AssuanCmd::UNKNOWN);
    }

    void testIsCommandMatchExact() {
        QVERIFY(isCommandMatch("BYE", "BYE"));
    }

    void testIsCommandMatchWithSpace() {
        QVERIFY(isCommandMatch("SETPROMPT test", "SETPROMPT"));
    }

    void testIsCommandMatchSubstringNoSpace() {
        QVERIFY(!isCommandMatch("BYEING", "BYE"));
    }

    void testIsCommandMatchTooShort() {
        QVERIFY(!isCommandMatch("AB", "ABC"));
    }

    void testIsCommandMatchEmpty() {
        QVERIFY(!isCommandMatch("", "BYE"));
    }

    void testIsCommandMatchEmptyLineNonEmptyCmd() {
        QVERIFY(!isCommandMatch("", "A"));
    }

    void testHasKeyInfoOptionExactMatch() {
        QVERIFY(hasKeyInfoOption("allow-external-password-cache"));
    }

    void testHasKeyInfoOptionSubstring() {
        QVERIFY(!hasKeyInfoOption("x-allow-external-password-cache"));
    }

    void testHasKeyInfoOptionEmpty() {
        QVERIFY(!hasKeyInfoOption(""));
    }

    void testHasKeyInfoOptionOther() {
        QVERIFY(!hasKeyInfoOption("default-pwmngr=test"));
    }

    void testHasKeyInfoOptionWithEquals() {
        QVERIFY(!hasKeyInfoOption("allow-external-password-cache=1"));
    }

    void testGetPwmngrValue() {
        QCOMPARE(getPwmngrValue("default-pwmngr=GNOME Keyring"), "GNOME Keyring");
    }

    void testGetPwmngrValueEmptyValue() {
        QCOMPARE(getPwmngrValue("default-pwmngr="), "");
    }

    void testGetPwmngrValueNoMatch() {
        QVERIFY(getPwmngrValue("allow-external-password-cache").empty());
    }

    void testGetPwmngrValueSubstring() {
        QVERIFY(getPwmngrValue("x-default-pwmngr=test").empty());
    }

    void testGetPwmngrValueMultipleEquals() {
        QCOMPARE(getPwmngrValue("default-pwmngr=a=b=c"), "a=b=c");
    }

    void testGetPwmngrValueOnlyPrefix() {
        QString val = QString::fromStdString(getPwmngrValue("default-pwmngr"));
        QVERIFY(val.isEmpty());
    }
};

QTEST_MAIN(TestAssuanProtocol)
#include "tst_assuan.moc"
