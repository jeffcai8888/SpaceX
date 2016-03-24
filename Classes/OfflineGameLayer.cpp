#include <math.h>
#include "Macro.h"
#include "OfflineGameLayer.h"
#include "Hero.h"
#include "Bullet.h"
#include "Ground.h"
#include "Foresight.h"
#include "OperateLayer.h"
#include "JsonParser.h"

#include "SceneManager.h"
#include "SimpleAudioEngine.h"
#include "SocketManager.h"

USING_NS_CC;

OfflineGameLayer::OfflineGameLayer()
	:GameLayer()
{
	
}

OfflineGameLayer::~OfflineGameLayer()
{
	
}

bool OfflineGameLayer::init()
{
	bool ret = false;
	do {
		CC_BREAK_IF( !GameLayer::init());
		ret = true;
	} while(0);

	return ret;
}

void OfflineGameLayer::onEnter()
{
	GameLayer::onEnter();
}

void OfflineGameLayer::onExit()
{
	GameLayer::onExit();
}

void OfflineGameLayer::update(float dt)
{
	this->updateHero(dt);
	this->updateBullet(dt);
	this->updatePhysicsWorld(dt);
	this->updateForesight(dt);
}
