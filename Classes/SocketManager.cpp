#include "SocketManager.h"
#include "Macro.h"
#include "GameLayer.h"
#include "GameData.h"

SocketManager* SocketManager::instance = nullptr;

SocketManager::SocketManager()
:m_networkType(NT_Offline)
,m_pSocketClient(nullptr)
,m_pSocketServer(nullptr)
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

void SocketManager::init(int networkType)
{
	m_networkType = networkType;
	if (NT_Client == m_networkType)
	{
		m_pSocketClient = new SocketClient();
	}
	else if (NT_Server == m_networkType)
	{
		m_pSocketServer = new SocketServer();
	}
}

void SocketManager::start()
{
	if (NT_Client == m_networkType)
	{
		if (!m_pSocketClient->connectServer("127.0.0.1", 8000))
		{
			CCLOG("Client connect error");
		}
	}
	else if (NT_Server == m_networkType)
	{
		m_pSocketServer->startServer(8000);
	}
}

void SocketManager::sendData(int type, int index, int actionState, cocos2d::Vec2 position, cocos2d::Vec2 vec)
{
	NetworkData data;
	data.dataType = type;
    data.index = index;
	data.actionState = actionState;
	data.position = position;
	data.vec = vec;
	data.dataSize = sizeof(data);
	if(m_networkType == NT_Server)
		m_pSocketServer->sendMessage((const char*)&data, sizeof(data));
	else if (m_networkType == NT_Client)
		m_pSocketClient->sendMessage((const char*)&data, sizeof(data));
		
}

std::string SocketManager::getIPAddress()
{
	return m_pSocketServer->localIPAddresses();
}
