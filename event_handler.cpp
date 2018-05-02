#include <iostream>
#include "event_handler.h"

EventHandler* EventHandler::instance = new EventHandler();

Event::Event(Time t, EventType eType, void* pt)
{
	time = t;
	type = eType;
	ptr = pt;
}

Time Event::getTime() const
{
	return time;
}

EventType Event::getType() const
{
	return type;
}

std::string Event::getEventName() const
{
	switch (type)
	{
		case NEW_REQ : return "NEW_REQ"; break;
		case REQ_COMP : return "REQ_COMPLETE"; break;
		case REQ_TOUT : return "REQ_TIME_OUT"; break;
		case CTX_SWTCH : return "CONTEXT_SWITCH"; break;
	}
}

void* Event::getPtr() const
{
	return ptr;
}

bool Event::operator<(const Event& e) const
{
	return time > e.time;
}

void EventHandler::addNewReqEvent(Time t, User* user)
{
	eventList.push(Event(t, NEW_REQ, user));
}

void EventHandler::addReqCompEvent(Time t, Request* req)
{
	eventList.push(Event(t, REQ_COMP, req));
}

void EventHandler::addReqTimeOutEvent(Time t, Request* req)
{
	eventList.push(Event(t, REQ_TOUT, req));
}

void EventHandler::addContextSwitchEvent(Time t, Core* core)
{
	eventList.push(Event(t, CTX_SWTCH, core));
}

Event EventHandler::getNextEvent()
{
	if (eventList.size() == 0)
	{
		std::cerr<<"Event List Empty\n";
	}
	Event nextEvent = eventList.top();
	eventList.pop();
	return nextEvent;
}

EventHandler* EventHandler::getInstance()
{
	return instance;
}
