#pragma once

#include <QObject>
#include "XThreadGLWindow.h"

class XGLWindowTriangle : public XThreadGLWindow
{
	Q_OBJECT

private:
	using XQTBase = XThreadGLWindow;

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
