#ifndef _SPACEX_CLASSES_SERVERGAMELAYER_H_
#define _SPACEX_CLASSES_SERVERGAMELAYER_H_

#include <cocos2d.h>
#include "GameLayer.h"
#include "socket/SocketBase.h"

class BaseSprite;
class Hero;
class Bullet;
class GameData;

class ServerGameLayer : public GameLayer
{
public:
	ServerGameLayer();
	virtual ~ServerGameLayer();

	virtual void onEnter();

	void onRecv(HSocket socket, const char* data, int count);
	void onDisconnect(HSocket socket);

	CREATE_FUNC(ServerGameLayer);

protected:
	std::deque<cocos2d::Point> m_dequeShadow;
};

#endif