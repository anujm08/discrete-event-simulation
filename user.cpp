#include "user.h"
#include "event_handler.h"
#include "request.h"

User::User()
{
	status = THINK;
	issuedReq = nullptr;
}

Time User::startThinking(Time t)
{
	// TODO(Distribution): Think Time
	if (issuedReq != nullptr)
	{
		issuedReq->setStatus(BAD);
	}
	status = THINK;
	Time thinkTime = 1.0;
	EventHandler::getInstance()->addNewReqEvent(t + thinkTime, this);
}

Request* User::issueRequest(Time t)
{
	issuedReq = new Request(this, t);
	//TODO(Distribution): Timeout
	Time timeout = 1.0;
	EventHandler::getInstance()->addReqTimeOutEvent(t + timeout, issuedReq);
	status = WAIT;

	return issuedReq;
}
