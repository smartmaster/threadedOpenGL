#include <QtGlobal>

#include "XThreadGLWindow.h"
#include <QMutexLocker>


XThreadGLRender::XThreadGLRender(QObject* parent, XThreadGLWindow* window) :
	QObject{ parent },
	_glwin{ window }
{
}

void XThreadGLRender::Render()
{

	_glwin->ThreadRender();

	//QMutexLocker lockerRender{ &_glwin->_renderLock };

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

	////QMutexLocker lockerTimeout{ &_glwin->_timeoutLock };
	////_glwin->_isRenerering = false;
	////lockerTimeout.unlock();
	////_glwin->_condTimeout.wakeOne();
	//_glwin->_event.Notify(false);

	
}


void XThreadGLWindow::RequestRender()
{
	//QMutexLocker lockerTimeout{ &_timeoutLock };

	//const ulong timeOut = 500;
	//bool waitOK = WaitRenderTimeout(timeOut);
	//if (!waitOK)
	//{
	//	return;
	//}

	const ulong timeOut = 500;
	bool waitOK = _event.Wait(timeOut);
	if (!waitOK)
	{
		return;
	}


	QMutexLocker lockerRender{ &_renderLock };
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

void XThreadGLWindow::ThreadRender()
{
	QMutexLocker lockerRender{ &_renderLock };
	
	xxxDelay();

	Render();
	_glctx->moveToThread(this->thread());
	
	lockerRender.unlock();

	//QMutexLocker lockerTimeout{ &_glwin->_timeoutLock };
	//_glwin->_isRenerering = false;
	//lockerTimeout.unlock();
	//_glwin->_condTimeout.wakeOne();
	_event.Notify(false);

	emit RenderFrameDoneSignal();
}

void XThreadGLWindow::Render()
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


void XThreadGLWindow::paintEvent(QPaintEvent* ev)
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

void XThreadGLWindow::resizeEvent(QResizeEvent* ev)
{
	if (_SurfaceDestroyed)
	{
		return;
	}

	//QMutexLocker lockerTimeout{ &_timeoutLock };
	//const ulong timeOut = 500;
	//bool waitOK = WaitRenderTimeout(timeOut);
	//if (!waitOK)
	//{
	//	return;
	//}

	bool initGL = false;

	QMutexLocker lockerRender{ &_renderLock };

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

bool XThreadGLWindow::event(QEvent* ev)
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

void XThreadGLWindow::FinalizeGL()
{
	QMutexLocker lockerRender{ &_renderLock };

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

bool XThreadGLWindow::MakeCurrentCtx(const char* msg, const char* msg1)
{
	bool ok = _glctx->makeCurrent(this);
	Q_ASSERT_X(ok && _glctx->isValid(), msg, msg1);
	return ok;
}

void XThreadGLWindow::DoneCurrentCtx()
{
	_glctx->doneCurrent();
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

void XThreadGLWindow::SetAnimating(bool run)
{
	_animating = run;
	if (_animating)
	{
		//connect(_render, &XThreadGLRender::RenderFrameDoneSignal, this, &XThreadGLWindow::requestUpdate);
		connect(this, &XThreadGLWindow::RenderFrameDoneSignal, this, &XThreadGLWindow::requestUpdate);
		requestUpdate();
	}
	else
	{
		//disconnect(_render, &XThreadGLRender::RenderFrameDoneSignal, this, &XThreadGLWindow::requestUpdate);
		disconnect(this, &XThreadGLWindow::RenderFrameDoneSignal, this, &XThreadGLWindow::requestUpdate);
	}
}

XThreadGLWindow::XThreadGLWindow(QWindow* parent)
	: QWindow(parent)
{
	setSurfaceType(QSurface::OpenGLSurface);

	_thread = new QThread{ this };
	_render = new XThreadGLRender{ nullptr, this };
	_render->moveToThread(_thread);

	connect(_thread, &QThread::finished, _render, &QObject::deleteLater);
	connect(this, &XThreadGLWindow::RequestRenderSignal, _render, &XThreadGLRender::Render);

	_thread->start();
}

XThreadGLWindow::~XThreadGLWindow()
{
	_thread->quit();
	_thread->wait();

	if (_glctx)
	{
		_glctx->deleteLater();
		_glctx = nullptr;
	}
	//FinalizeGL();

}
