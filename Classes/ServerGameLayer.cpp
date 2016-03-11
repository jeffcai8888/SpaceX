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
#include "Box2D/Box2D.h"

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
}

void ServerGameLayer::updateHero(float dt)
{
	setViewPointCenter();

	if (m_pHero->getCurrActionState() == ACTION_STATE_MOVE && m_pHero->isInMoveAction(MOVE_STATE_UP) && m_pHero->getPosition().y < m_pHero->getPrePosition().y)
	{
		m_pHero->runJumpAction(false);
	}

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

	Point diff = m_pHero->getPosition() - m_pHero->getPrePosition();
	if (fabs(diff.x) > EPSILON || fabs(diff.y) > EPSILON)
	{
		b2Vec2 v = m_pHero->getB2Body()->GetLinearVelocity();
		SocketManager::getInstance()->sendData(NDT_HeroPos, m_pHero->getPosition(), Vec2(v.x, v.y));
	}

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
