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

void OfflineGameLayer::updateHero(float dt)
{
	setViewPointCenter();

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
	else
	{
		m_pForesight->setPosition(m_pForesight->getPosition() + diff);
	}
    
    /*if(m_pHero->getCurrActionState() == ACTION_STATE_MOVE)
    {
        CCLOG("ACTION_STATE_MOVE START");
        if(m_pHero->isInMoveAction(MOVE_STATE_UP))
        {
            CCLOG("MOVE_STATE_UP %f %f", m_pHero->getPhysicsBody()->getVelocity().x, m_pHero->getPhysicsBody()->getVelocity().y);
        }
        if(m_pHero->isInMoveAction(MOVE_STATE_DOWN))
        {
            CCLOG("MOVE_STATE_DOWN %f %f", m_pHero->getPhysicsBody()->getVelocity().x, m_pHero->getPhysicsBody()->getVelocity().y);
        }
        if(m_pHero->isInMoveAction(MOVE_STATE_WALK))
        {
            CCLOG("MOVE_STATE_WALK %f %f", m_pHero->getPhysicsBody()->getVelocity().x, m_pHero->getPhysicsBody()->getVelocity().y);
        }
        CCLOG("ACTION_STATE_MOVE END");
    }*/
	//CCLOG("(%f, %f) (%f, %f)", m_pHero->getPhysicsBody()->getPosition().x, m_pHero->getPhysicsBody()->getPosition().y, m_pHero->getPosition().x, m_pHero->getPosition().y);
}

void OfflineGameLayer::updateBullet(float dt)
{
	for (auto sp_obj : m_vecBullets)
	{
		if (sp_obj->getIsActive())
		{
			sp_obj->update(dt);
		}
	}
}
