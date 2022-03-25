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
#include <QWaitCondition>
#include "XWaitObject.h"

class XThreadGLWindow;
class XThreadGLRender : public QObject
{
	Q_OBJECT

private:
	XThreadGLWindow* _glwin{ nullptr };

public:
	XThreadGLRender(QObject* parent, XThreadGLWindow* window);

public slots:
	void Render();

//signals:
//	void RenderFrameDoneSignal();

};

class XThreadGLWindow : public QWindow, protected QOpenGLFunctions_4_5_Core
{
	Q_OBJECT

private:
	friend class XThreadGLRender;
	using XQTBase = QWindow;

private:
	QOpenGLPaintDevice* _paintDev{ nullptr };
	QOpenGLContext* _glctx{ nullptr };

	int _eventCounter{ 0 };
	bool _animating{ false };
	//bool _isRenerering{ false };
	bool _SurfaceDestroyed{ false };

	
	
	QMutex _renderLock;
	QThread* _thread{nullptr};
	XThreadGLRender* _render{ nullptr };

	//QMutex _timeoutLock;
	//QWaitCondition _condTimeout;
	XEvent _event{false};
	//XSemphore _event{ 1 };
	

private:
	void ThreadRender();
	void Render();
	void RequestRender();
	void FinalizeGL();

	bool MakeCurrentCtx(const char* msg, const char* msg1);
	void DoneCurrentCtx();

	//bool WaitRenderTimeout(ulong mstime);

signals:
	void RequestRenderSignal();
	void RenderFrameDoneSignal();

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

//protected:
//	XThreadGLRender* ThreadGLRender();

public:
	void SetAnimating(bool run);

public:
	XThreadGLWindow(QWindow* parent);
	virtual ~XThreadGLWindow();
};
