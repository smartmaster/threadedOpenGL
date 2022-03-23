#pragma once

#include <QObject>
#include <QWindow>
#include <QOpenGLFunctions_4_5_Core>
#include <QPaintDevice>
#include <QEvent>
#include <QExposeEvent>
#include <QResizeEvent>
#include <QOpenGLPaintDevice>
#include <QOpenGLContext>
#include <QSize>
#include <QMutex>
#include <QThread>

class XThreadedGLWindow;
class XThreadedGLWindowRenderer : public QObject
{
	Q_OBJECT

private:
	XThreadedGLWindow* _glwin{ nullptr };

public:
	XThreadedGLWindowRenderer(QObject* parent, XThreadedGLWindow* window);

public slots:
	void Render();
};

class XThreadedGLWindow : public QWindow, protected QOpenGLFunctions_4_5_Core
{
	Q_OBJECT

private:
	friend class XThreadedGLWindowRenderer;
	using XQTBase = QWindow;

private:
	QOpenGLPaintDevice* _paintDev{ nullptr };
	QOpenGLContext* _glctx{ nullptr };
	bool _animating{ false };
	int _eventCounter{ 0 };
	QMutex _renderLock;
	QThread* _thread{nullptr};
	XThreadedGLWindowRenderer* _renderer{ nullptr };

private:
	void Render();
	void NotifyRenderer();

signals:
	void RendererSignal();

private:
	virtual bool event(QEvent* ev) override;
	//virtual void exposeEvent(QExposeEvent* ev) override;
	virtual void paintEvent(QPaintEvent* ev) override;
	virtual void resizeEvent(QResizeEvent* ev) override;


protected:
	virtual void GLInitialize() = 0;
	virtual void GLResize(const QSize& size, const QSize& oldSize) = 0;
	virtual void GLPaint(QPaintDevice* paintDev) = 0;
	virtual void GLFinalize() = 0;

public:
	void FinalizeGL();
	void SetAnimating(bool run);

public:
	XThreadedGLWindow(QWindow* parent);
	~XThreadedGLWindow();
};
