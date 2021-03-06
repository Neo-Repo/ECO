#include "src/spice/spice.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "src/CursorPosProvider.h"
#include "src/settings.h"
#include "src/server.h"
#include "src/startup.h"
#include "src/power.h"
#include "src/virt.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // get username
    QString username = qgetenv("USER");
    if (username.isEmpty())
        username = qgetenv("USERNAME");
    engine.rootContext()->setContextProperty("username", username);

    // libvirt
    virConnectPtr conn = nullptr;
    virDomainPtr domain = nullptr;
    Virt(&username, &conn, &domain);

    // CursorPosProvider
    CursorPosProvider mousePosProvider;
    engine.rootContext()->setContextProperty("mousePosition", &mousePosProvider);

    // Server
    Server server(&username);
    engine.rootContext()->setContextProperty("serverBackend", &server);

    // Spice
    Spice spice(QString("/home/%1/.config/Neo/ECO/display.sock").arg(username));
    engine.rootContext()->setContextProperty("spiceBackend", &spice);

    // Startup
    Startup startup(&username);
    engine.rootContext()->setContextProperty("startupBackend", &startup);

    // Settings
    Settings settings(&username, &conn, &domain);
    engine.rootContext()->setContextProperty("settingsBackend", &settings);

    // Power
    Power power(&username, conn, domain);
    engine.rootContext()->setContextProperty("powerBackend", &power);

    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
