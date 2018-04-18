#ifndef REQUEST_H
#define REQUEST_H

typedef double Time;

class User;
class Thread;

enum RequestStatus : bool
{
	GOOD = true,
	BAD = false,
};

class Request
{
	// int ID;
	RequestStatus status;
	bool executing;
	User* issuer;
	Thread* assignedThread;
	Time arrivalTime;
	Time remainingServiceTime;
	Time startTimeOfCurQuantum;

public:
	Request(User* user, Time curTime);

	RequestStatus getStatus() const;
	bool isExecuting() const;
	User* getIssuer() const;
	Thread* getThread() const;
	Time getArrivalTime() const;
	Time getRemainingServiceTime() const;

	void setStatus(RequestStatus st);

	void fininshRunning(Time t);

	void startRunning(Time t);
};

#endif
