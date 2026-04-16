#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QObject>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

// Fix GLib/Qt conflict
#undef signals
#include <libsecret/secret.h>
#define signals Q_SIGNALS

// Define the GnuPG Pinentry schema for libsecret
static const SecretSchema * get_pinentry_schema() {
    static const SecretSchema schema = {
        "org.gnupg.Pinentry",
        SECRET_SCHEMA_NONE,
        {
            { "keyinfo", SECRET_SCHEMA_ATTRIBUTE_STRING },
            { nullptr, SECRET_SCHEMA_ATTRIBUTE_STRING }
        }
    };
    return &schema;
}

class PinentryBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString prompt READ prompt WRITE setPrompt NOTIFY promptChanged)
    Q_PROPERTY(QString desc READ desc WRITE setDesc NOTIFY descChanged)
    Q_PROPERTY(bool allowCache READ allowCache WRITE setAllowCache NOTIFY allowCacheChanged)
    Q_PROPERTY(QString keyInfo READ keyInfo WRITE setKeyInfo NOTIFY keyInfoChanged)
    Q_PROPERTY(bool remember READ remember WRITE setRemember NOTIFY rememberChanged)
    Q_PROPERTY(QString pwmngrLabel READ pwmngrLabel WRITE setPwmngrLabel NOTIFY pwmngrLabelChanged)

public:
    explicit PinentryBridge(QObject *parent = nullptr) : QObject(parent) {}

    QString prompt() const { return m_prompt; }
    void setPrompt(const QString &p) { if (m_prompt != p) { m_prompt = p; emit promptChanged(); } }

    QString desc() const { return m_desc; }
    void setDesc(const QString &d) { if (m_desc != d) { m_desc = d; emit descChanged(); } }

    bool allowCache() const { return m_allowCache; }
    void setAllowCache(bool a) { if (m_allowCache != a) { m_allowCache = a; emit allowCacheChanged(); } }

    QString keyInfo() const { return m_keyInfo; }
    void setKeyInfo(const QString &k) { if (m_keyInfo != k) { m_keyInfo = k; emit keyInfoChanged(); } }

    bool remember() const { return m_remember; }
    void setRemember(bool r) { if (m_remember != r) { m_remember = r; emit rememberChanged(); } }

    QString pwmngrLabel() const { return m_pwmngrLabel; }
    void setPwmngrLabel(const QString &l) { if (m_pwmngrLabel != l) { m_pwmngrLabel = l; emit pwmngrLabelChanged(); } }

    Q_INVOKABLE void submit(const QString &pin) {
        m_result = pin;
        if (m_remember && !m_keyInfo.isEmpty()) {
            saveToKeyring(pin);
        }
        m_done = true;
        m_cv.notify_all();
    }

    Q_INVOKABLE void cancel() {
        m_result = "";
        m_done = true;
        m_cv.notify_all();
    }

    Q_INVOKABLE QString checkKeyring() {
        if (m_keyInfo.isEmpty()) return "";
        
        GError *error = nullptr;
        char *password = secret_password_lookup_sync(
            get_pinentry_schema(),
            nullptr, &error,
            "keyinfo", m_keyInfo.toUtf8().constData(),
            nullptr
        );

        if (error) {
            g_error_free(error);
            return "";
        }

        if (password) {
            QString result = QString::fromUtf8(password);
            secret_password_free(password);
            return result;
        }
        return "";
    }

    void saveToKeyring(const QString &pin) {
        GError *error = nullptr;
        secret_password_store_sync(
            get_pinentry_schema(),
            SECRET_COLLECTION_DEFAULT,
            "GnuPG Passphrase",
            pin.toUtf8().constData(),
            nullptr, &error,
            "keyinfo", m_keyInfo.toUtf8().constData(),
            nullptr
        );
        if (error) g_error_free(error);
    }

    QString getResult() { return m_result; }
    void reset() { m_done = false; m_result = ""; m_remember = false; }
    void waitForDone() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return m_done; });
    }

signals:
    void promptChanged();
    void descChanged();
    void allowCacheChanged();
    void keyInfoChanged();
    void rememberChanged();
    void pwmngrLabelChanged();

private:
    QString m_prompt = "Passphrase:";
    QString m_desc = "Please enter your secret passphrase";
    bool m_allowCache = false;
    QString m_keyInfo = "";
    bool m_remember = false;
    QString m_pwmngrLabel = "Save in password manager";
    QString m_result = "";
    bool m_done = false;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

PinentryBridge* g_bridge = nullptr;
QQmlApplicationEngine* g_engine = nullptr;

void assuan_loop() {
    std::cout << "OK Pleased to meet you" << std::endl;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.substr(0, 9) == "SETPROMPT") {
            g_bridge->setPrompt(QString::fromStdString(line.substr(10)));
            std::cout << "OK" << std::endl;
        } else if (line.substr(0, 7) == "SETDESC") {
            g_bridge->setDesc(QString::fromStdString(line.substr(8)));
            std::cout << "OK" << std::endl;
        } else if (line.substr(0, 10) == "SETKEYINFO") {
            g_bridge->setKeyInfo(QString::fromStdString(line.substr(11)));
            std::cout << "OK" << std::endl;
        } else if (line.substr(0, 6) == "OPTION") {
            if (line.find("allow-external-password-cache") != std::string::npos) {
                g_bridge->setAllowCache(true);
            } else if (line.find("default-pwmngr=") != std::string::npos) {
                g_bridge->setPwmngrLabel(QString::fromStdString(line.substr(line.find("=") + 1)));
            }
            std::cout << "OK" << std::endl;
        } else if (line.substr(0, 6) == "GETPIN") {
            g_bridge->reset();
            
            // Check keyring first
            QString cached = g_bridge->checkKeyring();
            if (!cached.isEmpty()) {
                std::cout << "D " << cached.toStdString() << std::endl;
                std::cout << "OK" << std::endl;
                continue;
            }

            QMetaObject::invokeMethod(qApp, []() {
                g_engine = new QQmlApplicationEngine();
                g_engine->rootContext()->setContextProperty("bridge", g_bridge);
                g_engine->load(QUrl(QStringLiteral("qrc:/PinentryHypr/Main.qml")));
            });

            g_bridge->waitForDone();

            QMetaObject::invokeMethod(qApp, []() {
                delete g_engine;
                g_engine = nullptr;
            });

            if (!g_bridge->getResult().isEmpty()) {
                std::cout << "D " << g_bridge->getResult().toStdString() << std::endl;
                std::cout << "OK" << std::endl;
            } else {
                std::cout << "ERR 83886179 Operation cancelled" << std::endl;
            }
        } else if (line.substr(0, 3) == "BYE") {
            std::cout << "OK" << std::endl;
            QCoreApplication::quit();
            return;
        } else {
            std::cout << "OK" << std::endl;
        }
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

#include "main.moc"
