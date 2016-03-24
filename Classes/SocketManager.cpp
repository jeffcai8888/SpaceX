#include "SocketManager.h"
#include "Macro.h"
#include "GameLayer.h"

SocketManager* SocketManager::instance = nullptr;

SocketManager::SocketManager()
:m_pSocketClient(nullptr)
,m_pSocketServer(nullptr)
,m_networkType(NT_Offline)
{
}

SocketManager::~SocketManager()
{
}

SocketManager* SocketManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new SocketManager();
	}
	return instance;
}

void SocketManager::init()
{
	if (NT_Client == m_networkType)
	{
		m_pSocketClient = SocketClient::construct();
		if (!m_pSocketClient->connectServer("127.0.0.1", 8000))
		{
			CCLOG("Client connect error");
		}
	}
	else if (NT_Server == m_networkType)
	{
		m_pSocketServer = SocketServer::getInstance();
		m_pSocketServer->startServer(8000);
	}
}

void SocketManager::sendData(int type, int actionState, int moveState, cocos2d::Vec2 vec)
{
	NetworkData data;
	data.dataType = type;
	data.actionState = actionState;
	data.moveState = moveState;
	data.vec = vec;
	data.dataSize = sizeof(data);
	m_pSocketServer->sendMessage((const char*)&data, sizeof(data));
}
