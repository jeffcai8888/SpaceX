#include <math.h>
#include "Macro.h"
#include "ServerGameLayer.h"
#include "Hero.h"
#include "Bullet.h"
#include "Ground.h"
#include "OperateLayer.h"
#include "JsonParser.h"

#include "SceneManager.h"
#include "SimpleAudioEngine.h"
#include "SocketManager.h"

USING_NS_CC;

ServerGameLayer::ServerGameLayer()
	:GameLayer()
{

}

ServerGameLayer::~ServerGameLayer()
{
	m_vecBullets.clear();
}

bool ServerGameLayer::init()
{
	bool ret = false;
	do {
		CC_BREAK_IF( !GameLayer::init());
		
		ret = true;
	} while(0);

	return ret;
}

void ServerGameLayer::onEnter()
{
	GameLayer::onEnter();
}

void ServerGameLayer::onExit()
{
	GameLayer::onExit();
}

void ServerGameLayer::update(float dt)
{
	this->updateHero(dt);
	this->updateBullet(dt);
	this->updatePhysicsWorld(dt);
	this->updateForesight(dt);
}
