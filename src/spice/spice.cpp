#include "spice.h"

static void channel_new(SpiceSession *session, SpiceChannel *channel)
{
    int id;
    g_object_get(channel, "channel-id", &id, NULL);
    if (SPICE_IS_DISPLAY_CHANNEL(channel)) {
        qDebug() << QString("display %1 created").arg(id+1);
        SpiceView *view = new SpiceView();
        view->id = id;
        Spice::getSpice()->displays.append(view);

        // The value 2 should not be hard coded
        if (Spice::getSpice()->displays.size() == 2) {
            foreach (SpiceView *view, Spice::getSpice()->displays) {
                Spice::getSpice()->displays[view->id]->display = spice_display_new(session, view->id);
                Spice::getSpice()->displays[view->id]->setWindowTitle(QString("Display %1").arg(view->id+1));
                Spice::getSpice()->displays[view->id]->show();
            }
        }
    }
}

Spice *Spice::instance = NULL;

Spice::Spice(const QString path)
{
    session = NULL;
    instance = this;
    connectToGuest(path);
}

Spice *Spice::getSpice()
{
    if (instance)
        return instance;
    instance = new Spice("");
    return instance;
}

void Spice::connectToGuest(const QString &path)
{
    session = spice_session_new();
    g_object_set(session, "unix-path", \
                 path.toLatin1().constData(), NULL);
    g_signal_connect(session, "channel-new",
                                 G_CALLBACK(channel_new), 0);
    spice_session_connect(session);
}

quint32 Spice::getKeyboardLockModifiers()
{
    XKeyboardState keyboard_state;
    Display *x_display = QX11Info::display();
    quint32 modifiers = 0;

    XGetKeyboardControl(x_display, &keyboard_state);

    if (keyboard_state.led_mask & 0x01)
        modifiers |= SPICE_INPUTS_CAPS_LOCK;
    if (keyboard_state.led_mask & 0x02)
        modifiers |= SPICE_INPUTS_NUM_LOCK;
    if (keyboard_state.led_mask & 0x04)
        modifiers |= SPICE_INPUTS_SCROLL_LOCK;

    return modifiers;
}
