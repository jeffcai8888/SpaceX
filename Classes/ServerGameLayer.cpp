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

void ServerGameLayer::updateHero(float dt)
{
	setViewPointCenter();

	/*float x = m_pHero->getPhysicsBody()->getVelocity().x;
	float y = m_pHero->getPhysicsBody()->getVelocity().y;
	if (!m_pHero->getIsOnRotateGround())
		y += m_pHero->getGravity() * dt;
	m_pHero->getPhysicsBody()->setVelocity(Vec2(x, y));*/

	if (m_pHero->getCurrActionState() == ACTION_STATE_MOVE && m_pHero->isInMoveAction(MOVE_STATE_UP) && m_pHero->getPosition().y < m_pHero->getPrePosition().y)
	{
		m_pHero->runJumpAction(false);
	}

	Point diff = m_pHero->getPosition() - m_pHero->getPrePosition();
	//CCLOG("aaa %f", diff.x / dt );
	//CCLOG("bbb %f", m_pHero->getPhysicsBody()->getVelocity().x * dt);

	m_pHero->setPrePosition(m_pHero->getPosition());
	if (m_pHero->getIsAttacking())
	{
		m_shootTime += dt;
		if (m_shootTime >= m_pHero->getBulletInterval())
		{
			Bullet* bullet = getUnusedBullet();
			bullet->launch(m_pHero);
			this->addChild(bullet);
			m_shootTime = 0.f;
			if (m_pHero->getShootDirection().x != 0)
				m_pHero->setFlippedX(m_pHero->getShootDirection().x < 0);
		}
	}

	/*if (fabs(diff.x) > EPSILON || fabs(diff.y) > EPSILON)
	{
		SocketManager::getInstance()->sendData(NDT_HeroPos, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
	}*/

	m_pHero->setPrePosition(m_pHero->getPosition());
}

void ServerGameLayer::updateBullet(float dt)
{
	for (auto sp_obj : m_vecBullets)
	{
		if (sp_obj->getIsActive())
		{
			sp_obj->update(dt);
		}
	}
}
