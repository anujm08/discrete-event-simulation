#include <stdio.h>
#include <iostream>
#include <cmath>
#include <list>
#include <queue>
#include <cstdlib>
#include "event_handler.h"
#include "core.h"
#include "request.h"
#include "server.h"
#include "thread.h"
#include "user.h"
using namespace std;

typedef double Time;

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

	void startUserThinking(int index, Time t)
	{
		users[index].startThinking(t);
	}

	int addRequest(Request* req, Time t)
	{
		if (!canAddReq())
		{
			return -1;
		}

		if (buffer.size() == 0 and server.canAddThread())
		{
			server.assignReq(req, t);
		}
		else
		{
			buffer.push(req);
		}
	}

	void assignNextRequest(Time t)
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
		server.assignReq(req, t);
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
	Metrics metrics;

public:
	// TODO :  Update metrics start time
	Simulation(int numUsers, int bufferSize, int numCores, Time tQuantum, int threadLimit)
	: queuingNetwork(numUsers, bufferSize, numCores, tQuantum, threadLimit), metrics(0.0)
	{
		simulationTime = 0.0;
		lastEventTime = 0.0;
		for (int i = 0; i < numUsers; i++)
		{
			queuingNetwork.startUserThinking(i, simulationTime);

		}
	}

	void simulate(Time endTIme)
	{
		while(simulationTime < endTIme)
		{

		}
	}
};

int main()
{
	int numUsers = 100;
	int bufferSize = 60;
	int numCores = 8;
	Time tQuantum = 1;
	int threadLimit = 30;
	Simulation simulation(numUsers, bufferSize, numCores, tQuantum, threadLimit);
	return 0;
}