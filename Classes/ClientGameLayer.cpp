#include <math.h>
#include "Macro.h"
#include "ClientGameLayer.h"
#include "Hero.h"
#include "Bullet.h"
#include "Ground.h"
#include "OperateLayer.h"
#include "JsonParser.h"

#include "SceneManager.h"
#include "SimpleAudioEngine.h"
#include "SocketManager.h"

USING_NS_CC;

ClientGameLayer::ClientGameLayer()
	:GameLayer()
{
	m_dequeShadow.clear();
}

ClientGameLayer::~ClientGameLayer()
{
	m_vecBullets.clear();
	m_dequeShadow.clear();
}

bool ClientGameLayer::init()
{
	bool ret = false;
	do {
		CC_BREAK_IF( !GameLayer::init());	
		ret = true;
	} while(0);

	return ret;
}

void ClientGameLayer::onEnter()
{
	GameLayer::onEnter();
	SocketManager::getInstance()->getSocketClient()->onRecv = CC_CALLBACK_2(ClientGameLayer::onRecv, this);
	SocketManager::getInstance()->getSocketClient()->onDisconnect = CC_CALLBACK_0(ClientGameLayer::onDisconnect, this);
}

void ClientGameLayer::onExit()
{
	GameLayer::onExit();
}

void ClientGameLayer::update(float dt)
{
	this->updateHero(dt);
	this->updateBullet(dt);
	this->updatePhysicsWorld(dt);
}

void ClientGameLayer::updateHero(float dt)
{
	setViewPointCenter();
	/*CCLOG("Hero %f, %f", m_pHero->getPositionX(), m_pHero->getPositionX());
	float x = m_pHero->getPhysicsBody()->getVelocity().x;
	float y = m_pHero->getPhysicsBody()->getVelocity().y;
	if (!m_pHero->getIsOnRotateGround())
		y += m_pHero->getGravity() * dt;
	m_pHero->getPhysicsBody()->setVelocity(Vec2(x, y));*/
}

void ClientGameLayer::updateBullet(float dt)
{
	
}

void ClientGameLayer::onRecv(const char* data, int count)
{
	NetworkData* networkData = (NetworkData*)data;
	if (networkData->dataSize == sizeof(NetworkData))
	{
		switch (networkData->dataType)
		{
		case NDT_HeroWalk:
			m_pHero->runWalkAction(!m_pHero->isInAir());
			//m_pHero->setPosition(networkData->position);
			m_pHero->getPhysicsBody()->setVelocity(networkData->velocity);
			break;
		case NDT_HeroJumpUp:
			m_pHero->runJumpAction(true);
			//m_pHero->setPosition(networkData->position);
			m_pHero->getPhysicsBody()->setVelocity(networkData->velocity);
			break;
		case NDT_HeroJumpDown:
			m_pHero->runJumpAction(false);
			//m_pHero->setPosition(networkData->position);
			m_pHero->getPhysicsBody()->setVelocity(networkData->velocity);
			break;
		case NDT_HeroStop:
			m_pHero->runIdleAction();
			//m_pHero->setPosition(networkData->position);
			m_pHero->getPhysicsBody()->setVelocity(networkData->velocity);
			break;
		case NDT_HeroPos:
			m_dequeShadow.push_back(networkData->position);
			break;

		default:
			break;
		}
	}
}

void ClientGameLayer::onDisconnect()
{
	CCLOG("Client disconnect");
}
