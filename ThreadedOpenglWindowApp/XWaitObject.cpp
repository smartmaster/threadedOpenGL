#include "XWaitObject.h"

#include <QMutexLocker>
#include <QWaitCondition>
#include <QMutex>


XEvent::XEvent(bool busy) :
	_busy(busy)
{

}

bool XEvent::Wait(uint timeout)
{
	bool waitok = true;
	QMutexLocker<QMutex> locker{&_mutex};
	while (_busy)
	{
		waitok = _cond.wait(&_mutex, timeout);
		if (!waitok)
		{
			break;
		}
	}

	if (waitok)
	{
		_busy = true;
	}

	locker.unlock();
	return waitok;
}

void XEvent::Notify(bool all)
{
	QMutexLocker<QMutex> locker{ &_mutex };
	_busy = false;
	locker.unlock();

	if (all)
	{
		_cond.notify_all();
	}
	else
	{
		_cond.notify_one();
	}
	
}

XSemphore::XSemphore(int counter) :
	_couter{counter}
{

}

bool XSemphore::Wait(uint timeout)
{
	bool waitok = true;

	QMutexLocker<QMutex> locker{ &_mutex };
	while (_couter == 0)
	{
		waitok = _cond.wait(&_mutex, timeout);
		if (!waitok)
		{
			break;
		}
	}

	if (waitok)
	{
		--_couter;
	}

	locker.unlock();
	return waitok;
}

void XSemphore::Notify(bool all)
{
	QMutexLocker<QMutex> locker{ &_mutex };
	++_couter;
	locker.unlock();

	if (all)
	{
		_cond.notify_all();
	}
	else
	{
		_cond.notify_one();
	}

}
