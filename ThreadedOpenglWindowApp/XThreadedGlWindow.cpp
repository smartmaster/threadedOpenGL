#include <QtGlobal>

#include "XThreadedGlWindow.h"
#include <QMutexLocker>

#define DoneCurrentCtx(glctx) glctx->doneCurrent()

void XThreadedGLWindow::Render()
{
	if (isExposed())
	{
		bool ok = _glctx->makeCurrent(this);
		Q_ASSERT_X(ok && _glctx->isValid(), __FILE__, __FUNCTION__);
		GLPaint(_paintDev);
		_glctx->swapBuffers(this);

		DoneCurrentCtx(_glctx);
		//if (_animating)
		//{
		//	requestUpdate();
		//}

	}
}

void XThreadedGLWindow::NotifyRenderer()
{
	{
		QMutexLocker locker{ &_renderLock };
		_glctx->moveToThread(_thread);
		emit RendererSignal();
	}

	if (_animating)
	{
		requestUpdate();
	}
}

void XThreadedGLWindow::paintEvent(QPaintEvent* ev)
{
	NotifyRenderer();
}

//void XThreadedGLWindow::exposeEvent(QExposeEvent* ev)
//{
//	NotifyRenderer();
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

void XThreadedGLWindow::resizeEvent(QResizeEvent* ev)
{
	if (_SurfaceAboutToBeDestroyed)
	{
		return;
	}

	QMutexLocker locker{ &_renderLock };
	
	if (nullptr == _glctx)
	{
		//_glctx = new QOpenGLContext{ this };
		_glctx = new QOpenGLContext{ nullptr }; //cannot have parent for moving thread
		_glctx->setFormat(requestedFormat());
		_glctx->setShareContext(nullptr);
		_glctx->create();
		bool ok = _glctx->makeCurrent(this);
		Q_ASSERT_X(ok && _glctx->isValid(), __FILE__, __FUNCTION__);

		_paintDev = new QOpenGLPaintDevice{};


		initializeOpenGLFunctions();
		glDebugMessageCallback(GLDebugPoc, nullptr);


		GLInitialize();
	}
	else
	{
		bool ok = _glctx->makeCurrent(this);
		Q_ASSERT_X(ok && _glctx->isValid(), __FILE__, __FUNCTION__);
	}

	qreal dpr = devicePixelRatio();
	_paintDev->setDevicePixelRatio(dpr);
	_paintDev->setSize(ev->size() * dpr);

	GLResize(ev->size(), ev->oldSize());

	DoneCurrentCtx(_glctx);
}

//void XThreadedGLWindow::GLInitialize()
//{
//
//}
//
//void XThreadedGLWindow::GLResize(const QSize& size, const QSize& oldSize)
//{
//
//}
//
//void XThreadedGLWindow::GLPaint(QPaintDevice* paintDev)
//{
//
//}
//
//void XThreadedGLWindow::GLFinalize()
//{
//
//}

bool XThreadedGLWindow::event(QEvent* ev)
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
		NotifyRenderer();
		return true;

	case QEvent::PlatformSurface:
	{
		auto* psev = (QPlatformSurfaceEvent*)(ev);
		if (QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed == psev->surfaceEventType())
		{
			_SurfaceAboutToBeDestroyed = true;
			FinalizeGL();
		}
	}
	return XQTBase::event(ev);


	//case QEvent::Paint:
	//case QEvent::Expose:
	//	Render();
	//	return true;

	default:
		return XQTBase::event(ev);
		break;
	}


}

void XThreadedGLWindow::FinalizeGL()
{
	QMutexLocker locker{ &_renderLock };

	if (_glctx)
	{
		bool ok = _glctx->makeCurrent(this);
		Q_ASSERT_X(ok && _glctx->isValid(), __FUNCTION__, __FILE__);

		GLFinalize();

		delete _paintDev;
		_paintDev = nullptr;

		//_glctx->deleteLater();
		//_glctx = nullptr;

		DoneCurrentCtx(_glctx);
	}
}

void XThreadedGLWindow::SetAnimating(bool run)
{
	_animating = run;
	if (_animating)
	{
		requestUpdate();
	}
}

XThreadedGLWindow::XThreadedGLWindow(QWindow* parent)
	: QWindow(parent)
{
	setSurfaceType(QSurface::OpenGLSurface);

	_thread = new QThread{ this };
	_renderer = new XThreadedGLWindowRenderer{ nullptr, this };
	_renderer->moveToThread(_thread);

	connect(_thread, &QThread::finished, _renderer, &QObject::deleteLater);
	connect(this, &XThreadedGLWindow::RendererSignal, _renderer, &XThreadedGLWindowRenderer::Render);

	_thread->start();
}

XThreadedGLWindow::~XThreadedGLWindow()
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

XThreadedGLWindowRenderer::XThreadedGLWindowRenderer(QObject* parent, XThreadedGLWindow* window) :
	QObject{ parent },
	_glwin{ window }
{
}

void XThreadedGLWindowRenderer::Render()
{
	QMutexLocker locker{ &_glwin->_renderLock };
	_glwin->Render();
	_glwin->_glctx->moveToThread(_glwin->thread());
}
