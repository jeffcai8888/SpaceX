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
	NDT_HeroStop,
	NDT_HeroAttack,
	NDT_HeroStopAttack
};

struct NetworkData
{
	int dataSize;
	int dataType;
	int index;
	int actionState;
	cocos2d::Vec2 position;
	cocos2d::Vec2 vec;
};

class SocketManager
{
public:
	static SocketManager* getInstance();

	void init(int networkType);
	void start();
	void sendData(int, int, cocos2d::Vec2, cocos2d::Vec2);
	std::string getIPAddress();

	CC_SYNTHESIZE(int, m_networkType, NetworkType);
	CC_SYNTHESIZE(std::string, m_ServerAddr, ServerAddr);
private:
	SocketManager();
	virtual ~SocketManager();

	static SocketManager* instance;
};

#endif  //__SPACEX_CLASSES_SOCKETMANAGER_H__