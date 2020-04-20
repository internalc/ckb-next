#ifndef NOUSERINPUTLINEEDIT_H
#define NOUSERINPUTLINEEDIT_H

#include <QObject>
#include <QPlainTextEdit>

class NoUserInputTextEdit : public QPlainTextEdit {
    Q_OBJECT
public:
    NoUserInputTextEdit(QWidget* parent);
signals:
    void macroKeyEvent(int keycode, bool keydown, Qt::KeyboardModifiers modifiers);
private:
    bool eventFilter(QObject* obj, QEvent* evt);
};

#endif // NOUSERINPUTLINEEDIT_H
