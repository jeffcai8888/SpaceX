#include "SocketServer.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>s

SocketServer* SocketServer::s_server = nullptr;

SocketServer* SocketServer::getInstance()
{
	if (s_server == nullptr)
	{
		s_server = new SocketServer;
	}

	return s_server;
}

void SocketServer::destroyInstance()
{
	CC_SAFE_DELETE(s_server);
}

SocketServer::SocketServer() :
	_socketServer(0),
	onRecv(nullptr),
	onStart(nullptr),
	onNewConnection(nullptr)
{

}

SocketServer::~SocketServer()
{
	this->clear();
};

void SocketServer::clear()
{
	if (_socketServer)
	{
		_mutex.lock();
		this->closeConnect(_socketServer);
		_mutex.unlock();
	}

	for (auto msg : _UIMessageQueue)
	{
		CC_SAFE_DELETE(msg);
	}
	_UIMessageQueue.clear();

	Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
}

bool SocketServer::startServer(unsigned short port)
{
	if (!initServer(port))
	{
		return false;
	}

	return true;
}

bool SocketServer::initServer(unsigned short port)
{
	if (_socketServer != 0)
	{
		this->closeConnect(_socketServer);
	}
	_socketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (error(_socketServer))
	{
		log("socket error!");
		_socketServer = 0;
		return false;
	}

	do 
	{
		struct sockaddr_in sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		_serverPort = port;		// save the port

		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(_serverPort);
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		int ret = 0;
		ret = bind(_socketServer, (const sockaddr*)&sockAddr, sizeof(sockAddr));
		if (ret < 0)
		{
			log("bind error!");
			break;
		}
	
		ret = listen(_socketServer, 5);
		if (ret < 0)
		{
			log("listen error!");
			break;
		}
		// start 
		char hostName[256];
		gethostname(hostName, sizeof(hostName));
        localIPAddresses();
		struct hostent* hostInfo = gethostbyname(hostName);
		char* ip = inet_ntoa(*(struct in_addr *)*hostInfo->h_addr_list);
		this->acceptClient();

		if (onStart != nullptr)
		{
			log("start server!");
			this->onStart(ip);
		}

		Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);

		return true;

	} while (false);
	
	this->closeConnect(_socketServer);
	_socketServer = 0;
	return false;
}

void SocketServer::acceptClient()
{
	std::thread th(&SocketServer::acceptFunc, this);
	th.detach();
}

void SocketServer::acceptFunc()
{
	int len = sizeof(sockaddr);
	struct sockaddr_in sockAddr;
	while (true)
	{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		HSocket clientSock = accept(_socketServer, (sockaddr*)&sockAddr, (int *)&len);
#else
		HSocket clientSock = accept(_socketServer, (sockaddr*)&sockAddr, (socklen_t *)&len);
#endif
		if (error(clientSock))
		{
			log("accept error!");
		}
		else
		{
			this->newClientConnected(clientSock);
		}
	}
}

void SocketServer::newClientConnected(HSocket socket)
{
	log("new connect!");

	_clientSockets.push_back(socket);
	std::thread th(&SocketServer::recvMessage, this, socket);
	th.detach();

	if (onNewConnection)
	{
		std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
		SocketMessage * msg = new SocketMessage(NEW_CONNECTION, (unsigned char*)&socket, sizeof(HSocket));
		_UIMessageQueue.push_back(msg);
	}
}

void SocketServer::recvMessage(HSocket socket)
{
	char buff[1024];
	int ret = 0;

	while (true)
	{
		ret = recv(socket, buff, sizeof(buff), 0);
		if (ret < 0)
		{
			log("recv(%d) error!", socket);
			break;
		}
		else
		{
			if (ret > 0 && onRecv != nullptr)
			{
				std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
				RecvData recvData;
				recvData.socketClient = socket;
				memcpy(recvData.data, buff, ret);
				recvData.dataLen = ret;
				SocketMessage * msg = new SocketMessage(RECEIVE, (unsigned char*)&recvData, sizeof(RecvData));
				_UIMessageQueue.push_back(msg);
			}
		}
	}

	_mutex.lock();
	this->closeConnect(socket);
	if (onDisconnect != nullptr)
	{
		std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
		SocketMessage * msg = new SocketMessage(DISCONNECT, (unsigned char*)&socket, sizeof(HSocket));
		_UIMessageQueue.push_back(msg);
	}
	_mutex.unlock();
}

void SocketServer::sendMessage(HSocket socket, const char* data, int count)
{
	for (auto& sock : _clientSockets)
	{
		if (sock == socket)
		{
			int ret = send(socket, data, count, 0);
			if (ret < 0)
			{
				log("send error!");
			}
			break;
		}
	}
}

void SocketServer::sendMessage(const char* data, int count)
{
	for (auto& socket : _clientSockets)
	{
		int ret = send(socket, data, count, 0);
		if (ret < 0)
		{
			log("send error!");
		}
	}
}

void SocketServer::update(float dt)
{
	if (_UIMessageQueue.size() == 0)
	{
		return;
	}

	_UIMessageQueueMutex.lock();

	if (_UIMessageQueue.size() == 0)
	{
		_UIMessageQueueMutex.unlock();
		return;
	}

	SocketMessage *msg = *(_UIMessageQueue.begin());
	_UIMessageQueue.pop_front();

	switch (msg->getMsgType())
	{
	case NEW_CONNECTION:
		if (onNewConnection)
		{
			this->onNewConnection(*(HSocket*)msg->getMsgData()->getBytes());
		}
		break;
	case DISCONNECT:
		if (onDisconnect)
		{
			this->onDisconnect(*(HSocket*)msg->getMsgData()->getBytes());
		}
		break;
	case RECEIVE:
		if (onRecv)
		{
			RecvData* recvData = (RecvData*)msg->getMsgData()->getBytes();
			this->onRecv(recvData->socketClient, (const char*)recvData->data, recvData->dataLen);
		}
		break;
	default:
		break;
	}

	CC_SAFE_DELETE(msg);
	_UIMessageQueueMutex.unlock();
}

std::string SocketServer::localIPAddresses()
{
    struct ifaddrs *allInterfaces;
    
    // Get list of all interfaces on the local machine:
    if (getifaddrs(&allInterfaces) == 0) {
        struct ifaddrs *interface;
        
        // For each interface ...
        for (interface = allInterfaces; interface != NULL; interface = interface->ifa_next) {
            unsigned int flags = interface->ifa_flags;
            struct sockaddr *addr = interface->ifa_addr;
            std::string name = std::string(interface->ifa_name);
            
            // Check for running IPv4, IPv6 interfaces. Skip the loopback interface.
            if ((flags & (IFF_UP|IFF_RUNNING|IFF_LOOPBACK)) == (IFF_UP|IFF_RUNNING)) {
                if (addr->sa_family == AF_INET || addr->sa_family == AF_INET6) {
                    
                    // Convert interface address to a human readable string:
                    if(name.compare("en0") == 0)
                    {
                        char host[NI_MAXHOST];
                        getnameinfo(addr, addr->sa_len, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
                        freeifaddrs(allInterfaces);
                        return std::string(host);
                    }
                    
                }
            }
        }
        
        freeifaddrs(allInterfaces);
    }
}