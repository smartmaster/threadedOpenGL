#include "SmlThreadedOpenglMainWindow.h"
#include <QtWidgets/QApplication>
#include "SmlSurfaceFormat.h"

int main(int argc, char *argv[])
{
    SmlSurfaceFormatUtils::SurfaceFormat();

    QApplication a(argc, argv);
    SmlThreadedOpenglMainWindow w;
    w.show();
    return a.exec();
}
