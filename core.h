#ifndef CORE_H
#define CORE_H

#include <list>

class Server;
class Thread;
class Request;

typedef double Time;

enum CoreStatus : bool
{
	BUSY = true,
	IDLE = false,
};

class Core
{
	int id;
	CoreStatus status;
	Server* server;

public:
	std::list<Thread*> threads;
	std::list<Thread*>::iterator currentThreadIter;
	Core(Server* serverPtr, int coreID);
	Core(const Core&) = delete;

	int getID() const;
	CoreStatus getStatus() const;
	int getNumThreads() const;

	Request* getCurrentExecutingRequest() const;

	void scheduleThread(Time t);
	void removeCurrentThread(Time t);
	void contextSwitch(Time t);
	void addThread(Thread* thr, Time t);
};

#endif
