#include <stdio.h>
#include <iostream>
#include <cmath>
#include <list>
#include <queue>
#include <cstdlib>
#include <utility>

using namespace std;

typedef double Time;

class User;
class Core;
class Server;
class Thread;
class Request;
class QueuingNetwork;
class EventHandler;
class Metrics;
class Simulation;


enum UserStatus : bool
{
	WAIT = true,
	THINK = false,
};

class User
{
	// int ID;
	UserStatus status;
	Request* issuedReq;

public:
	User()
	{
		status = THINK;
		issuedReq = NULL;
	}
};


enum CoreStatus : bool
{
	BUSY = true,
	IDLE = false,
};

class Core
{
	// int ID;
	CoreStatus status;
	list<Thread*> threads;
	list<Thread*>::iterator currentThreadIter;
	Server *server;

public:
	Core(Server* serverPtr)
	{
		status = IDLE;
		server = serverPtr;
		currentThreadIter = threads.end();
	}

	CoreStatus getStatus()
	{
		return status;
	}

	int getNumThreads()
	{
		return threads.size();
	}

	Request* getCurrentExecutingRequest()
	{
		if (currentThreadIter == threads.end())
		{
			return NULL;
		}
		return (*currentThreadIter)->getRequest();
	}

	// TO DO: Keep In mind context switch overhead
	void scheduleThread()
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
			thr->startRequest();

			if (status == IDLE)
			{
				status = BUSY;
				server->incrementCoresInUse();
			}
			thr->getRequest();
		}

	}
	void removeCurrentThread()
	{
		if (currentThreadIter == threads.end())
		{
			cerr<<"No thread is executing\n";
			exit(1);
		}
		Thread* thr = *currentThreadIter;
		thr->stopRequest();
		currentThreadIter = threads.erase(currentThreadIter);
		server->removeThread();
		return scheduleThread();
	}

	void contextSwitch()
	{
		if (currentThreadIter == threads.end())
		{
			cerr<<"No thread is executing\n";
			exit(1);
		}
		
		Thread* thr = *currentThreadIter;
		thr->stopRequest();
		currentThreadIter++;
		scheduleThread();
	}
	void addThread(Thread* thr)
	{
		threads.push_back(thr);
		if (status == IDLE)
		{
			scheduleThread();
		}
	}

};

class Server
{
	vector<Core> cores;
	Time timeQuantum;
	int maxNumThreads;
	int numActiveThreads;
	int numCoresInUse;

	int getCoreWithLeastThread()
	{
		int minm = -1;
		int index = 0;
		for (int i = 0; i < cores.size(); i++)
		{
			if (cores[i].getNumThreads() < minm)
			{
				minm = cores[i].getNumThreads();
				index = i;
			}
		}
		return index;
	}

public:
	Server(int numCores, Time tQuantum, int threadLimit)
	{
		cores.resize(numCores, Core(this));
		timeQuantum = tQuantum;
		maxNumThreads = threadLimit;
		numActiveThreads = 0;
		numCoresInUse = 0;
	}

	Time getTimeQuantum() const
	{
		return timeQuantum;
	}

	int getNumActiveThreads() const
	{
		return numActiveThreads;
	}

	int getMaxNumThreads() const
	{
		return maxNumThreads;
	}

	int getNumCoresInUse() const
	{
		return numCoresInUse;
	}

	bool canAddThread() const
	{
		return numActiveThreads < maxNumThreads;
	}

	int incrementCoresInUse()
	{
		if (numCoresInUse > cores.size())
		{
			cerr<<"All Cores are already in use\n";
			exit(1);
		}
		numCoresInUse++;
	}

	int decrementCoresInUse()
	{
		if (numCoresInUse ==0)
		{
			cerr<<"No core is in use\n";
			exit(1);
		}
		numCoresInUse--;
	}

	int removeThread()
	{
		if (numActiveThreads == 0)
		{
			cerr << "No Thread on server\n";
			exit(1);
		}
		numActiveThreads--;
	}

	void assignReq(Request* req)
	{
		if (!canAddThread())
		{
			cerr<<"Already Max Num of Threads\n";
			exit(1);
		}

		int coreIndex = getCoreWithLeastThread();
		Thread* thr = new Thread(req, &cores[coreIndex]);
		cores[coreIndex].addThread(thr);
	}
};

class Thread
{
	// int ID;
	Request* curRequest;
	Core* affinedCore;

public:
	Thread(Request* req, Core* core)
	{
		curRequest = req;
		affinedCore = core;
	}

	Request* getRequest() const
	{
		return curRequest;
	}

	void stopRequest()
	{
		curRequest->fininshRunning();
	}

	void startRequest()
	{
		curRequest->startRunning();
	}
};

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
	Request(User* user, Time curTime, Time serviceTime)
	{
		status = GOOD;
		executing = false;
		issuer = user;
		assignedThread = NULL;
		arrivalTime = curTime;
		remainingServiceTime = serviceTime;
		startTimeOfCurQuantum = -1;
	}

	RequestStatus getStatus() const
	{
		return status;
	}

	bool isExecuting() const
	{
		return executing;
	}

	User* getIssuer() const
	{
		return issuer;
	}

	Thread* getThread() const
	{
		return assignedThread;
	}

	Time getArrivalTime() const
	{
		return arrivalTime;
	}

	Time getRemainingServiceTime() const
	{
		return remainingServiceTime;
	}

	void fininshRunning(Time t)
	{
		remainingServiceTime -= (t - startTimeOfCurQuantum);
		startTimeOfCurQuantum = -1;
		executing = false;
	}

	void startRunning(Time t)
	{
		startTimeOfCurQuantum = t;
		executing = true;
	}

};

class QueuingNetwork
{
	vector<User> users;
	Server server;
	queue<Request*> buffer;
	int maxBufferSize;

public:
	QueuingNetwork(int numUsers, int bufferSize, int numCores, Time tQuantum, int threadLimit)
	: server(numCores, tQuantum, threadLimit)
	{
		users.resize(numUsers);
		maxBufferSize = bufferSize;
	}

	int getNumReq() const
	{
		if (buffer.size() == 0)
		{
			return server.getNumActiveThreads();
		}
		else
		{
			return server.getNumActiveThreads() + buffer.size();
		}
	}

	int getNumCoresInUse() const
	{
		return server.getNumCoresInUse();
	}

	bool canAddReq() const
	{
		return buffer.size() < maxBufferSize;
	}

	int addRequest(Request* req)
	{
		if (!canAddReq())
		{
			return -1;
		}

		if (buffer.size() == 0 and server.canAddThread())
		{
			server.assignReq(req);
		}
		else
		{
			buffer.push(req);
		}
	}

	void assignNextRequest()
	{
		if (buffer.size() == 0)
		{
			return;
		}

		Request* req = buffer.front();
		buffer.pop();
		
		if (!server.canAddThread())
		{
			cerr<<"Can't assign Request. Already Max Threads\n";
			exit(1);
		}
		server.assignReq(req);
	}
};

enum Event : int
{
	NEW_REQ = 0,
	REQ_COMP = 1,
	REQ_TOUT = 2,
	CTX_SWTCH = 3,
};

class EventHandler
{
	priority_queue<pair<Event, void*> > eventList;

public:
	EventHandler()
	{

	}

	void addNewReqEvent(User* user)
	{
		eventList.push(make_pair(NEW_REQ, user));
	}

	void addReqCompEvent(Request* req)
	{
		eventList.push(make_pair(REQ_COMP, req));
	}

	void addReqTimeOutEvent(Request* req)
	{
		eventList.push(make_pair(REQ_TOUT, req));
	}

	void addContextSwitchEvent(Core* core)
	{
		eventList.push(make_pair(CTX_SWTCH, core));
	}

	pair<Event, void*> getNextEvent()
	{
		if (eventList.size() == 0)
		{
			cerr<<"Event List Empty\n";
		}
		pair<Event, void*> nextEvent = eventList.top();
		eventList.pop();
		return nextEvent;
	}
};

class Metrics
{
	Time simStartTime;
	Time lastAreaUpdateTime;
	int numGoodReqCompleted;
	int numBadReqCompleted;
	int numReqDropped;
	Time goodRespTimeTotal;
	Time badRespTimeTotal;
	double coreUtilizationArea;
	double numReqArea;

public:
	Metrics(Time startTime)
	{
		simStartTime = simStartTime;
		lastAreaUpdateTime = -1;
		numGoodReqCompleted = 0;
		numBadReqCompleted = 0;
		numReqDropped = 0;
		goodRespTimeTotal = 0.0;
		badRespTimeTotal = 0.0;
		coreUtilizationArea = 0.0;
		numReqArea = 0.0;
	}

	void updateAreaMetric(const QueuingNetwork &q, Time t)
	{
		numReqArea += (q.getNumReq() * (t - lastAreaUpdateTime));
		coreUtilizationArea += (q.getNumCoresInUse() * (t - lastAreaUpdateTime));
		lastAreaUpdateTime = t;
	}

	void updateReqComp(Request* req, Time t)
	{
		if (req->getStatus() == GOOD)
		{
			numGoodReqCompleted++;
			goodRespTimeTotal += t - req->getArrivalTime();
		}
	}

	void updateReqDropped(Time t)
	{
		numReqDropped++;
	}

};

class Simulation
{
	Time simulationTime;
	Time lastEventTime;
	QueuingNetwork queuingNetwork;
	EventHandler eventHandler;
	Metrics metrics;

public:
	// TODO :  Update metrics start time
	Simulation(int numUsers, int bufferSize, int numCores, Time tQuantum, int threadLimit)
	: queuingNetwork(numUsers, bufferSize, numCores, tQuantum, threadLimit), metrics(0.0)
	{
		simulationTime = 0.0;
		lastEventTime = 0.0;
	}
};