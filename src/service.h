#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>
#include <QDebug>
#include <QJsonDocument>

class Service : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ getConnected WRITE setConnected NOTIFY connectedChanged)

public:
    explicit Service(QObject *parent = nullptr);
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

#endif // SERVICE_H
