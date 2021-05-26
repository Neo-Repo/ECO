#ifndef STARTUP_H
#define STARTUP_H

#include <QObject>
#include <QJsonObject>
#include <QJSValue>

class Startup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList result READ getResult WRITE setResult NOTIFY resultChanged)

public:
    explicit Startup(QString *user, QObject *parent = nullptr);
    Q_INVOKABLE QStringList required();
    Q_INVOKABLE QJsonObject spec();
    Q_INVOKABLE void startInstall();
    Q_INVOKABLE void startNewMachine(QJSValue rawInput);
    void install();
    void newMachine();
    QStringList getResult() const;

public slots:
    void setResult(const QStringList &newResult);

signals:
    void resultChanged();
    void created();

private:
    QString username;
    QStringList result;
    QJsonObject input;
};

#endif // STARTUP_H
