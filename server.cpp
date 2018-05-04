#include <iostream>
#include "server.h"
#include "core.h"
#include "request.h"
#include "thread.h"

int Server::getCoreWithLeastThread()
{
	int minm = numActiveThreads;
	int index = 0;
	for (int i = 0; i < cores.size(); i++)
	{
		if (cores[i]->getNumThreads() < minm)
		{
			minm = cores[i]->getNumThreads();
			index = i;
		}
	}
	return index;
}

Server::Server(int numCores, int threadLimit, Time tQuantum, Time csTime)
{
	cores.reserve(numCores);
	for (int i = 0; i < numCores; i++)
	{
		cores.push_back(new Core(this, i));
	}
	maxNumThreads = threadLimit;
	timeQuantum = tQuantum;
	contextSwitchTime = csTime;
	numActiveThreads = 0;
	numCoresInUse = 0;
}

Time Server::getTimeQuantum() const
{
	return timeQuantum;
}

Time Server::getContextSwitchTime() const
{
	return contextSwitchTime;
}

int Server::getNumActiveThreads() const
{
	return numActiveThreads;
}

int Server::getMaxNumThreads() const
{
	return maxNumThreads;
}

int Server::getNumCoresInUse() const
{
	return numCoresInUse;
}

bool Server::canAddThread() const
{
	return numActiveThreads < maxNumThreads;
}

int Server::incrementCoresInUse()
{
	if (numCoresInUse > cores.size())
	{
		std::cerr<<"All Cores are already in use\n";
		exit(1);
	}
	numCoresInUse++;
}

int Server::decrementCoresInUse()
{
	if (numCoresInUse ==0)
	{
		std::cerr<<"No core is in use\n";
		exit(1);
	}
	numCoresInUse--;
}

int Server::removeThread()
{
	if (numActiveThreads == 0)
	{
		std::cerr << "No Thread on server\n";
		exit(1);
	}
	numActiveThreads--;
}

void Server::assignReq(Request* req, Time t)
{
	if (!canAddThread())
	{
		std::cerr<<"Already Max Num of Threads\n";
		exit(1);
	}

	int coreIndex = getCoreWithLeastThread();
	Thread* thr = new Thread(req, cores[coreIndex]);
	req->assignThread(thr);
	numActiveThreads++;
	cores[coreIndex]->addThread(thr, t);
}
