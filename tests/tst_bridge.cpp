#include <QtTest>
#include <QSignalSpy>
#include "pinentry_bridge.h"

class TestPinentryBridge : public QObject {
    Q_OBJECT

private:
    PinentryBridge *bridge;

private slots:
    void initTestCase() {
        bridge = new PinentryBridge(this);
    }

    void testInitialValues() {
        QCOMPARE(bridge->prompt(), "Passphrase:");
        QCOMPARE(bridge->desc(), "Please enter your secret passphrase");
        QCOMPARE(bridge->mode(), "getpin");
        QCOMPARE(bridge->allowCache(), false);
        QCOMPARE(bridge->keyInfo(), "");
        QCOMPARE(bridge->remember(), false);
        QCOMPARE(bridge->pwmngrLabel(), "Save in password manager");
        QCOMPARE(bridge->okText(), "");
        QCOMPARE(bridge->cancelText(), "");
        QCOMPARE(bridge->notSetText(), "");
        QCOMPARE(bridge->errorText(), "");
        QCOMPARE(bridge->titleText(), "");
        QCOMPARE(bridge->timeout(), 0);
        QCOMPARE(bridge->remainingTime(), 0);
        QVERIFY(bridge->securePin().empty());
    }

    void testSetPrompt() {
        QSignalSpy spy(bridge, &PinentryBridge::promptChanged);
        bridge->setPrompt("Enter key:");
        QCOMPARE(bridge->prompt(), "Enter key:");
        QCOMPARE(spy.count(), 1);
    }

    void testSetPromptNoChange() {
        QSignalSpy spy(bridge, &PinentryBridge::promptChanged);
        bridge->setPrompt("Enter key:");
        QCOMPARE(spy.count(), 0);
    }

    void testSetPromptEmpty() {
        QSignalSpy spy(bridge, &PinentryBridge::promptChanged);
        bridge->setPrompt("");
        QCOMPARE(bridge->prompt(), "");
        QCOMPARE(spy.count(), 1);
    }

    void testSetDesc() {
        QSignalSpy spy(bridge, &PinentryBridge::descChanged);
        bridge->setDesc("Please enter your password");
        QCOMPARE(bridge->desc(), "Please enter your password");
        QCOMPARE(spy.count(), 1);
    }

    void testSetDescWithNewlines() {
        bridge->setDesc("Line one\nLine two\nLine three");
        QCOMPARE(bridge->desc(), "Line one\nLine two\nLine three");
    }

    void testModeGetPin() {
        bridge->setMode(PinentryBridge::Mode::Message);
        QSignalSpy spy(bridge, &PinentryBridge::modeChanged);
        bridge->setMode(PinentryBridge::Mode::GetPin);
        QCOMPARE(bridge->mode(), "getpin");
        QCOMPARE(spy.count(), 1);
    }

    void testModeConfirm() {
        QSignalSpy spy(bridge, &PinentryBridge::modeChanged);
        bridge->setMode(PinentryBridge::Mode::Confirm);
        QCOMPARE(bridge->mode(), "confirm");
        QCOMPARE(spy.count(), 1);
    }

    void testModeMessage() {
        QSignalSpy spy(bridge, &PinentryBridge::modeChanged);
        bridge->setMode(PinentryBridge::Mode::Message);
        QCOMPARE(bridge->mode(), "message");
        QCOMPARE(spy.count(), 1);
    }

    void testModeSameNoSignal() {
        QSignalSpy spy(bridge, &PinentryBridge::modeChanged);
        bridge->setMode(PinentryBridge::Mode::Message);
        QCOMPARE(spy.count(), 0);
    }

    void testModeCycleAll() {
        bridge->setMode(PinentryBridge::Mode::GetPin);
        QCOMPARE(bridge->mode(), "getpin");

        bridge->setMode(PinentryBridge::Mode::Confirm);
        QCOMPARE(bridge->mode(), "confirm");

        bridge->setMode(PinentryBridge::Mode::Message);
        QCOMPARE(bridge->mode(), "message");

        bridge->setMode(PinentryBridge::Mode::GetPin);
        QCOMPARE(bridge->mode(), "getpin");
    }

    void testSetAllowCache() {
        QSignalSpy spy(bridge, &PinentryBridge::allowCacheChanged);
        bridge->setAllowCache(true);
        QCOMPARE(bridge->allowCache(), true);
        QCOMPARE(spy.count(), 1);
    }

    void testSetAllowCacheToggle() {
        bridge->setAllowCache(false);
        QCOMPARE(bridge->allowCache(), false);
        bridge->setAllowCache(true);
        QCOMPARE(bridge->allowCache(), true);
        bridge->setAllowCache(false);
        QCOMPARE(bridge->allowCache(), false);
    }

    void testSetKeyInfo() {
        QSignalSpy spy(bridge, &PinentryBridge::keyInfoChanged);
        bridge->setKeyInfo("mykey123");
        QCOMPARE(bridge->keyInfo(), "mykey123");
        QCOMPARE(spy.count(), 1);
    }

    void testSetKeyInfoLong() {
        QString longKey(4096, 'A');
        bridge->setKeyInfo(longKey);
        QCOMPARE(bridge->keyInfo(), longKey);
    }

    void testSetRemember() {
        QSignalSpy spy(bridge, &PinentryBridge::rememberChanged);
        bridge->setRemember(true);
        QCOMPARE(bridge->remember(), true);
        QCOMPARE(spy.count(), 1);
    }

    void testSetPwmngrLabel() {
        QSignalSpy spy(bridge, &PinentryBridge::pwmngrLabelChanged);
        bridge->setPwmngrLabel("Save in keyring");
        QCOMPARE(bridge->pwmngrLabel(), "Save in keyring");
        QCOMPARE(spy.count(), 1);
    }

    void testSetOkText() {
        QSignalSpy spy(bridge, &PinentryBridge::okTextChanged);
        bridge->setOkText("Yes, please");
        QCOMPARE(bridge->okText(), "Yes, please");
        QCOMPARE(spy.count(), 1);
    }

    void testSetOkTextMultiWord() {
        bridge->setOkText("Yes, I agree to the terms and conditions");
        QCOMPARE(bridge->okText(), "Yes, I agree to the terms and conditions");
    }

    void testSetCancelText() {
        QSignalSpy spy(bridge, &PinentryBridge::cancelTextChanged);
        bridge->setCancelText("No, thanks");
        QCOMPARE(bridge->cancelText(), "No, thanks");
        QCOMPARE(spy.count(), 1);
    }

    void testSetNotSetText() {
        QSignalSpy spy(bridge, &PinentryBridge::notSetTextChanged);
        bridge->setNotSetText("Optional info");
        QCOMPARE(bridge->notSetText(), "Optional info");
        QCOMPARE(spy.count(), 1);
    }

    void testSetErrorText() {
        QSignalSpy spy(bridge, &PinentryBridge::errorTextChanged);
        bridge->setErrorText("Something failed");
        QCOMPARE(bridge->errorText(), "Something failed");
        QCOMPARE(spy.count(), 1);
    }

    void testSetTitleText() {
        QSignalSpy spy(bridge, &PinentryBridge::titleTextChanged);
        bridge->setTitleText("Custom Title");
        QCOMPARE(bridge->titleText(), "Custom Title");
        QCOMPARE(spy.count(), 1);
    }

    void testSetTimeout() {
        QSignalSpy spyTimeout(bridge, &PinentryBridge::timeoutChanged);
        QSignalSpy spyRemaining(bridge, &PinentryBridge::remainingTimeChanged);
        bridge->setTimeout(30);
        QCOMPARE(bridge->timeout(), 30);
        QCOMPARE(bridge->remainingTime(), 30);
        QCOMPARE(spyTimeout.count(), 1);
        QCOMPARE(spyRemaining.count(), 1);
    }

    void testSetTimeoutZero() {
        bridge->setTimeout(0);
        QCOMPARE(bridge->timeout(), 0);
        QCOMPARE(bridge->remainingTime(), 0);
    }

    void testSetTimeoutNegative() {
        bridge->setTimeout(-1);
        QCOMPARE(bridge->timeout(), -1);
        QCOMPARE(bridge->remainingTime(), -1);
    }

    void testSetTimeoutSameNoSignal() {
        bridge->setTimeout(60);
        QSignalSpy spy(bridge, &PinentryBridge::timeoutChanged);
        bridge->setTimeout(60);
        QCOMPARE(spy.count(), 0);
    }

    void testSetTimeoutThenSetAgain() {
        bridge->setTimeout(30);
        QSignalSpy spy(bridge, &PinentryBridge::timeoutChanged);
        bridge->setTimeout(60);
        QCOMPARE(bridge->timeout(), 60);
        QCOMPARE(bridge->remainingTime(), 60);
        QCOMPARE(spy.count(), 1);
    }

    void testReset() {
        bridge->setKeyInfo("testkey");
        bridge->setRemember(true);
        bridge->setPrompt("Custom prompt");
        bridge->setDesc("Custom desc");
        bridge->setOkText("OK");
        bridge->setTitleText("Title");
        bridge->setErrorText("Error");
        bridge->setTimeout(30);

        bridge->reset();

        QCOMPARE(bridge->keyInfo(), "testkey");
        QCOMPARE(bridge->prompt(), "Custom prompt");
        QCOMPARE(bridge->desc(), "Custom desc");
        QCOMPARE(bridge->remember(), false);
        QVERIFY(bridge->getResult().isEmpty());
        QCOMPARE(bridge->okText(), "");
        QCOMPARE(bridge->titleText(), "");
        QCOMPARE(bridge->errorText(), "");
        QCOMPARE(bridge->mode(), "getpin");
        QCOMPARE(bridge->timeout(), 0);
        QCOMPARE(bridge->remainingTime(), 0);
        QVERIFY(bridge->securePin().empty());
    }

    void testSubmit() {
        bridge->reset();
        bridge->submit("mypassword");

        QCOMPARE(bridge->getResult(), "mypassword");

        QString result = bridge->getResult();
        QCOMPARE(result, "mypassword");
    }

    void testSubmitEmptyString() {
        bridge->reset();
        bridge->submit("");
        QVERIFY(bridge->getResult().isEmpty());
    }

    void testSubmitAndGetResult() {
        bridge->reset();
        QVERIFY(bridge->getResult().isEmpty());

        bridge->submit("secret123");
        QCOMPARE(bridge->getResult(), "secret123");
    }

    void testCancel() {
        bridge->reset();
        bridge->submit("somevalue");
        QCOMPARE(bridge->getResult(), "somevalue");

        bridge->reset();
        bridge->cancel();
        QVERIFY(bridge->getResult().isEmpty());
    }

    void testMultipleSubmit() {
        bridge->reset();
        bridge->submit("first");
        QCOMPARE(bridge->getResult(), "first");

        bridge->reset();
        bridge->submit("second");
        QCOMPARE(bridge->getResult(), "second");
    }

    void testSubmitSpecialChars() {
        bridge->reset();
        bridge->submit("p@$$w0rd!#$%&'()*+,-./:;<=>?@[]^_`{|}~");
        QCOMPARE(bridge->getResult(), "p@$$w0rd!#$%&'()*+,-./:;<=>?@[]^_`{|}~");
    }

    void testSubmitUnicode() {
        bridge->reset();
        bridge->submit(QString::fromUtf8("pässwörd 🔑"));
        QCOMPARE(bridge->getResult(), QString::fromUtf8("pässwörd 🔑"));
    }

    void testConfirmOk() {
        bridge->reset();
        bridge->setMode(PinentryBridge::Mode::Confirm);
        bridge->confirmOk();
        QCOMPARE(bridge->getConfirmResult(), true);
    }

    void testConfirmCancel() {
        bridge->reset();
        bridge->setMode(PinentryBridge::Mode::Confirm);
        bridge->confirmCancel();
        QCOMPARE(bridge->getConfirmResult(), false);
    }

    void testConfirmOkMultiple() {
        bridge->reset();
        bridge->confirmOk();
        QCOMPARE(bridge->getConfirmResult(), true);
        bridge->confirmOk();
        QCOMPARE(bridge->getConfirmResult(), true);
    }

    void testMessageOk() {
        bridge->reset();
        bridge->setMode(PinentryBridge::Mode::Message);
        bridge->messageOk();
        QVERIFY(true);
    }

    void testCheckKeyringEmptyKeyInfo() {
        bridge->setKeyInfo("");
        QString result = bridge->checkKeyring();
        QVERIFY(result.isEmpty());
    }

    void testCheckKeyringWithoutInit() {
        PinentryBridge tmp;
        QString result = tmp.checkKeyring();
        QVERIFY(result.isEmpty());
    }

    void testSecurePinEmptyBeforeSubmit() {
        bridge->reset();
        QVERIFY(bridge->securePin().empty());
    }

    void testSecurePinAfterSubmit() {
        bridge->reset();
        bridge->submit("hunter2");
        const SecureString &pin = bridge->securePin();
        QCOMPARE(pin.toString(), "hunter2");
        QCOMPARE(pin.size(), size_t(7));
    }

    void testSecurePinAfterSubmitEmpty() {
        bridge->reset();
        bridge->submit("");
        QVERIFY(bridge->securePin().empty());
    }

    void testSecurePinClearedOnCancel() {
        bridge->reset();
        bridge->submit("secret");
        bridge->cancel();
        const SecureString &pin = bridge->securePin();
        QVERIFY(pin.empty());
    }

    void testSecurePinLongPassword() {
        bridge->reset();
        QString longPin(5000, 'X');
        bridge->submit(longPin);
        const SecureString &pin = bridge->securePin();
        QCOMPARE(pin.size(), size_t(5000));
        QCOMPARE(pin.toString(), longPin.toStdString());
    }

    void testMultipleOperationsSequence() {
        bridge->reset();

        bridge->setPrompt("PIN:");
        bridge->setDesc("Enter your PIN");
        QCOMPARE(bridge->prompt(), "PIN:");
        QCOMPARE(bridge->desc(), "Enter your PIN");

        bridge->submit("1234");
        QCOMPARE(bridge->getResult(), "1234");

        bridge->reset();
        QVERIFY(bridge->getResult().isEmpty());
        QVERIFY(bridge->securePin().empty());

        bridge->setMode(PinentryBridge::Mode::Confirm);
        bridge->confirmOk();
        QCOMPARE(bridge->getConfirmResult(), true);

        bridge->reset();
        bridge->setMode(PinentryBridge::Mode::Message);
        bridge->messageOk();

        bridge->reset();
        QCOMPARE(bridge->mode(), "getpin");
    }

    void testSetThenGetResultAfterReset() {
        bridge->reset();
        bridge->submit("first");
        bridge->reset();
        QVERIFY(bridge->getResult().isEmpty());
        bridge->submit("second");
        QCOMPARE(bridge->getResult(), "second");
    }

    void cleanupTestCase() {
        delete bridge;
    }
};

QTEST_MAIN(TestPinentryBridge)
#include "tst_bridge.moc"
