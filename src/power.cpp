#include "power.h"

Power::Power(QString *user, QObject *parent) : QObject(parent)
{
    username = *user;
}

void Power::action(QString type)
{
    if (type == "create")
        QProcess::startDetached(QString("virsh create /home/%1/.config/Neo/ECO/WindowsECO.xml").arg(username));
    else
        QProcess::startDetached(QString("virsh %1 WindowsECO").arg(type));
}

void Power::startStatusTimer()
{
    QThread *statusThread = new QThread();
    moveToThread(statusThread);

    connect(statusThread, &QThread::started, [this]() {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]() {
            QProcess virshStatus;
            virshStatus.start(QString("sh -c \"virsh dominfo WindowsECO | grep -w \"State:\" | awk '{ print $2}'\""));
            virshStatus.waitForFinished();
            QString stdout = virshStatus.readAllStandardOutput();

            if (stdout.contains("shut") && status != "off")
                setStatus("off");
            else if (stdout.contains("running") && status != "running")
                setStatus("running");
            else if (stdout.contains("paused") && status != "paused")
                setStatus("paused");
            else if (stdout.isEmpty() && !status.isEmpty())
                setStatus("");
        });
        timer->start(1000);
    });

    statusThread->start();
}

QString Power::getStatus() const
{
    return status;
}

void Power::setStatus(const QString &newStatus)
{
    status = newStatus;
    emit statusChanged();
}

