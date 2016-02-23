#ifndef __SOCK_H__
#define __SOCK_H__

#ifdef WIN32
#include <WinSock2.h>
#define SOCK_TYPE SOCKET
#else
#define SOCK_TYPE int
#define INVALID_SOCKET -1
#endif

#define MyLog(...)

class Sock
{
public:
	Sock();
	virtual ~Sock();

	bool setNonBlock()
	{
		SOCK_TYPE fd = _fd;
#ifdef WIN32
		BOOL flg = TRUE;

		if (ioctlsocket(fd, FIONBIO, (unsigned long*)&flg) != 0)
		{
			return false;
		}
		return true;
#else
		int flags = fcntl(fd, F_GETFL, 0);
		flags |= O_NONBLOCK;
		return fcnl(fd, F_SETFL, flags) != -1;
#endif
	}

	void close()
	{
#ifdef WIN32
		closesocket(_fd);
#else
		::close(_fd);
#endif
	}

	void attach(SOCK_TYPE fd)
	{
		_fd = fd;
	}

	bool isValidSocket()
	{
		return _fd != INVALID_SOCKET;
	}

protected:
	SOCK_TYPE _fd;
};

class SockClient : public Sock
{
public:
	SockClient(unsigned short port = 0, const char* ip = NULL)
	{
		if (port == 0 && ip == NULL)
		{
			return;
		}

		connect(port, ip);
	}

	~SockClient() {}

	int connect(unsigned short port, const char* ip)
	{
		_fd = socket(AF_INET, SOCK_STREAM, 0);

		struct  sockaddr_in addr;
		
		addr.sin_family = AF_INET;

		addr.sin_port = htons(port);

		addr.sin_addr.s_addr = inet_addr(ip);

		int ret = ::connect(_fd, (struct sockaddr*)&addr, sizeof(addr));
		
		if (ret < 0)
		{
			MyLog("connect error errno=%d", errno);
			return -1;
		}
		return 0;
	}

	int recv(char *buf, int len)
	{
		return ::recv(_fd, buf, len, 0);
	}

	int send(const char* buf, int len)
	{
		return ::send(_fd, buf, len, 0);
	}
};

class SockServer : public Sock
{
public:
	SockServer(unsigned short port, const char* ip = NULL)
	{
		listen(port, ip);
	}

	~SockServer() {}

	SockClient* accept()
	{
		SOCK_TYPE fd = ::accept(_fd, NULL, NULL);

		if (fd != INVALID_SOCKET)
		{
			SockClient* ret = new SockClient;
			ret->attach(fd);
			return ret;
		}
		return NULL;
	}

protected:
	int listen(unsigned short port, const char* ip = NULL)
	{
		_fd = socket(AF_INET, SOCK_STREAM, 0);

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		if (ip == NULL)
		{
			addr.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			addr.sin_addr.s_addr = inet_addr(ip);
		}

		int ret = bind(_fd, (sockaddr *)&addr, sizeof(addr));

		if (ret < 0)
		{
			MyLog("bind error");
			return -1;
		}

		this->setNonBlock();

		::listen(_fd, 10);

		return 0;
	}
};
#endif

