#include "spice.h"

static void main_channel_event(SpiceChannel *channel, SpiceChannelEvent event, gpointer data)
{
    switch (event) {
        case SPICE_CHANNEL_OPENED:
            qDebug() << "main channel: connected";
            break;
        case SPICE_CHANNEL_CLOSED:
            qDebug() << "main channel: connection lost";
            Spice::getSpice()->Retry();
            break;
        case SPICE_CHANNEL_ERROR_CONNECT:
            qDebug() << "main channel: failed to connect";
            break;
    }
}

static void channel_new(SpiceSession *session, SpiceChannel *channel)
{
    int id;
    g_object_get(channel, "channel-id", &id, NULL);

    if (SPICE_IS_MAIN_CHANNEL(channel))
        g_signal_connect(channel, "channel-event", G_CALLBACK(main_channel_event), 0);

    if (SPICE_IS_DISPLAY_CHANNEL(channel)) {
        qDebug() << QString("display %1 created").arg(id+1);
        SpiceView *view = new SpiceView();
        view->id = id;
        Spice::getSpice()->displays.append(view);

        // The value 2 should not be hard coded
        if (Spice::getSpice()->displays.size() == 2) {
            foreach (SpiceView *view, Spice::getSpice()->displays) {
                Spice::getSpice()->displays[view->id]->display = spice_display_new(session, view->id);
                if (id == 0)
                    Spice::getSpice()->displays[view->id]->setWindowTitle("Main Display");
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

void Spice::toggleDisplay(int id)
{
    if (displays.length() == 0) return;

    --id;
    if (Spice::getSpice()->displays[id]->isVisible())
        Spice::getSpice()->displays[id]->hide();
    else
        Spice::getSpice()->displays[id]->show();
}

void Spice::connectToGuest(const QString &_path)
{
    path = _path;
    session = spice_session_new();
    g_object_set(session, "unix-path", \
                 path.toLatin1().constData(), NULL);
    g_signal_connect(session, "channel-new",
                                 G_CALLBACK(channel_new), 0);
    spice_session_connect(session);
}

void Spice::Retry()
{
    foreach (SpiceView *v, displays) {
        v->hide();
    }
    displays[0]->inputs = NULL;
    displays = {};
    connectToGuest(path);
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
