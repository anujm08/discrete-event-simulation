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
	RequestStatus status;
	bool executing;
	bool completed;
	bool dropped;
	User* issuer;
	Thread* assignedThread;
	Time arrivalTime;
	Time remainingServiceTime;
	Time startTimeOfCurQuantum;

public:
	Request(User* user, Time curTime);

	RequestStatus getStatus() const;
	bool isCompleted() const;
	bool isExecuting() const;
	bool isDropped() const;
	User* getIssuer() const;
	Thread* getThread() const;
	Time getArrivalTime() const;
	Time getRemainingServiceTime() const;

	void assignThread(Thread* thr);
	void timeout();
	void setCompleted();
	void setDropped();
	void fininshRunning(Time t);
	void startRunning(Time t);
};

#endif
