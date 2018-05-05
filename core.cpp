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

void Core::scheduleThread(Time t, bool contextSwitch)
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
		Time contextswitchTime = (contextSwitch) ? server->getContextSwitchTime() : 0;
		if (currentThreadIter == threads.end())
		{
			currentThreadIter = threads.begin();
		}

		if (status == IDLE)
		{
			status = BUSY;
			server->incrementCoresInUse();
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
	scheduleThread(t, false);
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

	if (threads.size() == 1)
	{
		scheduleThread(t, false);
	}
	else
	{
		currentThreadIter++;
		scheduleThread(t, true);
	}
}
void Core::addThread(Thread* thr, Time t)
{
	threads.push_back(thr);
	if (status == IDLE)
	{
		scheduleThread(t, false);
	}
}
