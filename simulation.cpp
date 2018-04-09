#include <stdio.h>
#include <cmath>
#include <list>
#include <queue>

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
	WAIT = TRUE,
	THINK = FALSE,
};

class User
{
	int ID;
	UserStatus status;
	Request* issuedReq;
};


enum CoreStatus : bool
{
	BUSY = TRUE,
	IDLE = FALSE,
};

class Core
{
	// int ID;
	CoreStatus status;
	list<Thread*> threads;
	list<Thread*>::iterator currentThread;
	Server *server;
};

class Server
{
	vector<Core> cores;
	Time timeQuantum;
	int maxNumThreads;
	int numActiveThreads;
	int numCoresInUse;

};

class Thread
{
	Request* curRequest;
	Core* affinedCore;
};

enum RequestStatus : bool
{
	GOOD = TRUE,
	BAD = FALSE,
};

class Request
{
	int ID;
	RequestStatus status;
	User* issuer;
	Thread* assignedThread;
	Time arrivalTime;
	Time remainingServiceTime;
	Time startTimeOfCurQuantum;

};

class QueuingNetwork
{
	vector<User> users;
	Server server;
	queue<Request*> buffer;
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
	priority_queue<Event, void*> eventList;
};

class Metrics
{
	Time simStartTime;
	Time simEndTime;
	int numGoodReqCompleted;
	int numBadReqCompleted;
	int numReqDropped;
	Time goodRespTimeTotal;
	Time badRespTimeTotal;
	double coreUtilArea;
	double numReqArea;
};

class Simulation
{
	Time simulationTime;
	Time lastEventTime;
	QueuingNetwork queuingNetwork;
	EventHandler eventHandler;
	Metrics metrics;
};