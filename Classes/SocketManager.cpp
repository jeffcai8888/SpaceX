#include "SocketManager.h"
#include "Macro.h"
#include "GameLayer.h"

SocketManager* SocketManager::instance = nullptr;

SocketManager::SocketManager()
:m_networkType(NT_Offline)
, m_pSocketClient(nullptr)
, m_pSocketServer(nullptr)
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
		m_pSocketClient = SocketClient::getInstance();
		if (!m_pSocketClient->connectServer(m_ServerAddr.c_str(), 8000))
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
	if(m_networkType == NT_Server)
		m_pSocketServer->sendMessage((const char*)&data, sizeof(data));
	else if (m_networkType == NT_Client)
	{
		m_pSocketClient->sendMessage((const char*)&data, sizeof(data));
	}
		
}

std::string SocketManager::getIPAddress()
{
	return m_pSocketServer->localIPAddresses();
}
