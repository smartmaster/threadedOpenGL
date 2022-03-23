#include "ThreadedOpenglWindowApp.h"
#include <QtWidgets/QApplication>
#include "XSurfaceFormat.h"

int main(int argc, char *argv[])
{
    XSurfaceFormatUtils::SurfaceFormat();

    QApplication a(argc, argv);
    ThreadedOpenglWindowApp w;
    w.show();
    return a.exec();
}
