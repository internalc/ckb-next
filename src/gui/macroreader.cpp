#include <QElapsedTimer>
#include <QDebug>
#include "macroreader.h"
#include <QThread>

qint64 MacroReader::keyStrokeTime(){
    qint64 elapsed;
    if(timer){
        elapsed = timer->nsecsElapsed() / 1000;
    } else {
        elapsed = -1;
        timer = new QElapsedTimer;
    }
    timer->start();
    return elapsed;
}

void MacroReader::keystrokeReceived(QFile* f){
    QByteArray data = f->readLine().mid(4);
    // Remove the "key " prefix
    // Remove the newline at the end
    data.chop(1);
    QString line = QString::fromUtf8(data);
    qint64 elapsed = keyStrokeTime();
    emit macroLineRead(line, elapsed, (line.at(0) == QChar('+')), (line.length() == 2));
}

MacroReader::MacroReader(QStringList& macroPaths) : _macroPaths(macroPaths), timer(nullptr) {

    qDebug() << "Macroreader paths" << _macroPaths;
    // Wait a small amount of time for the nodes to open (100ms)
    // We shouldn't block the main event loop for too long
    QThread::msleep(100);

    int mlength = _macroPaths.length();
   /* QFile* ptr = new QFile(macroPaths.at(0));
    ptr->open(QIODevice::ReadOnly);
    sock = new QLocalSocket();
    sock->setSocketDescriptor(ptr->handle());
    connect(sock, &QLocalSocket::readyRead, this, [=] () { qDebug() << sock->readAll(); });*/
    //sock->connectToServer(QIODevice::ReadOnly);
    //qDebug() << sock->errorString();

    for(int i = 0; i < mlength; i++){
        QFile* ptr = new QFile(macroPaths.at(i));
        fhandles.append(ptr);
        if(!ptr->open(QIODevice::ReadOnly))
            qDebug() << "Unable to open" << macroPaths.at(i);
        QSocketNotifier* nptr = new QSocketNotifier(ptr->handle(), QSocketNotifier::Read);
        fnotifiers.append(nptr);
        // QSocketNotifier calls activated() repeatedly until there is no more data left
        connect(nptr, &QSocketNotifier::activated, this, [this, ptr] (int handle) { keystrokeReceived(ptr); });
    }
}

MacroReader::~MacroReader() {
    qDebug() << "Destructor";
    for(int i = 0; i < fhandles.length(); i++){
        delete fnotifiers[i];
        delete fhandles[i];
    }
    if(timer)
        delete timer;
}

#define SK(keycode, string) case keycode: \
                                return string

#define SKN(keycode, string, modifier, string2) case keycode: \
                                return ((modifiers & modifier) ? string2 : string)

static inline const char* translateQtKeyCode(int keycode, Qt::KeyboardModifiers modifiers){
    switch(keycode){

    // First row
    SK(Qt::Key_Escape, "esc");
    SK(Qt::Key_F1, "f1");
    SK(Qt::Key_F2, "f2");
    SK(Qt::Key_F3, "f3");
    SK(Qt::Key_F4, "f4");
    SK(Qt::Key_F5, "f5");
    SK(Qt::Key_F6, "f6");
    SK(Qt::Key_F7, "f7");
    SK(Qt::Key_F8, "f8");
    SK(Qt::Key_F9, "f9");
    SK(Qt::Key_F10, "f10");
    SK(Qt::Key_F11, "f11");
    SK(Qt::Key_F12, "f12");
    SK(Qt::Key_SysReq, "prtscn");
    SK(Qt::Key_Print, "prtscn");
    SK(Qt::Key_ScrollLock, "scroll");
    SK(Qt::Key_Pause, "pause");
    // Add media keys?

    // Second row
    // FIXME tilde
    SKN(Qt::Key_1, "1", Qt::KeypadModifier, "num1");
    SKN(Qt::Key_2, "2", Qt::KeypadModifier, "num2");
    SKN(Qt::Key_3, "3", Qt::KeypadModifier, "num3");
    SKN(Qt::Key_4, "4", Qt::KeypadModifier, "num4");
    SKN(Qt::Key_5, "5", Qt::KeypadModifier, "num5");
    SK(Qt::Key_Clear, "num5");
    SKN(Qt::Key_6, "6", Qt::KeypadModifier, "num6");
    SKN(Qt::Key_7, "7", Qt::KeypadModifier, "num7");
    SKN(Qt::Key_8, "8", Qt::KeypadModifier, "num8");
    SKN(Qt::Key_9, "9", Qt::KeypadModifier, "num9");
    SKN(Qt::Key_0, "0", Qt::KeypadModifier, "num0");
    SKN(Qt::Key_Minus, "minus", Qt::KeypadModifier, "numminus");
    SKN(Qt::Key_Plus, "equals", Qt::KeypadModifier, "numplus");
    SK(Qt::Key_Equal, "equals");
    SK(Qt::Key_Backspace, "bspace");

    SKN(Qt::Key_Insert, "ins", Qt::KeypadModifier, "numdot");
    SKN(Qt::Key_Home, "home", Qt::KeypadModifier, "num7");
    SKN(Qt::Key_PageUp, "pgup", Qt::KeypadModifier, "num9");

    SK(Qt::Key_NumLock, "numlock");

    // Third row
    SK(Qt::Key_Tab, "tab");
    SK(Qt::Key_Q, "q");
    SK(Qt::Key_W, "w");
    SK(Qt::Key_E, "e");
    SK(Qt::Key_R, "r");
    SK(Qt::Key_T, "t");
    SK(Qt::Key_Y, "y");
    SK(Qt::Key_U, "u");
    SK(Qt::Key_I, "i");
    SK(Qt::Key_O, "o");
    SK(Qt::Key_P, "p");
    // FIXME: Add braces and slash
    SKN(Qt::Key_Delete, "del", Qt::KeypadModifier, "numdot");
    SKN(Qt::Key_End, "end", Qt::KeypadModifier, "num1");
    SKN(Qt::Key_PageDown, "pgdn", Qt::KeypadModifier, "num3");

    // Fourth row
    SK(Qt::Key_CapsLock, "caps");
    SK(Qt::Key_A, "a");
    SK(Qt::Key_S, "s");
    SK(Qt::Key_D, "d");
    SK(Qt::Key_F, "f");
    SK(Qt::Key_G, "g");
    SK(Qt::Key_H, "h");
    SK(Qt::Key_J, "j");
    SK(Qt::Key_K, "k");
    SK(Qt::Key_L, "l");
    // FIXME more stuff
    // Numpad enter shows up as Key_Enter
    SK(Qt::Key_Return, "enter");

    // Fifth row
    // We can't differentiate between left and right modifiers...
    SK(Qt::Key_Shift, "lshift");
    SK(Qt::Key_Z, "z");
    SK(Qt::Key_X, "x");
    SK(Qt::Key_C, "c");
    SK(Qt::Key_V, "v");
    SK(Qt::Key_B, "b");
    SK(Qt::Key_N, "n");
    SK(Qt::Key_M, "m");
    SKN(Qt::Key_Up, "up", Qt::KeypadModifier, "num8");
    SK(Qt::Key_Enter, "numenter");

    // Sixth row
    SK(Qt::Key_Control, "lctrl");
    SK(Qt::Key_Super_L, "lwin");
    SK(Qt::Key_Alt, "lalt");
    SK(Qt::Key_Space, "space");
    SK(Qt::Key_Super_R, "rwin");
    SK(Qt::Key_Menu, "rmenu");
    SKN(Qt::Key_Left, "left", Qt::KeypadModifier, "num4");
    SKN(Qt::Key_Down, "down", Qt::KeypadModifier, "num2");
    SKN(Qt::Key_Right, "right", Qt::KeypadModifier, "num6");

    default:
        return NULL;
    }
}

void MacroReader::translateQKeyEvent(int keycode, bool down, Qt::KeyboardModifiers modifiers){
    const char* keystr = translateQtKeyCode(keycode, modifiers);
    if(!keystr)
        return;
    QString line = QString::fromUtf8(keystr);
    line.prepend(QChar(down ? '+' : '-'));
    qint64 elapsed = keyStrokeTime();
    emit macroLineRead(line, elapsed, down, (line.length() == 2));
}
