#include <iostream>
#include "request.h"

Request::Request(User* user, Time curTime)
{
	status = GOOD;
	dropped = false;
	completed = false;
	executing = false;
	issuer = user;
	assignedThread = nullptr;
	arrivalTime = curTime;
	//TODO(Distribution) : service Time
	remainingServiceTime = 1.0;
	startTimeOfCurQuantum = -1;
}

RequestStatus Request::getStatus() const
{
	return status;
}

bool Request::isExecuting() const
{
	return executing;
}

bool Request::isCompleted() const
{
	return completed;
}

bool Request::isDropped() const
{
	return dropped;
}

User* Request::getIssuer() const
{
	return issuer;
}

Thread* Request::getThread() const
{
	return assignedThread;
}

Time Request::getArrivalTime() const
{
	return arrivalTime;
}

Time Request::getRemainingServiceTime() const
{
	return remainingServiceTime;
}

void Request::assignThread(Thread* thr)
{
	if (assignedThread != nullptr)
	{
		std::cerr<<"Thread already assigned\n";
		exit(1);
	}
	assignedThread = thr;
}

void Request::timeout()
{
	status = BAD;
}

void Request::setCompleted()
{
	if (completed)
	{
		std::cerr<<"Request Already Completed\n";
		exit(1);
	}
	completed = true;
}

void Request::setDropped()
{
	if (dropped)
	{
		std::cerr<<"Request Already Dropped\n";
		exit(1);
	}
	dropped = true;
}

void Request::fininshRunning(Time t)
{
	if (!executing)
	{
		std::cerr<<"Request Not Running\n";
		exit(1);
	}
	remainingServiceTime -= (t - startTimeOfCurQuantum);
	startTimeOfCurQuantum = -1;
	executing = false;
}

void Request::startRunning(Time t)
{
	if (executing)
	{
		std::cerr<<"Request Already Running\n";
		exit(1);
	}
	startTimeOfCurQuantum = t;
	executing = true;
}
