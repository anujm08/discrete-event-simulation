#ifndef THREAD_H
#define THREAD_H

class Core;
class Request;

typedef double Time;

class Thread
{
	// int ID;
	Request* curRequest;
	Core* affinedCore;

public:
	Thread(Request* req, Core* core);

	Request* getRequest() const;
	Core* getCore() const;

	void stopRequest(Time t);
	void startRequest(Time t);
};

#endif