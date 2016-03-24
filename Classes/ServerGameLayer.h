#ifndef _SPACEX_CLASSES_SERVERGAMELAYER_H_
#define _SPACEX_CLASSES_SERVERGAMELAYER_H_

#include <cocos2d.h>
#include "GameLayer.h"

class BaseSprite;
class Hero;
class Bullet;
class GameData;

class ServerGameLayer : public GameLayer
{
public:
	ServerGameLayer();
	virtual ~ServerGameLayer();

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	virtual void update(float dt);

	CREATE_FUNC(ServerGameLayer);
};

#endif