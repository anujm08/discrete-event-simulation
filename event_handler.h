#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <queue>
#include <string>

typedef double Time;

class Request;
class User;
class Core;

enum EventType : int
{
	NEW_REQ = 0,
	REQ_COMP = 1,
	REQ_TOUT = 2,
	CTX_SWTCH = 3,
};

class Event
{
	Time time;
	EventType type;
	void* ptr;

public:

	Event(Time t, EventType eType, void* pt);

	Time getTime() const;
	EventType getType() const;
	std::string getEventName() const;
	void* getPtr() const;

	bool operator< (const Event& e) const;
};

class EventHandler
{
	std::priority_queue<Event> eventList;
	static EventHandler* instance;
	EventHandler() {}

public:

	void addNewReqEvent(Time t, User* user);
	void addReqCompEvent(Time t, Request* req);
	void addReqTimeOutEvent(Time t, Request* req);
	void addContextSwitchEvent(Time t, Core* core);

	Event getNextEvent();

	static EventHandler* getInstance();
};

#endif
