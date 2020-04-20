#include "nouserinputtextedit.h"
#include <QPlainTextEdit>
#include <QDebug>

bool NoUserInputTextEdit::eventFilter(QObject* obj, QEvent* evt){
    if(evt->type() == QEvent::KeyPress || evt->type() == QEvent::KeyRelease){
        QKeyEvent* keyevt = static_cast<QKeyEvent*>(evt);
        // Ignore auto repeats
        if(keyevt->isAutoRepeat())
            return true;

        // modifiers(): Warning: This function cannot always be trusted. The user can confuse it by pressing both Shift keys simultaneously and releasing one of them, for example.
        emit macroKeyEvent(keyevt->key(), (evt->type() == QEvent::KeyPress), keyevt->modifiers());
        qDebug() << keyevt->key();
        return true;
    }
    return false;
}

NoUserInputTextEdit::NoUserInputTextEdit(QWidget* parent) : QPlainTextEdit(parent){
    installEventFilter(this);
}

