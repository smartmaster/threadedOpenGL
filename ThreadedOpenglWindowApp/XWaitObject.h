#pragma once

#include <QMutexLocker>
#include <QWaitCondition>
#include <QMutex>

class XEvent final
{
private:
	volatile bool _busy{ false };
	QMutex _mutex;
	QWaitCondition _cond;

public:
	XEvent(bool busy);
	bool Wait(uint timeout);
	void Notify(bool all);
};


class XSemphore final
{
private:
	volatile int _couter{ 0 };
	QMutex _mutex;
	QWaitCondition _cond;

public:
	XSemphore(int counter);
	bool Wait(uint timeout);
	void Notify(bool all);
};