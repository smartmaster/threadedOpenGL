#pragma once

#include <QMutexLocker>
#include <QWaitCondition>
#include <QMutex>

class SmlEvent final
{
private:
	volatile bool _busy{ false };
	QMutex _mutex;
	QWaitCondition _cond;

public:
	SmlEvent(bool busy);
	bool Wait(uint timeout);
	void Notify(bool all);
};


class SmlSemphore final
{
private:
	volatile int _couter{ 0 };
	QMutex _mutex;
	QWaitCondition _cond;

public:
	SmlSemphore(int counter);
	bool Wait(uint timeout);
	void Notify(bool all);
};


template<typename TMUTEX>
class SmlMTLocker
{
private:
	bool _locked{ false };
	TMUTEX* _mtx{ nullptr };
public:
	SmlMTLocker(TMUTEX* mtx, bool mt) :
		_mtx{ mtx }
	{
		if (mt && _mtx)
		{
			_mtx->lock();
			_locked = true;
		}
	}

	void unlock()
	{
		if (_locked && _mtx)
		{
			_mtx->unlock();
			_mtx = nullptr;
			_locked = false;
		}
	}

	~SmlMTLocker()
	{
		unlock();
	}
};