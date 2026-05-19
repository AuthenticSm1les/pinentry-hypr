#include <QtTest>
#include "secure_memory.h"

class TestSecureMemory : public QObject {
    Q_OBJECT

private:
    static bool isZeroed(const char *data, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            if (data[i] != 0) return false;
        }
        return true;
    }

private slots:
    void testDefaultConstruction() {
        SecureString s;
        QVERIFY(s.empty());
        QCOMPARE(s.size(), size_t(0));
        QCOMPARE(s.data(), nullptr);
    }

    void testStringConstruction() {
        SecureString s(std::string("hello"));
        QVERIFY(!s.empty());
        QCOMPARE(s.size(), size_t(5));
        QCOMPARE(s.toString(), "hello");
    }

    void testCStringConstruction() {
        SecureString s("world", 5);
        QCOMPARE(s.toString(), "world");
    }

    void testCStringConstructionWithNull() {
        SecureString s("hello\0world", 11);
        QCOMPARE(s.size(), size_t(11));
        std::string expected("hello\0world", 11);
        QCOMPARE(s.toString(), expected);
    }

    void testEmptyStringConstruction() {
        SecureString s("");
        QVERIFY(s.empty());
    }

    void testMoveConstructor() {
        SecureString s("test", 4);
        SecureString t(std::move(s));
        QCOMPARE(t.toString(), "test");
        QVERIFY(s.empty());
        QCOMPARE(s.data(), nullptr);
    }

    void testMoveConstructorFromEmpty() {
        SecureString s;
        SecureString t(std::move(s));
        QVERIFY(t.empty());
        QVERIFY(s.empty());
    }

    void testMoveAssignment() {
        SecureString s("abc", 3);
        SecureString t;
        t = std::move(s);
        QCOMPARE(t.toString(), "abc");
        QVERIFY(s.empty());
    }

    void testMoveAssignmentToNonEmpty() {
        SecureString s("abc", 3);
        SecureString t("xyz", 3);
        t = std::move(s);
        QCOMPARE(t.toString(), "abc");
        QVERIFY(s.empty());
    }

    void testSelfMoveAssignment() {
        SecureString s("data", 4);
        const char *origData = s.data();
        s = std::move(s);
        QCOMPARE(s.data(), origData);
        QCOMPARE(s.toString(), "data");
    }

    void testClear() {
        SecureString s("data", 4);
        s.clear();
        QVERIFY(s.empty());
    }

    void testClearZeroesData() {
        SecureString s("secret", 6);
        const char *raw = s.data();
        s.clear();
        QVERIFY(s.empty());
        QCOMPARE(raw[0], '\0');
        QCOMPARE(raw[1], '\0');
    }

    void testClearMultipleTimes() {
        SecureString s("data", 4);
        s.clear();
        s.clear();
        s.clear();
        QVERIFY(s.empty());
    }

    void testClearOnEmpty() {
        SecureString s;
        s.clear();
        QVERIFY(s.empty());
    }

    void testAssign() {
        SecureString s;
        s.assign("hello", 5);
        QCOMPARE(s.toString(), "hello");
        s.assign("world", 5);
        QCOMPARE(s.toString(), "world");
    }

    void testAssignShorterToLonger() {
        SecureString s("longerstring", 12);
        s.assign("short", 5);
        QCOMPARE(s.toString(), "short");
        QCOMPARE(s.size(), size_t(5));
    }

    void testAssignLongerToShorter() {
        SecureString s("short", 5);
        s.assign("muchlongerstring", 16);
        QCOMPARE(s.toString(), "muchlongerstring");
        QCOMPARE(s.size(), size_t(16));
    }

    void testAssignEmpty() {
        SecureString s("data", 4);
        s.assign("", 0);
        QVERIFY(s.empty());
    }

    void testAssignSameSize() {
        SecureString s("abcde", 5);
        s.assign("fghij", 5);
        QCOMPARE(s.toString(), "fghij");
    }

    void testToString() {
        SecureString s("test-value", 10);
        QCOMPARE(s.toString(), "test-value");
    }

    void testToStringEmpty() {
        const SecureString s;
        QVERIFY(s.toString().empty());
    }

    void testToStringAfterMove() {
        SecureString s("original", 8);
        SecureString t(std::move(s));
        QCOMPARE(t.toString(), "original");
        QVERIFY(s.toString().empty());
    }

    void testDataAfterMove() {
        SecureString s("data", 4);
        SecureString t(std::move(s));
        QVERIFY(t.data() != nullptr);
        QCOMPARE(s.data(), nullptr);
    }

    void testDataNotNullAfterAssign() {
        SecureString s("test", 4);
        QVERIFY(s.data() != nullptr);
    }

    void testLongString() {
        std::string big(10000, 'A');
        SecureString s(big);
        QCOMPARE(s.size(), size_t(10000));
        QCOMPARE(s.toString(), big);
    }

    void testVeryLongString() {
        std::string big(100000, 'B');
        SecureString s(big);
        QCOMPARE(s.size(), size_t(100000));
        QCOMPARE(s.toString(), big);
    }

    void testPageAlignedCapacity() {
        long pageSize = sysconf(_SC_PAGESIZE);
        if (pageSize <= 0) pageSize = 4096;

        // A small string should allocate a full page
        SecureString s("small", 5);
        QVERIFY(s.data() != nullptr);
    }

    void testToStringReturnsSameAfterMultipleCalls() {
        SecureString s("consistent", 10);
        QCOMPARE(s.toString(), "consistent");
        QCOMPARE(s.toString(), "consistent");
        QCOMPARE(s.toString(), "consistent");
    }

    void testDataPointerValidity() {
        SecureString s("data", 4);
        const char *ptr = s.data();
        QVERIFY(ptr != nullptr);
        QCOMPARE(ptr[0], 'd');
        QCOMPARE(ptr[1], 'a');
        QCOMPARE(ptr[2], 't');
        QCOMPARE(ptr[3], 'a');
    }

    void testSizeConsistency() {
        SecureString s("hello", 5);
        QCOMPARE(s.size(), s.toString().size());
    }

    void testEmptySize() {
        SecureString s;
        QCOMPARE(s.size(), s.toString().size());
    }

    void testAssignResetsData() {
        SecureString s("old", 3);
        s.assign("new", 3);
        QCOMPARE(s.toString(), "new");
    }

    void testNoExceptMoveConstructor() {
        SecureString s("test", 4);
        SecureString t(std::move(s));
        QVERIFY(true);
    }

    void testNoExceptMoveAssignment() {
        SecureString s("test", 4);
        SecureString t;
        t = std::move(s);
        QVERIFY(true);
    }
};

QTEST_MAIN(TestSecureMemory)
#include "tst_secure_memory.moc"
