#include "SmlThreadedOpenglMainWindow.h"
#include "SmlGLWindowTriangle.h"
#include <QHBoxLayout>

SmlThreadedOpenglMainWindow::SmlThreadedOpenglMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ///////////////////////////////////////////////////
    _glwin = new SmlGLWindowTriangle(nullptr);
    _glwin->setFlags(Qt::FramelessWindowHint);
    _glwin->create();
    _cont = QWidget::createWindowContainer(_glwin, this, Qt::FramelessWindowHint);

    ui.verticalLayoutOpenGl->addWidget(_cont);

    ///////////////////////////////////////////////////
    connect(ui.pushButtonTestOtho, &QPushButton::clicked, this, &SmlThreadedOpenglMainWindow::OnPushButtonTestOthoClicked);


}

#include "SmlAxisCoord.test.h"

void SmlThreadedOpenglMainWindow::OnPushButtonTestOthoClicked()
{
    SmartLib::AxisCoordTest::Case0_projection();
}
