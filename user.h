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
	int id;
	UserStatus status;
	Request* issuedReq;

public:
	User(int userID);

	int getID() const;
	Time startThinking(Time t);
	Request* issueRequest(Time t);
};

#endif
