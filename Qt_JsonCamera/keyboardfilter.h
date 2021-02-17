#ifndef KEYBOARDFILTER_H
#define KEYBOARDFILTER_H

#include <QObject>
#include <QApplication>
#include <QKeyEvent>

class KeyboardFilter : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardFilter(QObject *parent = 0) : QObject( parent ) {}

protected:
  bool eventFilter( QObject *dist, QEvent *event );

};

#endif // KEYBOARDFILTER_H
