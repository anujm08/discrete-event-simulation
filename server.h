#ifndef SERVER_H
#define SERVER_H

#include <vector>

typedef double Time;
class Core;
class Request;

class Server
{
	std::vector<Core*> cores;
	Time timeQuantum;
	Time contextSwitchTime;
	int maxNumThreads;
	int numActiveThreads;
	int numCoresInUse;

	int getCoreWithLeastThread();

public:
	Server(int numCores, int threadLimit, Time tQuantum, Time csTime);

	Time getTimeQuantum() const;
	Time getContextSwitchTime() const;
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
