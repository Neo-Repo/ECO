#ifndef POWER_H
#define POWER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QThread>
#include <libvirt/libvirt.h>

class Power : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ getStatus WRITE setStatus NOTIFY statusChanged)

public:
    explicit Power(QString *user, virConnectPtr conn, virDomainPtr domain, QObject *parent = nullptr);
    static void newPowerStatus(virConnectPtr conn, virDomainPtr domain, int event, int detail, void *opaque);
    Q_INVOKABLE void action(QString type);
    QString getStatus() const;

public slots:
    void setStatus(const QString &newStatus);

signals:
    void statusChanged();

private:
    QString username;
    QString status;
};

#endif // POWER_H
