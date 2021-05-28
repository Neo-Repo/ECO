#include "service.h"

Service::Service(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);

    connect(server, &QTcpServer::newConnection, this, &Service::newConnection);

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

bool Service::getConnected() const
{
    return connected;
}

void Service::setConnected(const bool &connectionStatus)
{
    connected = connectionStatus;
    emit connectedChanged();
}

void Service::newConnection()
{
    socket = server->nextPendingConnection();
    qDebug() << "New Connection " + socket->peerAddress().toString();
    socket->write("{\"op\":0, \"interval\":1000}");
    connect(socket, &QTcpSocket::readyRead, this, &Service::read);
    setConnected(true);

    // we start the timer and expect the client set interval again to avoid timeout
    heartBeatTimer->start(2000);
}

void Service::read()
{
    QJsonDocument data = QJsonDocument::fromJson(socket->readAll());

    switch (data["op"].toInt()) {
        // code 0: heartbeat interval
        case 0:
            heartBeatTimer->setInterval(2000);
            break;
    }
}
