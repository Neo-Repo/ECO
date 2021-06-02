#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);

    connect(server, &QTcpServer::newConnection, this, &Server::newConnection);

//    if(!server->listen(QHostAddress::Any, 40110))
//        qDebug() << "Server could not start";
//    else
//        qDebug() << "Server started!";

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

void Server::setConnected(const bool &connectionStatus)
{
    connected = connectionStatus;
    emit connectedChanged();
}

void Server::newConnection()
{
    socket = server->nextPendingConnection();
    qDebug() << "New Connection " + socket->peerAddress().toString();
    socket->write("{\"op\":0, \"interval\":1000}");
    connect(socket, &QTcpSocket::readyRead, this, &Server::read);
    setConnected(true);

    // we start the timer and expect the client set interval again to avoid timeout
    heartBeatTimer->start(2000);
}

void Server::read()
{
    QJsonDocument data = QJsonDocument::fromJson(socket->readAll());

    switch (data["op"].toInt()) {
        // code 0: heartbeat interval
        case 0:
            heartBeatTimer->setInterval(2000);
            break;
    }
}
