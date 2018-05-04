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
#include "time_distribution.h"
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
	QueuingNetwork(int numUsers, int bufferSize, int numCores, int threadLimit, Time tQuantum, Time csTime)
	: server(numCores, threadLimit, tQuantum, csTime)
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
	Time startTime;
	Time lastAreaUpdateTime;
	int numGoodReqCompleted;
	int numBadReqCompleted;
	int numReqDropped;
	Time goodRespTimeTotal;
	Time badRespTimeTotal;
	double coreUtilizationArea;
	double numReqArea;

public:
	Metrics(Time metricStartTime)
	{
		startTime = metricStartTime;
		lastAreaUpdateTime = metricStartTime;
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
		// Update Metrics only after transient period
		if (t > startTime)
		{
			numReqArea += (q.getNumReq() * (t - lastAreaUpdateTime));
			coreUtilizationArea += (q.getNumCoresInUse() * (t - lastAreaUpdateTime));
		}
		lastAreaUpdateTime = max(t, lastAreaUpdateTime);
	}

	void updateReqComp(Request* req, Time t)
	{
		if (t > startTime)
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
	}

	void incrementReqDropped(Time t)
	{
		if (t > startTime)
			numReqDropped++;
	}

	void print()
	{
		Time totalTime = lastAreaUpdateTime - startTime;
		if (totalTime <= 0)
		{
			cout<<"Simulation Run time is less than transient time. No results Collected!" << endl;
			return;
		}
		cout << "Metric Start Time: " << startTime << endl;
		cout << "Metric End Time: " << lastAreaUpdateTime << endl;
		cout << "Num of good requests completed: " << numGoodReqCompleted << endl;
		cout << "Num of bad requests completed: " << numBadReqCompleted << endl;
		cout << "Num of requests dropped: " <<numReqDropped << endl;
		cout << "Goodput: " << 1.0 * numGoodReqCompleted / totalTime << endl;
		cout << "Badput: " << 1.0 * numBadReqCompleted / totalTime << endl;
		cout << "Throughput: " << 1.0 * (numGoodReqCompleted + numBadReqCompleted) / totalTime << endl;

		cout << "Response Time of Good Reqs: ";
		if (numGoodReqCompleted > 0)
			cout << 1.0 * goodRespTimeTotal / numGoodReqCompleted<<"" << endl;
		else
			cout << "-" << endl;

		cout << "Response Time of Bad Reqs: ";
		if (numBadReqCompleted > 0)
			cout << 1.0 * badRespTimeTotal / numBadReqCompleted<<"" << endl;
		else
			cout << "-" << endl;

		cout << "Response Time of All Reqs: ";
		if (numGoodReqCompleted + numBadReqCompleted > 0)
			cout << 1.0 * (badRespTimeTotal + goodRespTimeTotal) / (numGoodReqCompleted + numBadReqCompleted) << endl;
		else
			cout << "-" << endl;
		cout << "Average Number of Cores Utilized: " << coreUtilizationArea / totalTime << endl;
		cout << "Average Number of Requests in System: " << numReqArea / totalTime << endl;
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
	Simulation(int numUsers, int bufferSize, int numCores, int threadLimit, Time tQuantum, Time csTime, Time transientTime, bool verbose = false)
	: queuingNetwork(numUsers, bufferSize, numCores, threadLimit, tQuantum, csTime), metrics(transientTime)
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
						metrics.incrementReqDropped(simulationTime);
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
					if (req->isCompleted() || req->isDropped())
					{
						// Ignore and delete if already completed or dropped
						delete req;
						// Don't print this event
						continue;
					}
					else
					{
						// Timeout request to update status
						req->timeout();
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
				cout << "Simulation Time = " << simulationTime;
				cout << " | Event = " << e.getEventName();
				cout << " | Busy Cores = " << queuingNetwork.getNumCoresInUse();
				cout << " | Requests in System = " << queuingNetwork.getNumReq();
				cout << " | Active Threads = " << queuingNetwork.getNumActiveThreads();
				cout << endl;
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
	// int seed = 999;
	// int numUsers = 20;
	// int bufferSize = 200;
	// int numCores = 8;
	// int threadLimit = 100;

	// Time tQuantum = 100;
	// Time csTime = 0.01;
	// Time totalSimulationTime = 1000;
	// Time transientTime = totalSimulationTime / 10;

	// Time thinkMean = 5;
	// Time thinkVariance = 1;

	// Time serviceConst = 2;
	// Time serviceUniformMin = 1;
	// Time serviceUniformMax = 3;
	// Time serviceExpMean = 2;
	// float serviceProb1 = 0.2;
	// float serviceProb2 = 0.5;

	// Time timeOutMinm = 10;
	// Time timeOutExpMean = 5;

	int seed;
	int numUsers, numCores;
	int bufferSize, threadLimit;

	Time tQuantum, csTime, totalSimulationTime, transientTime;
	Time thinkMean, thinkVariance;
	Time serviceConst, serviceUniformMin, serviceUniformMax, serviceExpMean;
	Time timeOutMinm, timeOutExpMean;
	float serviceProb1, serviceProb2;

	cin >> seed;
	cin >> numUsers;
	cin >> numCores;
	cin >> bufferSize;
	cin >> threadLimit;

	cin >> tQuantum;
	cin >> csTime;
	cin >> transientTime;
	cin >> totalSimulationTime;

	cin >> thinkMean;
	cin >> thinkVariance;

	cin >> serviceConst;
	cin >> serviceProb1;

	cin >> serviceUniformMin;
	cin >> serviceUniformMax;
	cin >> serviceProb2;

	cin >> serviceExpMean;

	cin >> timeOutMinm;
	cin >> timeOutExpMean;

	TimeDistribution::setSeed(seed);
	TimeDistribution::setThinkTimeDistribution(thinkMean, thinkVariance);
	TimeDistribution::setServiceTimeDistribution(serviceConst, serviceUniformMin, serviceUniformMax, serviceExpMean, serviceProb1, serviceProb2);
	TimeDistribution::setTimeOutDistribution(timeOutMinm, timeOutExpMean);

	Simulation simulation(numUsers, bufferSize, numCores, threadLimit, tQuantum, csTime, transientTime, true);
	simulation.simulate(totalSimulationTime);
	simulation.printMetrics();

	return 0;
}
