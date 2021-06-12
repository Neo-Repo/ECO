#include "spice-widget.h"
#include "spice-view.h"
#include <QObject>

class Spice : public QObject
{
    Q_OBJECT
public:
    Spice(const QString path);
    static Spice *getSpice();
    Q_INVOKABLE void toggleDisplay(int id);
    void connectToGuest(const QString &path);
    quint32 getKeyboardLockModifiers();
    QVector<SpiceView *> displays;
    QString path;
private:
    static Spice *instance;
    SpiceSession *session;
};
