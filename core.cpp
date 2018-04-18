#include <iostream>
#include "core.h"
#include "event_handler.h"
#include "request.h"
#include "server.h"
#include "thread.h"

Core::Core(Server* serverPtr)
{
	status = IDLE;
	server = serverPtr;
	currentThreadIter = threads.end();
}

CoreStatus Core::getStatus()
{
	return status;
}

int Core::getNumThreads()
{
	return threads.size();
}

Request* Core::getCurrentExecutingRequest()
{
	if (currentThreadIter == threads.end())
	{
		return NULL;
	}
	return (*currentThreadIter)->getRequest();
}

// TODO(distribution): Keep In mind context switch overhead
void Core::scheduleThread(Time t)
{
	if (threads.size() == 0)
	{
		if (status == BUSY)
		{
			status = IDLE;
			server->decrementCoresInUse();
		}
	}
	else
	{
		if (currentThreadIter == threads.end())
		{
			currentThreadIter = threads.begin();
		}

		Thread* thr = *currentThreadIter;
		thr->startRequest(t);

		if (status == IDLE)
		{
			status = BUSY;
			server->incrementCoresInUse();
		}

		Request* req = thr->getRequest();

		if (server->getTimeQuantum() > req->getRemainingServiceTime())
		{
			EventHandler::getInstance()->addReqCompEvent(t + req->getRemainingServiceTime(), req);
		}
		else
		{
			EventHandler::getInstance()->addContextSwitchEvent(t + server->getTimeQuantum(), this);
		}
	}

}
void Core::removeCurrentThread(Time t)
{
	if (currentThreadIter == threads.end())
	{
		std::cerr<<"No thread is executing\n";
		exit(1);
	}
	Thread* thr = *currentThreadIter;
	thr->stopRequest(t);
	currentThreadIter = threads.erase(currentThreadIter);
	server->removeThread();
	scheduleThread(t);
}

void Core::contextSwitch(Time t)
{
	if (currentThreadIter == threads.end())
	{
		std::cerr<<"No thread is executing\n";
		exit(1);
	}

	Thread* thr = *currentThreadIter;
	thr->stopRequest(t);
	currentThreadIter++;
	scheduleThread(t);
}
void Core::addThread(Thread* thr, Time t)
{
	threads.push_back(thr);
	if (status == IDLE)
	{
		scheduleThread(t);
	}
}
