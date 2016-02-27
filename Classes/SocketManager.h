#ifndef __SPACEX_CLASSES_SOCKETMANAGER_H__
#define __SPACEX_CLASSES_SOCKETMANAGER_H__

#include "socket/SocketClient.h"
#include "socket/SocketServer.h"

enum NetworkType
{
	NT_Offline,
	NT_Server,
	NT_Client
};

enum NetworkDataType
{
	NDT_HeroPos,
	NDT_GroundPos,
	NDT_HeroWalk,
	NDT_HeroJumpUp,
	NDT_HeroJumpDown,
	NDT_HeroStop
};

struct NetworkData
{
	int dataSize;
	int dataType;
	cocos2d::Vec2 position;
};

class SocketManager
{
public:
	static SocketManager* getInstance();

	void init();
	void sendData(int, cocos2d::Vec2);

	CC_SYNTHESIZE(SocketClient*, m_pSocketClient, SocketClient);
	CC_SYNTHESIZE(SocketServer*, m_pSocketServer, SocketServer);
	CC_SYNTHESIZE(int, m_networkType, NetworkType);

private:
	SocketManager();
	virtual ~SocketManager();

	static SocketManager* instance;
};

#endif  //__SPACEX_CLASSES_SOCKETMANAGER_H__