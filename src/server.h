#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QDebug>
#include <QJsonDocument>

class Server : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ getConnected WRITE setConnected NOTIFY connectedChanged)

public:
    explicit Server(QObject *parent = nullptr);
    bool getConnected() const;

public slots:
    void setConnected(const bool &connectionStatus);
    void newConnection();
    void read();

signals:
    void connectedChanged();

private:
    bool connected = false;
    QTcpServer *server;
    QTcpSocket *socket;
    QTimer *heartBeatTimer;
};

#endif // SERVER_H
