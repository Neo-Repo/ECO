#include "spice-widget.h"
#include "spice-view.h"
#include <QObject>

class Spice : public QObject
{
    Q_OBJECT
public:
    Spice();
    static Spice *getSpice();
    void connectToGuest(const QString &path);
    quint32 getKeyboardLockModifiers();
    QVector<SpiceView *> displays;
private:
    static Spice *instance;
    SpiceSession *session;
};
