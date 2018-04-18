#ifndef SERVER_H
#define SERVER_H

#include <vector>

typedef double Time;
class Core;
class Request;

class Server
{
	std::vector<Core> cores;
	Time timeQuantum;
	int maxNumThreads;
	int numActiveThreads;
	int numCoresInUse;

	int getCoreWithLeastThread();

public:
	Server(int numCores, Time tQuantum, int threadLimit);

	Time getTimeQuantum() const;
	int getNumActiveThreads() const;
	int getMaxNumThreads() const;
	int getNumCoresInUse() const;

	int incrementCoresInUse();
	int decrementCoresInUse();
	int removeThread();

	bool canAddThread() const;

	void assignReq(Request* req, Time t);
};

#endif