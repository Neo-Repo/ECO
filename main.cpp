#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "src/CursorPosProvider.h"
#include "src/service.h"
#include "src/startup.h"
#include "src/power.h"
#include "src/virt.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // get username
    QString username = qgetenv("USER");
    if (username.isEmpty())
        username = qgetenv("USERNAME");

    // libvirt
    virConnectPtr conn = nullptr;
    virDomainPtr domain = nullptr;
    Virt(&username, &conn, &domain);

    // CursorPosProvider
    CursorPosProvider mousePosProvider;
    engine.rootContext()->setContextProperty("mousePosition", &mousePosProvider);

    // Service
    Service service;
    engine.rootContext()->setContextProperty("serviceBackend", &service);

    // Startup
    Startup startup(&username);
    engine.rootContext()->setContextProperty("startupBackend", &startup);

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
