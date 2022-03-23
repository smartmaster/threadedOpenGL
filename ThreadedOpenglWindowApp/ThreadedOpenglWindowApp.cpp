#include "ThreadedOpenglWindowApp.h"
#include "XGLWindowTriangle.h"
#include <QHBoxLayout>

ThreadedOpenglWindowApp::ThreadedOpenglWindowApp(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    
    _glwin = new XGLWindowTriangle(nullptr);
    _glwin->setFlags(Qt::FramelessWindowHint);
    _glwin->create();
    _cont = QWidget::createWindowContainer(_glwin, this, Qt::FramelessWindowHint);

    ui.centralWidget->setLayout(new QHBoxLayout(this));
    ui.centralWidget->layout()->addWidget(_cont);

}
