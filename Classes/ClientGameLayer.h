#ifndef _SPACEX_CLASSES_CLIENTGAMELAYER_H_
#define _SPACEX_CLASSES_CLIENTGAMELAYER_H_

#include <cocos2d.h>
#include "GameLayer.h"


class BaseSprite;
class Hero;
class Bullet;

class ClientGameLayer : public GameLayer
{
public:
	ClientGameLayer();
	virtual ~ClientGameLayer();

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	void onRecv(const char* data, int count);
	void onDisconnect();

	virtual void update(float dt);
	virtual void updateHero(float dt);
	virtual void updateBullet(float dt);

	CREATE_FUNC(ClientGameLayer);

protected:
	std::deque<cocos2d::Point> m_dequeShadow;
};

#endif