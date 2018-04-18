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
	// TODO(Distribution)
	if (issuedReq != nullptr)
	{
		issuedReq->setStatus(BAD);
	}
	status = THINK;
	Time thinkTime = 1.0;
	EventHandler::getInstance()->addNewReqEvent(t + thinkTime, this);
}

void User::setIssuedReq(Request* req)
{
	issuedReq = req;
	status = WAIT;
}
