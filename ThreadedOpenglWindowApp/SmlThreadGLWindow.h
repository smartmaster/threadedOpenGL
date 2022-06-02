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
#include "SmlWaitObject.h"

class SmlThreadGLWindow;
class SmlThreadGLRender : public QObject
{
	Q_OBJECT

private:
	SmlThreadGLWindow* _glwin{ nullptr };

public:
	SmlThreadGLRender(QObject* parent, SmlThreadGLWindow* window);

public slots:
	void Render();

//signals:
//	void RenderFrameDoneSignal();

};

class SmlThreadGLWindow : public QWindow, protected QOpenGLFunctions_4_5_Core
{
	Q_OBJECT

private:
	friend class SmlThreadGLRender;
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
	SmlThreadGLRender* _render{ nullptr };

	//QMutex _timeoutLock;
	//QWaitCondition _condTimeout;
	SmlEvent _eventRenderDone{false};
	//XSemphore _event{ 1 };

	bool _requestMode{ false };
	volatile bool _ctxAcquired{ false };
	QWaitCondition _condRequest;

	bool _multiThreadMode{ true };
	

private:
	void ThreadRender();
	void Render();
	void RequestRender();
	void FinalizeGL();

	bool MakeCurrentCtx(const char* msg, const char* msg1);
	void DoneCurrentCtx();

	//bool WaitRenderTimeout(ulong mstime);

	void RequestCtx();

public slots:
	void ResponseCtx();

signals:
	void RequestCtxSignal();
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
	SmlThreadGLWindow(QWindow* parent, bool requestMode = false, bool multiThreadMode = true);
	virtual ~SmlThreadGLWindow();
};
