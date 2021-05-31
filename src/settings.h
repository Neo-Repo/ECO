#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include <QDomNode>
#include <QDomDocument>
#include <QProcess>
#include <libvirt/libvirt.h>

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QString *user, virConnectPtr *_conn, virDomainPtr *_domain, QObject *parent = nullptr);
    Q_INVOKABLE int getRAM();
    Q_INVOKABLE void setRAM(QString);
    Q_INVOKABLE int getCPU();
    Q_INVOKABLE void setCPU(QString);
    Q_INVOKABLE void openXML();
    QString getXML();

private:
    QString username;
    virConnectPtr conn;
    virDomainPtr domain;

signals:

};

#endif // SETTINGS_H
