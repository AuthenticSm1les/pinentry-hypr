#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QTimer>
#include <iostream>
#include <string>
#include <thread>

#include "assuan_protocol.h"
#include "pinentry_bridge.h"
#include "curses_fallback.h"

PinentryBridge* g_bridge = nullptr;
QQmlApplicationEngine* g_engine = nullptr;

static bool useFallback() {
    return cursesAvailable() && qEnvironmentVariableIsEmpty("WAYLAND_DISPLAY");
}

static bool useQtGui() {
    return !useFallback();
}

static void showQmlWindow() {
    QMetaObject::invokeMethod(qApp, []() {
        g_engine = new QQmlApplicationEngine();
        g_engine->rootContext()->setContextProperty("bridge", g_bridge);
        g_engine->load(QUrl(QStringLiteral("qrc:/PinentryHypr/Main.qml")));
    });
}

static void destroyQmlWindow() {
    QMetaObject::invokeMethod(qApp, []() {
        delete g_engine;
        g_engine = nullptr;
    });
}

void handleGetPin() {
    g_bridge->reset();
    g_bridge->setMode(PinentryBridge::Mode::GetPin);

    // Check keyring first
    QString cached = g_bridge->checkKeyring();
    if (!cached.isEmpty()) {
        std::cout << "D " << cached.toStdString() << std::endl;
        std::cout << "OK" << std::endl;
        return;
    }

    if (useFallback()) {
        std::string result;
        bool ok = cursesGetpin(
            g_bridge->prompt().toStdString(),
            g_bridge->desc().toStdString(),
            result,
            g_bridge->timeout()
        );
        if (ok && !result.empty()) {
            std::cout << "D " << result << std::endl;
            std::cout << "OK" << std::endl;
        } else {
            std::cout << "ERR 83886179 Operation cancelled" << std::endl;
        }
        return;
    }

    showQmlWindow();
    g_bridge->waitForDone();
    destroyQmlWindow();

    if (!g_bridge->getResult().isEmpty()) {
        std::cout << "D " << g_bridge->getResult().toStdString() << std::endl;
        std::cout << "OK" << std::endl;
    } else {
        std::cout << "ERR 83886179 Operation cancelled" << std::endl;
    }
}

void handleConfirm() {
    g_bridge->reset();
    g_bridge->setMode(PinentryBridge::Mode::Confirm);

    if (useFallback()) {
        bool ok = cursesConfirm(
            g_bridge->desc().toStdString(),
            g_bridge->okText().toStdString(),
            g_bridge->cancelText().toStdString(),
            g_bridge->timeout()
        );
        if (ok) {
            std::cout << "OK" << std::endl;
        } else {
            std::cout << "ERR 83886179 Operation cancelled" << std::endl;
        }
        return;
    }

    showQmlWindow();
    g_bridge->waitForDone();
    destroyQmlWindow();

    if (g_bridge->getConfirmResult()) {
        std::cout << "OK" << std::endl;
    } else {
        std::cout << "ERR 83886179 Operation cancelled" << std::endl;
    }
}

void handleMessage() {
    g_bridge->reset();
    g_bridge->setMode(PinentryBridge::Mode::Message);

    if (useFallback()) {
        cursesMessage(
            g_bridge->desc().toStdString(),
            g_bridge->okText().toStdString(),
            g_bridge->timeout()
        );
        std::cout << "OK" << std::endl;
        return;
    }

    showQmlWindow();
    g_bridge->waitForDone();
    destroyQmlWindow();

    std::cout << "OK" << std::endl;
}

void handleCommand(const AssuanLine &parsed, std::ostream &out = std::cout) {
    switch (parsed.cmd) {
    case AssuanCmd::SETPROMPT:
        g_bridge->setPrompt(QString::fromStdString(parsed.value));
        out << "OK" << std::endl;
        break;
    case AssuanCmd::SETDESC:
        g_bridge->setDesc(QString::fromStdString(parsed.value));
        out << "OK" << std::endl;
        break;
    case AssuanCmd::SETKEYINFO:
        g_bridge->setKeyInfo(QString::fromStdString(parsed.value));
        out << "OK" << std::endl;
        break;
    case AssuanCmd::SETTIMEOUT:
        {
            int secs = 0;
            try { secs = std::stoi(parsed.value); } catch (...) {}
            g_bridge->setTimeout(secs);
            out << "OK" << std::endl;
        }
        break;
    case AssuanCmd::SETOK:
        g_bridge->setOkText(QString::fromStdString(parsed.value));
        out << "OK" << std::endl;
        break;
    case AssuanCmd::SETCANCEL:
        g_bridge->setCancelText(QString::fromStdString(parsed.value));
        out << "OK" << std::endl;
        break;
    case AssuanCmd::SETNOTSET:
        g_bridge->setNotSetText(QString::fromStdString(parsed.value));
        out << "OK" << std::endl;
        break;
    case AssuanCmd::SETERROR:
        g_bridge->setErrorText(QString::fromStdString(parsed.value));
        out << "OK" << std::endl;
        break;
    case AssuanCmd::SETTITLE:
        g_bridge->setTitleText(QString::fromStdString(parsed.value));
        out << "OK" << std::endl;
        break;
    case AssuanCmd::OPTION:
        if (hasKeyInfoOption(parsed.value)) {
            g_bridge->setAllowCache(true);
        } else {
            std::string pwmngr = getPwmngrValue(parsed.value);
            if (!pwmngr.empty()) {
                g_bridge->setPwmngrLabel(QString::fromStdString(pwmngr));
            }
        }
        out << "OK" << std::endl;
        break;
    case AssuanCmd::GETPIN:
        handleGetPin();
        break;
    case AssuanCmd::CONFIRM:
        handleConfirm();
        break;
    case AssuanCmd::MESSAGE:
        handleMessage();
        break;
    case AssuanCmd::BYE:
        out << "OK" << std::endl;
        QCoreApplication::quit();
        break;
    default:
        out << "OK" << std::endl;
        break;
    }
}

void assuan_loop() {
    std::cout << "OK Pleased to meet you" << std::endl;
    std::string line;
    while (std::getline(std::cin, line)) {
        AssuanLine parsed = parseAssuanLine(line);
        handleCommand(parsed);
        if (parsed.cmd == AssuanCmd::BYE) return;
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Hyprland Pinentry");
    app.setDesktopFileName("pinentry-hypr");

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE"))
        QQuickStyle::setStyle("org.hyprland.style");

    g_bridge = new PinentryBridge(&app);

    std::thread t(assuan_loop);
    t.detach();

    return app.exec();
}
