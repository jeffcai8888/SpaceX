#ifndef __SOCKET_CLIENT_H__
#define __SOCKET_CLIENT_H__
#include "SocketBase.h"

class SocketClient : public SocketBase
{
public:
	static SocketClient* getInstance();
	void destroy();

	bool connectServer(const char* serverIP, unsigned short port);
	void sendMessage(const char* data, int count);

	std::function<void(const char* data, int count)> onRecv;
	std::function<void(const char* data)> onNewConnection;
	std::function<void()> onDisconnect;

	void update(float dt);

	bool _hasRecvLoginProtocol;

CC_CONSTRUCTOR_ACCESS:
	SocketClient(void);
	~SocketClient(void);

private:
	bool initClient();
	void recvMessage();
	void clear();

private:
	HSocket _socketClient;
	std::list<SocketMessage*> _UIMessageQueue;
	std::mutex   _UIMessageQueueMutex;
	
};

#endif