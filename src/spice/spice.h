#include "spice-widget.h"
#include "spice-view.h"

class Spice
{
public:
    static Spice *getSpice();
    void connectToGuest(const QString &host, const QString &port);
    quint32 getKeyboardLockModifiers();
    QVector<SpiceView *> displays;
private:
    Spice();
    static Spice *instance;
    SpiceSession *session;
};
