#include <QtTest>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QDir>
#include <QFile>
#include "pinentry_bridge.h"

class TestQmlRendering : public QObject {
    Q_OBJECT

private:
    QString findQmlFile() const {
        // Search upward from SOURCE_DIR for Main.qml
        QStringList candidates = {
            QDir(QStringLiteral(SOURCE_DIR)).absoluteFilePath(QStringLiteral("../Main.qml")),
            QDir(QStringLiteral(SOURCE_DIR)).absoluteFilePath(QStringLiteral("../../Main.qml")),
        };
        for (const auto &p : candidates) {
            if (QFile::exists(p)) return p;
        }
        return candidates.first();
    }

private slots:
    void testMainQmlParses() {
        QQmlEngine engine;
        PinentryBridge bridge;
        engine.rootContext()->setContextProperty("bridge", &bridge);

        QString qmlPath = findQmlFile();
        QVERIFY2(QFile::exists(qmlPath), qPrintable("QML file not found at: " + qmlPath));

        QQmlComponent component(&engine, QUrl::fromLocalFile(qmlPath));
        QVERIFY2(component.status() != QQmlComponent::Error,
                 qPrintable(component.errorString()));
    }

    void testGetpinModeCreates() {
        QQmlEngine engine;
        PinentryBridge bridge;
        engine.rootContext()->setContextProperty("bridge", &bridge);

        QQmlComponent component(&engine, QUrl::fromLocalFile(findQmlFile()));
        QVERIFY(component.status() == QQmlComponent::Ready);

        QScopedPointer<QObject> obj(component.create());
        QVERIFY(!obj.isNull());
    }

    void testConfirmModeCreates() {
        QQmlEngine engine;
        PinentryBridge bridge;
        bridge.setMode(PinentryBridge::Mode::Confirm);
        engine.rootContext()->setContextProperty("bridge", &bridge);

        QQmlComponent component(&engine, QUrl::fromLocalFile(findQmlFile()));
        QVERIFY(component.status() == QQmlComponent::Ready);

        QScopedPointer<QObject> obj(component.create());
        QVERIFY(!obj.isNull());
    }

    void testMessageModeCreates() {
        QQmlEngine engine;
        PinentryBridge bridge;
        bridge.setMode(PinentryBridge::Mode::Message);
        engine.rootContext()->setContextProperty("bridge", &bridge);

        QQmlComponent component(&engine, QUrl::fromLocalFile(findQmlFile()));
        QVERIFY(component.status() == QQmlComponent::Ready);

        QScopedPointer<QObject> obj(component.create());
        QVERIFY(!obj.isNull());
    }
};

QTEST_MAIN(TestQmlRendering)
#include "tst_main_qml.moc"
