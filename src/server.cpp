#include "server.h"

Server::Server(QString *user, QObject *parent) : QObject(parent)
{
    username = user;
    server = new QTcpServer(this);

    connect(server, &QTcpServer::newConnection, this, &Server::newConnection);

    if(!server->listen(QHostAddress::Any, 40110))
        qDebug() << "Service server could not start";
    else
        qDebug() << "Service server started!";

    // HeartBeat timer
    heartBeatTimer = new QTimer(this);
    connect(heartBeatTimer, &QTimer::timeout, this, [&]() {
        setConnected(false);
    });
}

bool Server::getConnected() const
{
    return connected;
}

void Server::setPrograms(QJsonDocument *doc)
{
    QJsonObject object = doc->object();
    QJsonObject::iterator i;
    for (i = object.begin(); i != object.end(); ++i) {
        if (i.key() == "data") {
            QJsonObject object = i->toObject();
            QJsonObject::iterator i;
            for (i = object.begin(); i != object.end(); ++i) {
                if (i.key() == "programs") {
                    QJsonObject object = i->toObject();
                    makeDesktop(&object, "program");
                }
                else if (i.key() == "storeApps") {
                    QJsonObject object = i->toObject();
                    makeDesktop(&object, "storeApp");
                }
            }
        }
    }
}

void Server::makeDesktop(QJsonObject *obj, QString type)
{
    QString xdgPath = "/home/"+*username+"/.local/share/Neo/ECO/";;
    QString name = (*obj)["name"].toString();
    QFile desktop(xdgPath+"applications/"+name+".desktop");
    desktop.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&desktop);
    stream << "[Desktop Entry]\n";
    stream << "Type=Application\n";
    stream << "Name="+name+"\n";
    stream << "Icon="+name+"\n";

    if (type == "program")
        stream << QString("Exec=ecopen p \"%1\"\n").arg((*obj)["path"].toString());

    else if (type == "storeApp")
        stream << QString("Exec=ecopen s \"%1\" \"%2\"\n").arg((*obj)["AppId"].toString()).arg((*obj)["PFN"].toString());

    desktop.close();

    // make the icon file
    QByteArray iconData = QByteArray::fromBase64((*obj)["icon"].toString().toUtf8());
    QImage icon = QImage::fromData(iconData);

    if(!icon.isNull())
        icon.save(xdgPath+"icons/"+name+".png", "PNG");
}

void Server::setConnected(const bool &connectionStatus)
{
    connected = connectionStatus;
    emit connectedChanged();
}

void Server::newConnection()
{
    req = false;
    socket = server->nextPendingConnection();
    //qDebug() << "New Connection " + socket->peerAddress().toString();
    socket->write("{\"op\":0, \"interval\":1000}");
    connect(socket, &QTcpSocket::readyRead, this, &Server::read);
    setConnected(true);
    mClients.push_back(socket);
    // we start the timer and expect the client set interval again to avoid timeout
    heartBeatTimer->start(2000);
}

void Server::read()
{
    rawData += socket->readAll();
    QJsonParseError parseError;
    QJsonDocument data = QJsonDocument::fromJson(rawData, &parseError);

    if (parseError.error == QJsonParseError::NoError) {
        switch (data["op"].toInt()) {
            // code 0: heartbeat interval
            case 0:
                heartBeatTimer->setInterval(2000);
                if (!req) {
                    req = true;
                    socket->write("{\"op\":1}");
                }
                break;

            // code 1: store apps and programs
            case 1:
                setPrograms(&data);
                break;

            // code 2: open
            case 2:
                for(auto &client : mClients) {
                    client->write(rawData);
                }
                break;
        }
        rawData = "";
    }
}
