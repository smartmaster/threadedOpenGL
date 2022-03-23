#include "XGLWindowTriangle.h"
#include <QColor>
#include <QPainter>


void XGLWindowTriangle::GLInitialize()
{

}

void XGLWindowTriangle::GLResize(const QSize& size, const QSize& oldSize)
{
	glViewport(0, 0, size.width(), size.height());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	QColor bgcolor = Qt::darkCyan;
	glClearColor(bgcolor.redF(), bgcolor.greenF(), bgcolor.blueF(), 1.0f);
}

void XGLWindowTriangle::GLPaint(QPaintDevice* paintDev)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	QColor bgcolor = Qt::darkCyan;
	glClearColor(bgcolor.redF(), bgcolor.greenF(), bgcolor.blueF(), 1.0f);


	QPainter painter{ paintDev };

	painter.setPen(Qt::white);
	painter.setFont(QFont("Arial", 30));
	painter.drawText(50, 50,  QString::number(++_counter));
}

void XGLWindowTriangle::GLFinalize()
{

}

void XGLWindowTriangle::keyPressEvent(QKeyEvent* ev)
{
	int key = ev->key();
	switch (key)
	{
	case Qt::Key_Space:
		_isAnimating = !_isAnimating;
		SetAnimating(_isAnimating);
		break;

	default:
		XQTBase::keyPressEvent(ev);
		break;
	}
	
}

XGLWindowTriangle::XGLWindowTriangle(QWindow*parent)
	: XQTBase(parent)
{
}

XGLWindowTriangle::~XGLWindowTriangle()
{
	//FinalizeGL();
}
