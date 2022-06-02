#include <QtGlobal>

#include "SmlThreadGLWindow.h"
#include <QMutexLocker>


SmlThreadGLRender::SmlThreadGLRender(QObject* parent, SmlThreadGLWindow* window) :
	QObject{ parent },
	_glwin{ window }
{
}

void SmlThreadGLRender::Render()
{

	_glwin->ThreadRender();

	//XMTLocker lockerRender{ &_glwin->_renderLock, _glwin->_multiThreadMode };

	//static int xxxdebug = 0;
	//if (xxxdebug)
	//{
	//	const int loop = 10;
	//	for (int ii = 0; ii < loop; ++ii)
	//	{
	//		QThread::msleep(1000);
	//	}
	//}


	//_glwin->Render();
	//_glwin->_glctx->moveToThread(_glwin->thread());
	//emit RenderFrameDoneSignal();

	//lockerRender.unlock();
	//

	////XMTLocker lockerTimeout{ &_glwin->_timeoutLock, _glwin->_multiThreadMode };
	////_glwin->_isRenerering = false;
	////lockerTimeout.unlock();
	////_glwin->_condTimeout.wakeOne();
	//_glwin->_event.Notify(false);


}


void SmlThreadGLWindow::RequestRender()
{
	//XMTLocker lockerTimeout{ &_timeoutLock, _multiThreadMode };

	//const ulong timeOut = 500;
	//bool waitOK = WaitRenderTimeout(timeOut);
	//if (!waitOK)
	//{
	//	return;
	//}

	if (!_multiThreadMode)
	{
		Render();
		emit RenderFrameDoneSignal();
		return;
	}


	const ulong timeOut = 500;
	bool waitOK = _eventRenderDone.Wait(timeOut);
	if (!waitOK)
	{
		return;
	}

	if (_requestMode)
	{
		emit RequestRenderSignal();
		return;
	}


	SmlMTLocker lockerRender{ &_renderLock, _multiThreadMode };
	//_isRenerering = true;

	//lockerTimeout.unlock();

	_glctx->moveToThread(_thread);

	lockerRender.unlock();

	emit RequestRenderSignal();

	//if (_animating)
	//{
	//	requestUpdate();
	//}
}

static void xxxDelay()
{
	static int xxxdebug = 0;
	if (xxxdebug)
	{
		static int secs = 10;
		for (int ii = 0; ii < secs; ++ii)
		{
			QThread::msleep(1000);
		}
	}
}

void SmlThreadGLWindow::ThreadRender()
{
	SmlMTLocker lockerRender{ &_renderLock, _multiThreadMode };

	if (_requestMode)
	{
		RequestCtx();
	}

	xxxDelay();

	Render();
	_glctx->moveToThread(this->thread());

	lockerRender.unlock();

	//XMTLocker lockerTimeout{ &_glwin->_timeoutLock, _glwin->_multiThreadMode };
	//_glwin->_isRenerering = false;
	//lockerTimeout.unlock();
	//_glwin->_condTimeout.wakeOne();
	_eventRenderDone.Notify(false);

	emit RenderFrameDoneSignal();
}

void SmlThreadGLWindow::Render()
{
	if (isExposed())
	{

		MakeCurrentCtx(__FUNCTION__, __FILE__);

		GLPaint(_paintDev);
		_glctx->swapBuffers(this);

		DoneCurrentCtx();
		//if (_animating)
		//{
		//	requestUpdate();
		//}

	}
}


void SmlThreadGLWindow::paintEvent(QPaintEvent* ev)
{
	RequestRender();
}

//void XThreadGLWindow::exposeEvent(QExposeEvent* ev)
//{
//	NotifyRender();
//}

static void APIENTRY GLDebugPoc(
	GLenum source, GLenum type,
	GLuint id, GLenum severity,
	GLsizei length, const GLchar* message,
	const void* userParam)
{
	QString str = QString::fromUtf16(u"severity:%3 message:[%5] source:%0 type:%1 id:%2 length:%4")
		.arg(source)
		.arg(type)
		.arg(id)
		.arg(severity)
		.arg(length)
		.arg(message);

	qDebug() << str;
}

void SmlThreadGLWindow::resizeEvent(QResizeEvent* ev)
{
	if (_SurfaceDestroyed)
	{
		return;
	}

	//XMTLocker lockerTimeout{ &_timeoutLock, _multiThreadMode };
	//const ulong timeOut = 500;
	//bool waitOK = WaitRenderTimeout(timeOut);
	//if (!waitOK)
	//{
	//	return;
	//}

	bool initGL = false;

	SmlMTLocker lockerRender{ &_renderLock, _multiThreadMode };

	//lockerTimeout.unlock();

	if (nullptr == _glctx)
	{
		initGL = true;
		//_glctx = new QOpenGLContext{ this };
		_glctx = new QOpenGLContext{ nullptr }; //cannot have parent for moving thread
		_glctx->setFormat(requestedFormat());
		_glctx->setShareContext(nullptr);
		_glctx->create();
	}

	MakeCurrentCtx(__FUNCTION__, __FILE__);

	if (initGL)
	{
		_paintDev = new QOpenGLPaintDevice{};

		initializeOpenGLFunctions();
		glDebugMessageCallback(GLDebugPoc, nullptr);


		GLInitialize();
	}

	qreal dpr = devicePixelRatio();
	_paintDev->setDevicePixelRatio(dpr);
	_paintDev->setSize(ev->size() * dpr);

	GLResize(ev->size(), ev->oldSize());

	DoneCurrentCtx();
}

//void XThreadGLWindow::GLInitialize()
//{
//
//}
//
//void XThreadGLWindow::GLResize(const QSize& size, const QSize& oldSize)
//{
//
//}
//
//void XThreadGLWindow::GLPaint(QPaintDevice* paintDev)
//{
//
//}
//
//void XThreadGLWindow::GLFinalize()
//{
//
//}

bool SmlThreadGLWindow::event(QEvent* ev)
{
	QEvent::Type et = ev->type();

#if 0
	++_eventCounter;
	qDebug() << _eventCounter << ev;

	switch (et)
	{
	case QEvent::Paint:
	case QEvent::Expose:
		qDebug() << _eventCounter << ev;
		break;
	}
#endif

	switch (et)
	{
	case QEvent::UpdateRequest:
		RequestRender();
		break;

	case QEvent::PlatformSurface:
	{
		auto* psev = (QPlatformSurfaceEvent*)(ev);
		if (QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed == psev->surfaceEventType())
		{
			_SurfaceDestroyed = true;
			FinalizeGL();
		}
	}
	break;

	}

	return XQTBase::event(ev);
}

void SmlThreadGLWindow::FinalizeGL()
{
	SmlMTLocker lockerRender{ &_renderLock, _multiThreadMode };

	if (_glctx)
	{
		MakeCurrentCtx(__FUNCTION__, __FILE__);

		GLFinalize();

		delete _paintDev;
		_paintDev = nullptr;

		//_glctx->deleteLater();
		//_glctx = nullptr;

		DoneCurrentCtx();
	}
}

bool SmlThreadGLWindow::MakeCurrentCtx(const char* msg, const char* msg1)
{
	bool ok = _glctx->makeCurrent(this);
	Q_ASSERT_X(ok && _glctx->isValid(), msg, msg1);
	return ok;
}

void SmlThreadGLWindow::DoneCurrentCtx()
{
	if (_multiThreadMode) //required for multi threaded mode rendering
	{
		_glctx->doneCurrent();
	}

}

void SmlThreadGLWindow::RequestCtx()
{
	//must be protected by QMutexLocker<QMutex> locker{ &_renderLock };
	emit RequestCtxSignal();
	//while (_glctx->thread() != _thread)
	while (!_ctxAcquired)
	{
		_condRequest.wait(&_renderLock);
	}
	_ctxAcquired = false;
}

void SmlThreadGLWindow::ResponseCtx()
{
	SmlMTLocker locker{ &_renderLock, _multiThreadMode };
	_glctx->moveToThread(_thread);
	_ctxAcquired = true;
	locker.unlock();
	_condRequest.notify_one();
}

//bool XThreadGLWindow::WaitRenderTimeout(ulong mstime)
//{
//	bool waitOK = true;
//	while (_isRenerering)
//	{
//		waitOK = _condTimeout.wait(&_timeoutLock, mstime);
//		if (!waitOK)
//		{
//			break;
//		}
//	}
//	return waitOK;
//}

//XThreadGLRender* XThreadGLWindow::ThreadGLRender()
//{
//	return _render;
//}

void SmlThreadGLWindow::SetAnimating(bool run)
{
	_animating = run;
	if (_animating)
	{
		//connect(_render, &XThreadGLRender::RenderFrameDoneSignal, this, &XThreadGLWindow::requestUpdate);
		connect(this, &SmlThreadGLWindow::RenderFrameDoneSignal, this, &SmlThreadGLWindow::requestUpdate);
		requestUpdate();
	}
	else
	{
		//disconnect(_render, &XThreadGLRender::RenderFrameDoneSignal, this, &XThreadGLWindow::requestUpdate);
		disconnect(this, &SmlThreadGLWindow::RenderFrameDoneSignal, this, &SmlThreadGLWindow::requestUpdate);
	}
}

SmlThreadGLWindow::SmlThreadGLWindow(QWindow* parent, bool requestMode /*= false*/, bool multiThreadMode /*= true*/)
	: QWindow(parent),
	_requestMode{ requestMode },
	_multiThreadMode{ multiThreadMode }
{
	setSurfaceType(QSurface::OpenGLSurface);

	if (_multiThreadMode)
	{
		_thread = new QThread{ this };
		_render = new SmlThreadGLRender{ nullptr, this };
		_render->moveToThread(_thread);

		connect(_thread, &QThread::finished, _render, &QObject::deleteLater);
		connect(this, &SmlThreadGLWindow::RequestRenderSignal, _render, &SmlThreadGLRender::Render);

		if (_requestMode)
		{
			connect(this, &SmlThreadGLWindow::RequestCtxSignal, this, &SmlThreadGLWindow::ResponseCtx);
		}

		_thread->start();
	}
}

SmlThreadGLWindow::~SmlThreadGLWindow()
{
	if (_multiThreadMode)
	{
		_thread->quit();
		_thread->wait();
	}

	if (_glctx)
	{
		_glctx->deleteLater();
		_glctx = nullptr;
	}
	//FinalizeGL();

}
