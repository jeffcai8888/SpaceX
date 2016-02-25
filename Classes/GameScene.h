#ifndef _SPACEX_CLASSES_GAMESCENE_H_
#define _SPACEX_CLASSES_GAMESCENE_H_

#include <cocos2d.h>
#include "socket/SocketClient.h"
#include "socket/SocketServer.h"

using namespace cocos2d;
class GameScene
{
public:
	static Scene* createScene();
	
	CC_SYNTHESIZE(SocketClient* , m_pSocketClient, SocketClient);
	CC_SYNTHESIZE(SocketServer* , m_pSocketServer, SocketServer);

};


#endif