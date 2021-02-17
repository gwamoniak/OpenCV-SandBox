#include "showvideocapture.h"
#include "keyboardfilter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ShowVideoCapture w;
    KeyboardFilter filter;
    w.installEventFilter(&filter);
    w.show();

    return a.exec();
}
