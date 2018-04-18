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
	// int ID;
	CoreStatus status;
	std::list<Thread*> threads;
	std::list<Thread*>::iterator currentThreadIter;
	Server* server;

public:
	Core(Server* serverPtr);

	CoreStatus getStatus();
	int getNumThreads();

	Request* getCurrentExecutingRequest();

	// TO DO: Keep In mind context switch overhead
	void scheduleThread(Time t);
	void removeCurrentThread(Time t);
	void contextSwitch(Time t);
	void addThread(Thread* thr, Time t);
};

#endif
