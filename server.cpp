#include <iostream>
#include "server.h"
#include "core.h"
#include "thread.h"

int Server::getCoreWithLeastThread()
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

Server::Server(int numCores, Time tQuantum, int threadLimit)
{
	cores.resize(numCores, Core(this));
	timeQuantum = tQuantum;
	maxNumThreads = threadLimit;
	numActiveThreads = 0;
	numCoresInUse = 0;
}

Time Server::getTimeQuantum() const
{
	return timeQuantum;
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
	Thread* thr = new Thread(req, &cores[coreIndex]);
	cores[coreIndex].addThread(thr, t);
}
