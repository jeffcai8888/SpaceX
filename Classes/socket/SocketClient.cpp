#include "SocketClient.h"
#include "GameData.h"

SocketClient* SocketClient::getInstance()
{
	SocketClient* client = new SocketClient;
	return client;
}

void SocketClient::destroy()
{
	delete this;
}

SocketClient::SocketClient(void) :
	onRecv(nullptr),
	_socketClient(0),
	_hasRecvLoginProtocol(false)
{
}

SocketClient::~SocketClient(void)
{
	this->clear();
}

void SocketClient::clear()
{
	if (_socketClient != 0)
	{
		_mutex.lock();
		this->closeConnect(_socketClient);
		_mutex.unlock();
	}

	for (auto msg : _UIMessageQueue)
	{
		CC_SAFE_DELETE(msg);
	}
	_UIMessageQueue.clear();

	Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
}

bool SocketClient::initClient()
{
	this->clear();

	_socketClient = socket(AF_INET, SOCK_STREAM, 0);
	if (error(_socketClient))
	{
		log("init client error!");
		_socketClient = 0;
		return false;
	}
	
	Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
	return true;
}

bool SocketClient::connectServer(const char* serverIP, unsigned short port)
{
	if (!this->initClient())
	{
		return false;
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(struct sockaddr_in));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);

	int ret = 0;
	ret = connect(_socketClient, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));
	if (ret < 0)
	{
		_socketClient = 0;
		return false;
	}

	std::string loginData = std::string("{\"role_id\":\"") + Value(GameData::getInstance()->getRoleType()).asString() + std::string("\"}");
	sendMessage(loginData.c_str(), loginData.length());

	std::thread recvThread(&SocketClient::recvMessage, this);
	recvThread.detach();

	return true;
}

void SocketClient::recvMessage()
{
	char recvBuf[1024];
	int ret = 0;
	while (true)
	{
		ret = recv(_socketClient, recvBuf, sizeof(recvBuf), 0);
		CCLOG("recvMessage");
		if (ret < 0)
		{
			log("recv error!");
			break;
		}

		if (ret > 0 && ( onRecv != nullptr || (!_hasRecvLoginProtocol && onNewConnection != nullptr)))
		{
			std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
			SocketMessage * msg;
			
			
			if (!_hasRecvLoginProtocol)
			{
				msg = new SocketMessage(NEW_CONNECTION, (unsigned char*)recvBuf, ret);
				_hasRecvLoginProtocol = true;
			}					
			else
				msg = new SocketMessage(RECEIVE, (unsigned char*)recvBuf, ret);
			_UIMessageQueue.push_back(msg);
		}
	}
	_mutex.lock();
	this->closeConnect(_socketClient);
	if (onDisconnect != nullptr)
	{
		std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
		SocketMessage * msg = new SocketMessage(DISCONNECT);
		_UIMessageQueue.push_back(msg);
	}
	_socketClient = 0;
	_mutex.unlock();
}

void SocketClient::sendMessage(const char* data, int count)
{
	if (_socketClient != 0)
	{
		int ret = send(_socketClient, data, count, 0);
		if (ret < 0)
		{
			log("send error!");
		}
	}
}

void SocketClient::update(float dt)
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
	case DISCONNECT:
		if (onDisconnect)
		{
			this->onDisconnect();
		}
		break;
	case RECEIVE:
		if (onRecv)
		{
			this->onRecv((const char*)msg->getMsgData()->getBytes(), msg->getMsgData()->getSize());
		}
		break;
	case NEW_CONNECTION:
		if (onNewConnection)
		{
			this->onNewConnection((const char*)msg->getMsgData()->getBytes());
		}
		break;
	default:
		break;
	}

	CC_SAFE_DELETE(msg);
	_UIMessageQueueMutex.unlock();
}