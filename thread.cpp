#include "thread.h"
#include "request.h"

Thread::Thread(Request* req, Core* core)
{
	curRequest = req;
	affinedCore = core;
}

Request* Thread::getRequest() const
{
	return curRequest;
}

Core* Thread::getCore() const
{
	return affinedCore;
}

void Thread::stopRequest(Time t)
{
	curRequest->fininshRunning(t);
}

void Thread::startRequest(Time t)
{
	curRequest->startRunning(t);
}
