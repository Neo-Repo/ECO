#ifndef POWER_H
#define POWER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include <QThread>

class Power : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ getStatus WRITE setStatus NOTIFY statusChanged)

public:
    explicit Power(QString *user, QObject *parent = nullptr);
    Q_INVOKABLE void action(QString type);
    Q_INVOKABLE void startStatusTimer();
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
