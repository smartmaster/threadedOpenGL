#pragma once

#include <QObject>
#include "XThreadedGlWindow.h"

class XGLWindowTriangle : public XThreadedGLWindow
{
	Q_OBJECT

private:
	using XQTBase = XThreadedGLWindow;

private:
	bool _isAnimating{ false };
	int _counter{ 0 };

private:
	virtual void GLInitialize() override;
	virtual void GLResize(const QSize& size, const QSize& oldSize) override;
	virtual void GLPaint(QPaintDevice* paintDev) override;
	virtual void GLFinalize() override;

private:
	virtual void keyPressEvent(QKeyEvent* ev) override;

public:
	XGLWindowTriangle(QWindow *parent);
	~XGLWindowTriangle();
};
