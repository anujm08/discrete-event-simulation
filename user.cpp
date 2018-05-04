#include "user.h"
#include "event_handler.h"
#include "request.h"
#include "time_distribution.h"

User::User(int userID)
{
	id = userID;
	status = THINK;
	issuedReq = nullptr;
}

int User::getID() const
{
	return id;
}

Time User::startThinking(Time t)
{
	status = THINK;
	Time thinkTime = TimeDistribution::getThinkTime();
	EventHandler::getInstance()->addNewReqEvent(t + thinkTime, this);
}

Request* User::issueRequest(Time t)
{
	issuedReq = new Request(this, t);
	Time timeout = TimeDistribution::getTimeOutTime();
	EventHandler::getInstance()->addReqTimeOutEvent(t + timeout, issuedReq);
	status = WAIT;

	return issuedReq;
}
