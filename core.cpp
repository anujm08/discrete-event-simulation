#include <iostream>
#include "core.h"
#include "event_handler.h"
#include "request.h"
#include "server.h"
#include "thread.h"

Core::Core(Server* serverPtr, int coreID)
{
	id = coreID;
	status = IDLE;
	server = serverPtr;
	currentThreadIter = threads.end();
}

CoreStatus Core::getStatus() const
{
	return status;
}

int Core::getID() const
{
	return id;
}
int Core::getNumThreads() const
{
	return threads.size();
}

Request* Core::getCurrentExecutingRequest() const
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
		Time contextswitchTime = server->getContextSwitchTime();
		if (currentThreadIter == threads.end())
		{
			currentThreadIter = threads.begin();
		}

		if (status == IDLE)
		{
			status = BUSY;
			server->incrementCoresInUse();
			contextswitchTime = 0;
		}

		Thread* thr = *currentThreadIter;
		thr->startRequest(t + contextswitchTime);

		Request* req = thr->getRequest();

		if (req->getRemainingServiceTime() <= server->getTimeQuantum())
		{
			EventHandler::getInstance()->addReqCompEvent(t + contextswitchTime + req->getRemainingServiceTime(), req);
		}
		else
		{
			EventHandler::getInstance()->addContextSwitchEvent(t + contextswitchTime + server->getTimeQuantum(), this);
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
