#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <mutex>
#include <condition_variable>
#include <string>

// Fix GLib/Qt conflict
#undef signals
#include <libsecret/secret.h>
#define signals Q_SIGNALS

#include "secure_memory.h"

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
    Q_PROPERTY(QString mode READ mode NOTIFY modeChanged)
    Q_PROPERTY(bool allowCache READ allowCache WRITE setAllowCache NOTIFY allowCacheChanged)
    Q_PROPERTY(QString keyInfo READ keyInfo WRITE setKeyInfo NOTIFY keyInfoChanged)
    Q_PROPERTY(bool remember READ remember WRITE setRemember NOTIFY rememberChanged)
    Q_PROPERTY(QString pwmngrLabel READ pwmngrLabel WRITE setPwmngrLabel NOTIFY pwmngrLabelChanged)
    Q_PROPERTY(QString okText READ okText WRITE setOkText NOTIFY okTextChanged)
    Q_PROPERTY(QString cancelText READ cancelText WRITE setCancelText NOTIFY cancelTextChanged)
    Q_PROPERTY(QString notSetText READ notSetText WRITE setNotSetText NOTIFY notSetTextChanged)
    Q_PROPERTY(QString errorText READ errorText WRITE setErrorText NOTIFY errorTextChanged)
    Q_PROPERTY(QString titleText READ titleText WRITE setTitleText NOTIFY titleTextChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
    Q_PROPERTY(int remainingTime READ remainingTime NOTIFY remainingTimeChanged)

public:
    enum class Mode { GetPin, Confirm, Message };
    Q_ENUM(Mode)

    explicit PinentryBridge(QObject *parent = nullptr);

    QString prompt() const { std::lock_guard<std::mutex> lock(m_mutex); return m_prompt; }
    void setPrompt(const QString &p) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_prompt != p) { m_prompt = p; emit promptChanged(); }
    }

    QString desc() const { std::lock_guard<std::mutex> lock(m_mutex); return m_desc; }
    void setDesc(const QString &d) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_desc != d) { m_desc = d; emit descChanged(); }
    }

    QString mode() const { std::lock_guard<std::mutex> lock(m_mutex); return m_mode; }
    void setMode(Mode m) {
        std::lock_guard<std::mutex> lock(m_mutex);
        QString newMode;
        switch (m) {
        case Mode::GetPin: newMode = "getpin"; break;
        case Mode::Confirm: newMode = "confirm"; break;
        case Mode::Message: newMode = "message"; break;
        }
        if (m_mode != newMode) { m_mode = newMode; emit modeChanged(); }
    }

    bool allowCache() const { std::lock_guard<std::mutex> lock(m_mutex); return m_allowCache; }
    void setAllowCache(bool a) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_allowCache != a) { m_allowCache = a; emit allowCacheChanged(); }
    }

    QString keyInfo() const { std::lock_guard<std::mutex> lock(m_mutex); return m_keyInfo; }
    void setKeyInfo(const QString &k) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_keyInfo != k) { m_keyInfo = k; emit keyInfoChanged(); }
    }

    bool remember() const { std::lock_guard<std::mutex> lock(m_mutex); return m_remember; }
    void setRemember(bool r) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_remember != r) { m_remember = r; emit rememberChanged(); }
    }

    QString pwmngrLabel() const { std::lock_guard<std::mutex> lock(m_mutex); return m_pwmngrLabel; }
    void setPwmngrLabel(const QString &l) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_pwmngrLabel != l) { m_pwmngrLabel = l; emit pwmngrLabelChanged(); }
    }

    QString okText() const { std::lock_guard<std::mutex> lock(m_mutex); return m_okText; }
    void setOkText(const QString &t) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_okText != t) { m_okText = t; emit okTextChanged(); }
    }

    QString cancelText() const { std::lock_guard<std::mutex> lock(m_mutex); return m_cancelText; }
    void setCancelText(const QString &t) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_cancelText != t) { m_cancelText = t; emit cancelTextChanged(); }
    }

    QString notSetText() const { std::lock_guard<std::mutex> lock(m_mutex); return m_notSetText; }
    void setNotSetText(const QString &t) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_notSetText != t) { m_notSetText = t; emit notSetTextChanged(); }
    }

    QString errorText() const { std::lock_guard<std::mutex> lock(m_mutex); return m_errorText; }
    void setErrorText(const QString &t) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_errorText != t) { m_errorText = t; emit errorTextChanged(); }
    }

    QString titleText() const { std::lock_guard<std::mutex> lock(m_mutex); return m_titleText; }
    void setTitleText(const QString &t) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_titleText != t) { m_titleText = t; emit titleTextChanged(); }
    }

    int timeout() const { std::lock_guard<std::mutex> lock(m_mutex); return m_timeout; }
    void setTimeout(int seconds) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_timeout != seconds) {
                m_timeout = seconds;
                m_remainingTime = seconds;
                emit timeoutChanged();
                emit remainingTimeChanged();
            }
        }
        startTimer();
    }

    int remainingTime() const { std::lock_guard<std::mutex> lock(m_mutex); return m_remainingTime; }

    const SecureString &securePin() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_securePin;
    }

    Q_INVOKABLE void submit(const QString &pin) {
        bool shouldSave = false;
        bool allocOk = true;
        QString keyInfoCopy;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            try {
                m_securePin.assign(pin.toUtf8().constData(), pin.toUtf8().size());
            } catch (const std::bad_alloc &) {
                allocOk = false;
            }
            if (allocOk) {
                m_result = pin;
                shouldSave = m_remember && !m_keyInfo.isEmpty();
                if (shouldSave) keyInfoCopy = m_keyInfo;
            } else {
                m_result = "";
            }
            m_done = true;
        }
        m_cv.notify_all();
        if (shouldSave && allocOk) {
            saveToKeyring(pin, keyInfoCopy);
        }
    }

    Q_INVOKABLE void cancel() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_result = "";
            m_securePin.clear();
            m_done = true;
        }
        m_cv.notify_all();
    }

    Q_INVOKABLE void confirmOk() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_confirmResult = true;
            m_done = true;
        }
        m_cv.notify_all();
    }

    Q_INVOKABLE void confirmCancel() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_confirmResult = false;
            m_done = true;
        }
        m_cv.notify_all();
    }

    Q_INVOKABLE void messageOk() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_done = true;
        }
        m_cv.notify_all();
    }

    Q_INVOKABLE QString checkKeyring() {
        QString keyInfoCopy;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            keyInfoCopy = m_keyInfo;
        }
        if (keyInfoCopy.isEmpty()) return "";

        GError *error = nullptr;
        char *password = secret_password_lookup_sync(
            get_pinentry_schema(),
            nullptr, &error,
            "keyinfo", keyInfoCopy.toUtf8().constData(),
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

    void saveToKeyring(const QString &pin, const QString &keyInfo) {
        GError *error = nullptr;
        secret_password_store_sync(
            get_pinentry_schema(),
            SECRET_COLLECTION_DEFAULT,
            "GnuPG Passphrase",
            pin.toUtf8().constData(),
            nullptr, &error,
            "keyinfo", keyInfo.toUtf8().constData(),
            nullptr
        );
        if (error) g_error_free(error);
    }

    QString getResult() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_result;
    }

    bool getConfirmResult() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_confirmResult;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_done = false;
        m_result = "";
        m_securePin.clear();
        m_remember = false;
        m_confirmResult = false;
        m_mode = "getpin";
        m_okText = "";
        m_cancelText = "";
        m_notSetText = "";
        m_errorText = "";
        m_titleText = "";
        m_timeout = 0;
        m_remainingTime = 0;
        stopTimer();
    }

    void waitForDone() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return m_done; });
    }

signals:
    void promptChanged();
    void descChanged();
    void modeChanged();
    void allowCacheChanged();
    void keyInfoChanged();
    void rememberChanged();
    void pwmngrLabelChanged();
    void okTextChanged();
    void cancelTextChanged();
    void notSetTextChanged();
    void errorTextChanged();
    void titleTextChanged();
    void timeoutChanged();
    void remainingTimeChanged();

private slots:
    void onTimeoutTick() {
        bool expired = false;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_timeout <= 0) return;
            if (m_remainingTime > 0) {
                m_remainingTime--;
                emit remainingTimeChanged();
                if (m_remainingTime <= 0) {
                    expired = true;
                    m_done = true;
                }
            }
        }
        if (expired) {
            stopTimerDirect();
            m_cv.notify_all();
        }
    }

private:
    QString m_prompt = "Passphrase:";
    QString m_desc = "Please enter your secret passphrase";
    QString m_mode = "getpin";
    bool m_allowCache = false;
    QString m_keyInfo = "";
    bool m_remember = false;
    QString m_pwmngrLabel = "Save in password manager";
    QString m_okText = "";
    QString m_cancelText = "";
    QString m_notSetText = "";
    QString m_errorText = "";
    QString m_titleText = "";
    int m_timeout = 0;
    int m_remainingTime = 0;
    QString m_result = "";
    SecureString m_securePin;
    bool m_confirmResult = false;
    bool m_done = false;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    QTimer *m_timer = nullptr;

    void startTimer() {
        QMetaObject::invokeMethod(this, [this]() { doStartTimer(); }, Qt::QueuedConnection);
    }

    void stopTimer() {
        QMetaObject::invokeMethod(this, [this]() { stopTimerDirect(); }, Qt::QueuedConnection);
    }

    void doStartTimer() {
        stopTimerDirect();
        int t;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            t = m_timeout;
        }
        if (t > 0) {
            if (!m_timer) {
                m_timer = new QTimer(this);
                connect(m_timer, &QTimer::timeout, this, &PinentryBridge::onTimeoutTick);
            }
            m_timer->start(1000);
        }
    }

    void stopTimerDirect() {
        if (m_timer) m_timer->stop();
    }
};
