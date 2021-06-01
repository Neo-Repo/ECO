#ifndef SETTINGS_H
#define SETTINGS_H
#define SUBSYSTEM "usb"

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include <QDomNode>
#include <QDomDocument>
#include <QProcess>
#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include <libudev.h>
#include <libvirt/libvirt.h>

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QJsonArray usbDevices READ getUsbDevices WRITE setUsbDevices NOTIFY usbDevicesChanged)

public:
    explicit Settings(QString *user, virConnectPtr *_conn, virDomainPtr *_domain, QObject *parent = nullptr);
    Q_INVOKABLE int getRAM();
    Q_INVOKABLE void setRAM(QString);
    Q_INVOKABLE int getCPU();
    Q_INVOKABLE void setCPU(QString);
    Q_INVOKABLE void openXML();
    Q_INVOKABLE void setDevice(QString vendor, QString product);
    QString getXML();
    QJsonArray getUsbDevices() const;
    QString make_device(QString vendor, QString product);
    bool check_connection(QString vendor, QString product);

    // udev
    void monitor_devices(struct udev* udev);
    void enumerate_devices(struct udev* udev);
    void check_device(struct udev_device* dev);
    void add_device(struct udev_device* dev);

public slots:
    void setUsbDevices(const QJsonArray &newUsbDevices);

signals:
    void usbDevicesChanged();

private:
    QString username;
    virConnectPtr conn;
    virDomainPtr domain;
    QJsonArray usbDevices;
};

#endif // SETTINGS_H
