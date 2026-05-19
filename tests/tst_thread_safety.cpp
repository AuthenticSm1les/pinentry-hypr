#include <QtTest>
#include "pinentry_bridge.h"
#include <thread>
#include <vector>
#include <atomic>

class TestThreadSafety : public QObject {
    Q_OBJECT

private:
    PinentryBridge *bridge;

private slots:
    void initTestCase() {
        bridge = new PinentryBridge(this);
    }

    // Concurrent set/get of prompt
    void testConcurrentSetGetPrompt() {
        std::atomic<bool> failed{false};
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([this, i, &failed]() {
                for (int j = 0; j < 100; ++j) {
                    QString val = QString("prompt_%1_%2").arg(i).arg(j);
                    bridge->setPrompt(val);
                    QString read = bridge->prompt();
                    if (read.isEmpty()) {
                        failed = true;
                    }
                }
            });
        }

        for (auto &t : threads) t.join();
        QVERIFY(!failed.load());
    }

    // Concurrent submit/getResult
    void testConcurrentSubmitGetResult() {
        std::atomic<bool> failed{false};
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([this, i, &failed]() {
                for (int j = 0; j < 50; ++j) {
                    QString pin = QString("pin_%1_%2").arg(i).arg(j);
                    bridge->submit(pin);
                    QString result = bridge->getResult();
                    if (result.isEmpty()) {
                        failed = true;
                    }
                }
            });
        }

        for (auto &t : threads) t.join();
        QVERIFY(!failed.load());
    }

    // Concurrent setDesc/desc
    void testConcurrentSetGetDesc() {
        std::atomic<bool> failed{false};
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([this, i, &failed]() {
                for (int j = 0; j < 100; ++j) {
                    QString val = QString("desc_%1_%2").arg(i).arg(j);
                    bridge->setDesc(val);
                    QString read = bridge->desc();
                    if (read.isNull()) {
                        failed = true;
                    }
                }
            });
        }

        for (auto &t : threads) t.join();
        QVERIFY(!failed.load());
    }

    // Concurrent setKeyInfo/keyInfo
    void testConcurrentSetGetKeyInfo() {
        std::atomic<bool> failed{false};
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([this, i, &failed]() {
                for (int j = 0; j < 100; ++j) {
                    QString val = QString("key_%1_%2").arg(i).arg(j);
                    bridge->setKeyInfo(val);
                    QString read = bridge->keyInfo();
                    if (read.isNull()) {
                        failed = true;
                    }
                }
            });
        }

        for (auto &t : threads) t.join();
        QVERIFY(!failed.load());
    }

    // Concurrent confirmOk/getConfirmResult
    void testConcurrentConfirmOk() {
        std::atomic<bool> failed{false};
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([this, &failed]() {
                for (int j = 0; j < 50; ++j) {
                    bridge->confirmOk();
                    bool result = bridge->getConfirmResult();
                    Q_UNUSED(result);
                }
            });
        }

        for (auto &t : threads) t.join();
        QVERIFY(!failed.load());
    }

    // Concurrent mode switch
    void testConcurrentModeSwitch() {
        std::atomic<bool> failed{false};
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([this, i, &failed]() {
                for (int j = 0; j < 100; ++j) {
                    PinentryBridge::Mode mode;
                    switch ((i + j) % 3) {
                    case 0: mode = PinentryBridge::Mode::GetPin; break;
                    case 1: mode = PinentryBridge::Mode::Confirm; break;
                    case 2: mode = PinentryBridge::Mode::Message; break;
                    }
                    bridge->setMode(mode);
                    QString m = bridge->mode();
                    if (m != "getpin" && m != "confirm" && m != "message") {
                        failed = true;
                    }
                }
            });
        }

        for (auto &t : threads) t.join();
        QVERIFY(!failed.load());
    }

    // Concurrent cancel and submit
    void testConcurrentCancelSubmit() {
        std::atomic<bool> failed{false};
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([this, &failed]() {
                for (int j = 0; j < 50; ++j) {
                    if (j % 2 == 0) {
                        bridge->submit("test");
                    } else {
                        bridge->cancel();
                    }
                }
            });
        }

        for (auto &t : threads) t.join();
        QVERIFY(!failed.load());
    }

    // Concurrent setTimeout/timeout
    void testConcurrentSetTimeout() {
        std::atomic<bool> failed{false};
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([this, i, &failed]() {
                for (int j = 0; j < 50; ++j) {
                    bridge->setTimeout((i * 50 + j) % 100);
                    int t = bridge->timeout();
                    Q_UNUSED(t);
                }
            });
        }

        for (auto &t : threads) t.join();
        QVERIFY(!failed.load());
    }

    // All properties concurrently
    void testConcurrentAllProperties() {
        std::atomic<bool> failed{false};
        std::vector<std::thread> threads;

        auto stress = [this, &failed](int id) {
            Q_UNUSED(id);
            for (int j = 0; j < 50; ++j) {
                bridge->setPrompt(QString("p%1").arg(j));
                bridge->setDesc(QString("d%1").arg(j));
                bridge->setKeyInfo(QString("k%1").arg(j));
                bridge->setOkText(QString("o%1").arg(j));
                bridge->setCancelText(QString("c%1").arg(j));
                bridge->setErrorText(QString("e%1").arg(j));
                bridge->setTitleText(QString("t%1").arg(j));
                bridge->setAllowCache(j % 2 == 0);
                bridge->setTimeout(j);

                bridge->prompt();
                bridge->desc();
                bridge->keyInfo();
                bridge->okText();
                bridge->cancelText();
                bridge->errorText();
                bridge->titleText();
                bridge->allowCache();
                bridge->timeout();
                bridge->remainingTime();
            }
        };

        for (int i = 0; i < 8; ++i) {
            threads.emplace_back(stress, i);
        }

        for (auto &t : threads) t.join();
        QVERIFY(!failed.load());
    }

    void cleanupTestCase() {
        delete bridge;
    }
};

QTEST_MAIN(TestThreadSafety)
#include "tst_thread_safety.moc"
