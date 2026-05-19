#include <QtTest>
#include <QSignalSpy>
#include "pinentry_bridge.h"

class TestProtocolFlow : public QObject {
    Q_OBJECT

private:
    PinentryBridge *bridge;

    void simulateGetPinSetup() {
        bridge->setMode(PinentryBridge::Mode::GetPin);
        bridge->setPrompt("Enter passphrase:");
        bridge->setDesc("Please enter your secret passphrase");
    }

    void simulateConfirmSetup() {
        bridge->setMode(PinentryBridge::Mode::Confirm);
        bridge->setDesc("Do you trust this key?");
        bridge->setOkText("Trust");
        bridge->setCancelText("Reject");
    }

    void simulateMessageSetup() {
        bridge->setMode(PinentryBridge::Mode::Message);
        bridge->setDesc("Operation completed successfully");
        bridge->setOkText("Dismiss");
    }

private slots:
    void initTestCase() {
        bridge = new PinentryBridge(this);
    }

    // Full GETPIN submit flow
    void testGetpinSubmitFlow() {
        bridge->reset();
        simulateGetPinSetup();
        QCOMPARE(bridge->mode(), "getpin");
        QCOMPARE(bridge->prompt(), "Enter passphrase:");
        QCOMPARE(bridge->desc(), "Please enter your secret passphrase");

        bridge->submit("my_secret_passphrase");
        QCOMPARE(bridge->getResult(), "my_secret_passphrase");
        QCOMPARE(bridge->securePin().toString(), "my_secret_passphrase");
    }

    // Full GETPIN cancel flow
    void testGetpinCancelFlow() {
        bridge->reset();
        simulateGetPinSetup();
        bridge->cancel();
        QVERIFY(bridge->getResult().isEmpty());
        QVERIFY(bridge->securePin().empty());
    }

    // Full CONFIRM approve flow
    void testConfirmApproveFlow() {
        bridge->reset();
        simulateConfirmSetup();
        QCOMPARE(bridge->mode(), "confirm");
        QCOMPARE(bridge->desc(), "Do you trust this key?");
        QCOMPARE(bridge->okText(), "Trust");
        QCOMPARE(bridge->cancelText(), "Reject");

        bridge->confirmOk();
        QCOMPARE(bridge->getConfirmResult(), true);
    }

    // Full CONFIRM reject flow
    void testConfirmRejectFlow() {
        bridge->reset();
        simulateConfirmSetup();
        bridge->confirmCancel();
        QCOMPARE(bridge->getConfirmResult(), false);
    }

    // Full CONFIRM with default text
    void testConfirmDefaultText() {
        bridge->reset();
        bridge->setMode(PinentryBridge::Mode::Confirm);
        bridge->setDesc("Is this okay?");
        QCOMPARE(bridge->okText(), "");
        QCOMPARE(bridge->cancelText(), "");

        bridge->confirmOk();
        QCOMPARE(bridge->getConfirmResult(), true);
    }

    // Full MESSAGE flow
    void testMessageFlow() {
        bridge->reset();
        simulateMessageSetup();
        QCOMPARE(bridge->mode(), "message");
        QCOMPARE(bridge->desc(), "Operation completed successfully");
        QCOMPARE(bridge->okText(), "Dismiss");

        bridge->messageOk();
        QVERIFY(true);
    }

    // MESSAGE with error text
    void testMessageWithErrorFlow() {
        bridge->reset();
        bridge->setMode(PinentryBridge::Mode::Message);
        bridge->setDesc("Authentication failed");
        bridge->setErrorText("Invalid passphrase provided");
        bridge->setTitleText("Error");
        QCOMPARE(bridge->errorText(), "Invalid passphrase provided");
        QCOMPARE(bridge->titleText(), "Error");

        bridge->messageOk();
        QVERIFY(true);
    }

    // GETPIN with keyinfo and remember
    void testGetpinWithRememberFlow() {
        bridge->reset();
        bridge->setKeyInfo("mykey123");
        bridge->setAllowCache(true);
        bridge->setRemember(true);
        bridge->submit("testpass");

        QCOMPARE(bridge->getResult(), "testpass");
        QCOMPARE(bridge->remember(), true);
        QCOMPARE(bridge->keyInfo(), "mykey123");
    }

    // GETPIN with timeout set
    void testGetpinWithTimeout() {
        bridge->reset();
        bridge->setTimeout(30);
        bridge->setPrompt("Quick PIN:");
        bridge->submit("1234");

        QCOMPARE(bridge->getResult(), "1234");
        QCOMPARE(bridge->timeout(), 30);
    }

    // Full cycle: GETPIN -> CONFIRM -> MESSAGE
    void testOperationCycle() {
        bridge->reset();
        simulateGetPinSetup();
        bridge->submit("password1");
        QCOMPARE(bridge->getResult(), "password1");

        bridge->reset();
        simulateConfirmSetup();
        bridge->confirmOk();
        QCOMPARE(bridge->getConfirmResult(), true);

        bridge->reset();
        simulateMessageSetup();
        bridge->messageOk();
        QCOMPARE(bridge->mode(), "message");
        bridge->reset();
        QCOMPARE(bridge->mode(), "getpin");
    }

    // GETPIN using a fresh bridge with default values
    void testGetpinDefaults() {
        PinentryBridge freshBridge;
        freshBridge.setPrompt("Passphrase:");
        freshBridge.setDesc("Please enter your secret passphrase");
        QCOMPARE(freshBridge.prompt(), "Passphrase:");
        QCOMPARE(freshBridge.desc(), "Please enter your secret passphrase");
    }

    // SETKEYINFO persists across reset
    void testKeyInfoPersistenceAcrossReset() {
        bridge->reset();
        bridge->setKeyInfo("persistent_key");
        bridge->reset();
        QCOMPARE(bridge->keyInfo(), "persistent_key");
    }

    // Multiple SETDESC calls, last one wins
    void testMultipleSetDesc() {
        bridge->reset();
        bridge->setDesc("First description");
        bridge->setDesc("Second description");
        bridge->setDesc("Final description");
        QCOMPARE(bridge->desc(), "Final description");
    }

    // GETPIN with empty password
    void testGetpinEmptyPassword() {
        bridge->reset();
        bridge->submit("");
        QVERIFY(bridge->getResult().isEmpty());
    }

    // CONFIRM with custom title and all text fields
    void testConfirmFullCustomization() {
        bridge->reset();
        bridge->setMode(PinentryBridge::Mode::Confirm);
        bridge->setTitleText("Security Check");
        bridge->setDesc("Do you want to allow this operation?");
        bridge->setOkText("Allow");
        bridge->setCancelText("Deny");
        bridge->setNotSetText("Remember my choice");

        QCOMPARE(bridge->titleText(), "Security Check");
        QCOMPARE(bridge->desc(), "Do you want to allow this operation?");
        QCOMPARE(bridge->okText(), "Allow");
        QCOMPARE(bridge->cancelText(), "Deny");
        QCOMPARE(bridge->notSetText(), "Remember my choice");

        bridge->confirmOk();
        QCOMPARE(bridge->getConfirmResult(), true);
    }

    void cleanupTestCase() {
        delete bridge;
    }
};

QTEST_MAIN(TestProtocolFlow)
#include "tst_protocol_flow.moc"
