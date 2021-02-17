#include "keyboardfilter.h"
#include <QDebug>


bool KeyboardFilter::eventFilter(QObject *dist, QEvent *event ) // definition
{

        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            qDebug("At key press %d", keyEvent->key());

                if(keyEvent->key() == Qt::Key_Escape)
                {

                     QApplication::quit();
                    return true;
                }
                return QObject::eventFilter(dist,event);
        }
        else
        {
            return false;
        }

}
