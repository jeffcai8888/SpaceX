#include "Sock.h"



Sock::Sock()
	:_fd(INVALID_SOCKET)
{
#ifdef WIN32
	static bool winInit = false;
	if (!winInit)
	{
		winInit = true;
		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);
	}
#endif
}


Sock::~Sock()
{
	if (isValidSocket())
	{
		close();
	}
}
