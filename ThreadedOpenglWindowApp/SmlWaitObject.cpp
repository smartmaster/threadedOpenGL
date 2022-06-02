#include "SmlWaitObject.h"

#include <QMutexLocker>
#include <QWaitCondition>
#include <QMutex>


SmlEvent::SmlEvent(bool busy) :
	_busy(busy)
{

}

bool SmlEvent::Wait(uint timeout)
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

void SmlEvent::Notify(bool all)
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

SmlSemphore::SmlSemphore(int counter) :
	_couter{counter}
{

}

bool SmlSemphore::Wait(uint timeout)
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

void SmlSemphore::Notify(bool all)
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
