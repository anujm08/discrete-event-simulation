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

	int getNumActiveThreads() const
	{
		return server.getNumActiveThreads();
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
		return 0;
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
		else
		{
			numBadReqCompleted++;
			badRespTimeTotal += t - req->getArrivalTime();
		}
	}

	void incrementReqDropped()
	{
		numReqDropped++;
	}

	void print()
	{
		Time totalTime = lastAreaUpdateTime - simStartTime;
		std::cout<<"Simulation Start Time: " <<simStartTime<<endl;
		std::cout<<"Simulation End Time: " <<lastAreaUpdateTime<<endl;
		std::cout<<"Num of good requests completed: " <<numGoodReqCompleted<<endl;
		std::cout<<"Num of bad requests completed: " <<numBadReqCompleted<<endl;
		std::cout<<"Num of requests dropped: " <<numReqDropped<<endl;

		std::cout<<"Response Time of Good Reqs: ";
		if (numGoodReqCompleted > 0)
			std::cout<<1.0 * goodRespTimeTotal / numGoodReqCompleted<<""<<endl;
		else
			std::cout<<"-"<<endl;

		std::cout<<"Response Time of Bad Reqs: ";
		if (numBadReqCompleted > 0)
			std::cout<<1.0 * badRespTimeTotal / numBadReqCompleted<<""<<endl;
		else
			std::cout<<"-"<<endl;

		std::cout<<"Response Time of All Reqs: ";
		if (numGoodReqCompleted + numBadReqCompleted > 0)
			std::cout<<1.0 * (badRespTimeTotal + goodRespTimeTotal) / (numGoodReqCompleted + numBadReqCompleted)<<endl;
		else
			std::cout<<"-"<<endl;
		std::cout<<"Average Number of Cores Utilized: "<<coreUtilizationArea / totalTime<<endl;
		std::cout<<"Average Number of Requests in System: "<<numReqArea / totalTime<<endl;
	}

};

class Simulation
{
	Time simulationTime;
	Time lastEventTime;
	QueuingNetwork queuingNetwork;
	Metrics metrics;
	bool printTrace;

public:
	// TODO :  Update metrics start time
	Simulation(int numUsers, int bufferSize, int numCores, Time tQuantum, int threadLimit, bool verbose = false)
	: queuingNetwork(numUsers, bufferSize, numCores, tQuantum, threadLimit), metrics(0.0)
	{
		simulationTime = 0.0;
		lastEventTime = 0.0;
		for (int i = 0; i < numUsers; i++)
		{
			queuingNetwork.startUserThinking(i, simulationTime);
		}
		printTrace = verbose;
	}

	void simulate(Time endTime)
	{
		while (true)
		{
			Event e = EventHandler::getInstance()->getNextEvent();
			lastEventTime = simulationTime;
			simulationTime = e.getTime();
			if (simulationTime > endTime)
			{
				metrics.updateAreaMetric(queuingNetwork, endTime);
				break;
			}

			metrics.updateAreaMetric(queuingNetwork, simulationTime);

			switch (e.getType())
			{
				case NEW_REQ:
				{
					User* user = (User*) e.getPtr();
					// TODO check error in casting
					Request* req = user->issueRequest(simulationTime);
					int addStatus = queuingNetwork.addRequest(req, simulationTime);
					if (addStatus != 0)
					{
						req->setDropped();
						metrics.incrementReqDropped();
					}
					break;
				}
				case REQ_COMP:
				{
					// Remove Thread, Assign Next Request from Buffer, User start thinking
					Request* req = (Request*) e.getPtr();
					metrics.updateReqComp(req, simulationTime);
					User* user = req->getIssuer();
					Thread* thr = req->getThread();
					Core* core = thr->getCore();
					core->removeCurrentThread(simulationTime);
					delete thr;
					// Delete Request if it has already timed out
					if (req->getStatus() == BAD)
					{
						delete req;
					}
					else
					{
						req->setCompleted();
					}
					//TODO Checks : request is executing,core->curThread is thr
					queuingNetwork.assignNextRequest(simulationTime);
					user->startThinking(simulationTime);
					break;
				}
				case REQ_TOUT:
				{
					Request* req = (Request*) e.getPtr();
					if (req->isCompleted())
					{
						// Ignore and delete if already completed
						delete req;
					}
					else
					{
						if (req->isDropped())
						{
							delete req;
						}
						else
						{
							// Timeout request to update status
							req->timeout();
						}
						// User starts thinking, before resending the request
						User* user = req->getIssuer();
						user->startThinking(simulationTime);
					}
					break;
				}
				case CTX_SWTCH:
				{
					Core* core = (Core*) e.getPtr();
					core->contextSwitch(simulationTime);
					break;
				}
			}

			if (printTrace)
			{
				cout<<"Simulation Time = "<<simulationTime;
				cout<<" | Event = "<<e.getEventName();
				cout<<" | Busy Cores = "<<queuingNetwork.getNumCoresInUse();
				cout<<" | Requests in System = "<<queuingNetwork.getNumReq();
				cout<<" | Active Threads = "<<queuingNetwork.getNumActiveThreads();
				cout<<endl;
			}
		}
	}

	void printMetrics()
	{
		metrics.print();
	}
};

int main()
{
	int numUsers = 100;
	int bufferSize = 200;
	int numCores = 4;
	Time tQuantum = 1;
	int threadLimit = 100;

	Simulation simulation(numUsers, bufferSize, numCores, tQuantum, threadLimit, true);
	simulation.simulate(1000);
	simulation.printMetrics();

	return 0;
}
