#include "request.h"

Request::Request(User* user, Time curTime)
{
	status = GOOD;
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

void Request::setStatus(RequestStatus st)
{
	status = st;
}

void Request::fininshRunning(Time t)
{
	remainingServiceTime -= (t - startTimeOfCurQuantum);
	startTimeOfCurQuantum = -1;
	executing = false;
}

void Request::startRunning(Time t)
{
	startTimeOfCurQuantum = t;
	executing = true;
}
