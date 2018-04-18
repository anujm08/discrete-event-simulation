#ifndef USER_H
#define USER_H

typedef double Time;
class Request;

enum UserStatus : bool
{
	WAIT = true,
	THINK = false,
};

class User
{
	// int ID;
	UserStatus status;
	Request* issuedReq;

public:
	User();

	Time startThinking(Time t);

	void setIssuedReq(Request* req);
};

#endif
