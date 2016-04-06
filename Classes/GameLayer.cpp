#include <math.h>
#include "Macro.h"
#include "GameLayer.h"
#include "Hero.h"
#include "Gunner.h"
#include "Princess.h"
#include "Bullet.h"
#include "Ground.h"
#include "Box.h"
#include "Slope.h"
#include "Foresight.h"
#include "OperateLayer.h"
#include "JsonParser.h"

#include "SceneManager.h"
#include "SimpleAudioEngine.h"
#include "SocketManager.h"

USING_NS_CC;

GameLayer::GameLayer()
	:m_pTiledMap(nullptr)
	,m_pHero(nullptr)
    ,m_pForesight(nullptr)
    ,m_pRange(nullptr)
{
	m_vecBullets.clear();
	m_vecEventListener.clear();
	for (int i = 0; i < 3; ++i)
	{
		m_pEnemy[i] = nullptr;
	}
}

GameLayer::~GameLayer()
{
	
}

bool GameLayer::init()
{
	bool ret = false;
	do {
		CC_BREAK_IF( !Layer::init());


		//CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("bgm.mp3", true);
		//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(PATH_HERO_TALK_EFFECT);
		ret = true;
	} while(0);

	return ret;
}

void GameLayer::onEnter()
{
	Layer::onEnter();

	m_pTiledMap = TMXTiledMap::create("TYCHEs_COFEE.tmx");
	m_TiledMapSize.setSize(m_pTiledMap->getMapSize().width * m_pTiledMap->getTileSize().width, m_pTiledMap->getMapSize().height * m_pTiledMap->getTileSize().height);
	this->addChild(m_pTiledMap);

	const PhysicsMaterial m(1.f, 0.f, 0.f);
	Size boxSize(m_pTiledMap->getMapSize().width * m_pTiledMap->getTileSize().width, m_pTiledMap->getMapSize().height * m_pTiledMap->getTileSize().height);
	auto body = PhysicsBody::createEdgeBox(boxSize, m, 3);
	body->setCategoryBitmask(0x04);
	body->setContactTestBitmask(0x02);
	body->setCollisionBitmask(0x03);
	auto edgeNode = Node::create();
	edgeNode->setPosition(Point(boxSize.width / 2, boxSize.height / 2));
	edgeNode->setPhysicsBody(body);
	this->addChild(edgeNode);

	importGroundData(m_pTiledMap);

	auto listener = EventListenerCustom::create("bullet_disappear", [this](EventCustom* event) {
		Bullet* bullet = static_cast<Bullet *>(event->getUserData());
		if (bullet)
			this->removeChild(bullet);
	});

	_eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
	m_vecEventListener.pushBack(listener);


	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = [this](PhysicsContact& contact)->bool
	{
		if ((contact.getShapeA()->getCategoryBitmask() == PC_Hero && contact.getShapeB()->getCategoryBitmask() == PC_Ground)
			|| (contact.getShapeA()->getCategoryBitmask() == PC_Ground && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Hero))
		{
			BaseSprite* hero;
			Ground* ground;
			if (contact.getShapeA()->getCategoryBitmask() == PC_Hero)
			{
				hero = static_cast<BaseSprite *>(contact.getShapeA()->getBody()->getNode());
				ground = static_cast<Ground *>(contact.getShapeB()->getBody()->getNode());
			}
			else
			{
				hero = static_cast<BaseSprite *>(contact.getShapeB()->getBody()->getNode());
				ground = static_cast<Ground *>(contact.getShapeA()->getBody()->getNode());
			}
	
			if (hero->getCurrActionState() == ACTION_STATE_MOVE && hero->isInMoveAction(MOVE_STATE_DOWN))
			{
				if (hero->isInMoveAction(MOVE_STATE_WALK))
				{
					hero->stopMoveAction(MOVE_STATE_DOWN, true);
					Vec2 v = hero->getPhysicsBody()->getVelocity();
					hero->walk(v.x);
					SocketManager::getInstance()->sendData(NDT_HeroWalk, hero->getCurrActionState(), hero->getCurrMoveState(), hero->getPosition(), hero->getPhysicsBody()->getVelocity());
				}
				else
				{
					hero->stopMoveAction(MOVE_STATE_DOWN, true);
					hero->stop();
					SocketManager::getInstance()->sendData(NDT_HeroStop, hero->getCurrActionState(), hero->getCurrMoveState(), hero->getPosition(), Vec2(0, 0));
				}
				hero->setJumpStage(0);
				return true;
			}
			else if (hero->getCurrActionState() == ACTION_STATE_MOVE && hero->isInMoveAction(MOVE_STATE_UP))
			{
				return false;
			}
		}
		else if ((contact.getShapeA()->getCategoryBitmask() == PC_Hero && contact.getShapeB()->getCategoryBitmask() == PC_Box)
			|| (contact.getShapeA()->getCategoryBitmask() == PC_Box && contact.getShapeB()->getCategoryBitmask() == PC_Hero))
		{
			BaseSprite* hero;
			Box* box;
			if (contact.getShapeA()->getCategoryBitmask() == PC_Hero)
			{
				hero = static_cast<BaseSprite *>(contact.getShapeA()->getBody()->getNode());
				box = static_cast<Box *>(contact.getShapeB()->getBody()->getNode());
			}
			else
			{
				hero = static_cast<BaseSprite *>(contact.getShapeB()->getBody()->getNode());
				box = static_cast<Box *>(contact.getShapeA()->getBody()->getNode());
			}

			if (hero->getCurrActionState() == ACTION_STATE_MOVE && hero->isInMoveAction(MOVE_STATE_DOWN))
			{
				if (hero->isInMoveAction(MOVE_STATE_WALK))
				{
					hero->stopMoveAction(MOVE_STATE_DOWN, true);
					Vec2 v = hero->getPhysicsBody()->getVelocity();
					hero->walk(v.x);
					if(hero == this->m_pHero)
						SocketManager::getInstance()->sendData(NDT_HeroWalk, hero->getCurrActionState(), hero->getCurrMoveState(), hero->getPosition(), hero->getPhysicsBody()->getVelocity());
				}
				else
				{
					hero->stopMoveAction(MOVE_STATE_DOWN, true);
					hero->stop();
					if (hero == this->m_pHero)
						SocketManager::getInstance()->sendData(NDT_HeroStop, hero->getCurrActionState(), hero->getCurrMoveState(), hero->getPosition(), Vec2(0, 0));
				}
			}
			hero->setJumpStage(0);
			return true;
		}
		else if ((contact.getShapeA()->getCategoryBitmask() == PC_Hero && contact.getShapeB()->getCategoryBitmask() == PC_Slope)
			|| (contact.getShapeA()->getCategoryBitmask() == PC_Slope && contact.getShapeB()->getCategoryBitmask() == PC_Hero))
		{
			BaseSprite* hero;
			Slope* slope;
			if (contact.getShapeA()->getCategoryBitmask() == PC_Hero)
			{
				hero = static_cast<BaseSprite *>(contact.getShapeA()->getBody()->getNode());
				slope = static_cast<Slope *>(contact.getShapeB()->getBody()->getNode());
			}
			else
			{
				hero = static_cast<BaseSprite *>(contact.getShapeB()->getBody()->getNode());
				slope = static_cast<Slope *>(contact.getShapeA()->getBody()->getNode());
			}

			if (hero->getCurrActionState() == ACTION_STATE_MOVE && hero->isInMoveAction(MOVE_STATE_DOWN))
			{
				if (hero->isInMoveAction(MOVE_STATE_WALK))
				{
					hero->stopMoveAction(MOVE_STATE_DOWN, true);
					Vec2 v = hero->getPhysicsBody()->getVelocity();
					hero->walk(v.x);
					if (hero == this->m_pHero)
						SocketManager::getInstance()->sendData(NDT_HeroWalk, hero->getCurrActionState(), hero->getCurrMoveState(), hero->getPosition(), hero->getPhysicsBody()->getVelocity());
				}
				else
				{
					hero->stopMoveAction(MOVE_STATE_DOWN, true);
					hero->stop();
					if (hero == this->m_pHero)
						SocketManager::getInstance()->sendData(NDT_HeroStop, hero->getCurrActionState(), hero->getCurrMoveState(), hero->getPosition(), Vec2(0, 0));
				}
				hero->setJumpStage(0);
			}
			hero->setIsOnRotateGround(true);
			return true;
		}
        else if((contact.getShapeA()->getCategoryBitmask() == PC_Bullet && contact.getShapeB()->getCategoryBitmask() == PC_Ground) ||
			(contact.getShapeA()->getCategoryBitmask() == PC_Ground && contact.getShapeB()->getCategoryBitmask() == PC_Bullet))
        {
			Bullet* bullet;
			if(contact.getShapeA()->getCategoryBitmask() == PC_Bullet)
				bullet = static_cast<Bullet *>(contact.getShapeA()->getBody()->getNode());
			else
				bullet = static_cast<Bullet *>(contact.getShapeB()->getBody()->getNode());
            if(bullet)
            {
                bullet->setIsActive(false);
				this->removeChild(bullet);
            }
			return true;
        }
		else if ((contact.getShapeA()->getCategoryBitmask() == PC_Bullet && contact.getShapeB()->getCategoryBitmask() == PC_Box) ||
			(contact.getShapeA()->getCategoryBitmask() == PC_Box && contact.getShapeB()->getCategoryBitmask() == PC_Bullet))
		{
			Bullet* bullet;
			if (contact.getShapeA()->getCategoryBitmask() == PC_Bullet)
				bullet = static_cast<Bullet *>(contact.getShapeA()->getBody()->getNode());
			else
				bullet = static_cast<Bullet *>(contact.getShapeB()->getBody()->getNode());
			if (bullet)
			{
				bullet->setIsActive(false);
				this->removeChild(bullet);
			}
			return true;
		}
		else if ((contact.getShapeA()->getCategoryBitmask() == PC_Bullet && contact.getShapeB()->getCategoryBitmask() == PC_Slope) ||
			(contact.getShapeA()->getCategoryBitmask() == PC_Slope && contact.getShapeB()->getCategoryBitmask() == PC_Bullet))
		{
			Bullet* bullet;
			if (contact.getShapeA()->getCategoryBitmask() == PC_Bullet)
				bullet = static_cast<Bullet *>(contact.getShapeA()->getBody()->getNode());
			else
				bullet = static_cast<Bullet *>(contact.getShapeB()->getBody()->getNode());
			if (bullet)
			{
				bullet->setIsActive(false);
				this->removeChild(bullet);
			}
			return true;
		}
		else if ((contact.getShapeA()->getCategoryBitmask() == PC_Bullet && contact.getShapeB()->getCategoryBitmask() == PC_Damage) ||
			(contact.getShapeA()->getCategoryBitmask() == PC_Damage && contact.getShapeB()->getCategoryBitmask() == PC_Bullet))
		{
			Bullet* bullet;
			BaseSprite* hero;
			if (contact.getShapeA()->getCategoryBitmask() == PC_Bullet)
			{
				bullet = static_cast<Bullet *>(contact.getShapeA()->getBody()->getNode());
				hero = static_cast<BaseSprite *>(contact.getShapeB()->getBody()->getNode());
			}				
			else
			{
				bullet = static_cast<Bullet *>(contact.getShapeB()->getBody()->getNode());
				hero = static_cast<BaseSprite *>(contact.getShapeA()->getBody()->getNode());
			}
				
			if (bullet->getOwnerTag() == hero->getTag())
			{
				return false;
			}
			else
			{
				bullet->setIsActive(false);
				this->removeChild(bullet);
				hero->hurt(hero->getBullletPower());
				return true;
			}		
		}
		return true;
	};

	contactListener->onContactSeparate = [this](PhysicsContact& contact)
	{
		if ((contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Hero && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Slope)
			|| (contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Slope && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Hero))
		{
			BaseSprite* hero;
			Slope* slope;
			if (contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Hero)
			{
				hero = static_cast<BaseSprite *>(contact.getShapeA()->getBody()->getNode());
				slope = static_cast<Slope *>(contact.getShapeB()->getBody()->getNode());
			}
			else
			{
				hero = static_cast<BaseSprite *>(contact.getShapeB()->getBody()->getNode());
				slope = static_cast<Slope *>(contact.getShapeA()->getBody()->getNode());
			}

			hero->setIsOnRotateGround(false);

			if ((hero->getCurrActionState() == ACTION_STATE_MOVE && hero->isInMoveAction(MOVE_STATE_WALK)) || hero->getCurrActionState() == ACTION_STATE_IDLE)
			{
				hero->stopMoveAction(MOVE_STATE_WALK, false);
				hero->runJumpAction(false);
			}
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
	m_vecEventListener.pushBack(contactListener);

	this->getScene()->getPhysicsWorld()->setAutoStep(false);

	this->scheduleUpdate();
}

void GameLayer::onExit()
{
	Layer::onExit();
	removeAllEventListener();	
	this->unscheduleUpdate();
	this->removeAllBullets();
	this->removeAllChildren();
	
}

void GameLayer::update(float dt)
{
	this->updateHero(dt);
	this->updateEnemys(dt);
	this->updateBullet(dt);
	this->updatePhysicsWorld(dt);
	this->updateForesight(dt);
}

void GameLayer::updateHero(float dt)
{
	setViewPointCenter();

	float x = m_pHero->getPhysicsBody()->getVelocity().x;
	float y = m_pHero->getPhysicsBody()->getVelocity().y;
	if (!m_pHero->getIsOnRotateGround())
		y += m_pHero->getGravity() * dt;
	m_pHero->getPhysicsBody()->setVelocity(Vec2(x, y));

	if (m_pHero->getCurrActionState() == ACTION_STATE_MOVE && m_pHero->isInMoveAction(MOVE_STATE_UP) && m_pHero->getPosition().y < m_pHero->getPrePosition().y)
	{
		m_pHero->runJumpAction(false);
	}

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
    
    if(m_pHero->getIsLocked())
    {
        BaseSprite* target = getNearestEnemy();
        Vec2 direction = target->getPosition() - m_pHero->getPosition();
        direction.normalize();
        m_pHero->setShootDirection(direction);
    }
	
    
	//CCLOG("MoveState %d %d", m_pHero->getCurrActionState(), m_pHero->getCurrMoveState());
	//CCLOG("(%f, %f) (%f, %f)", m_pHero->getPhysicsBody()->getPosition().x, m_pHero->getPhysicsBody()->getPosition().y, m_pHero->getPosition().x, m_pHero->getPosition().y);
}

void GameLayer::updateEnemys(float dt)
{
	for (int i = 0; i < 3; ++i)
	{
		if(m_pEnemy[i] == nullptr)
			continue;
		float x = m_pEnemy[i]->getPhysicsBody()->getVelocity().x;
		float y = m_pEnemy[i]->getPhysicsBody()->getVelocity().y;
		if (!m_pEnemy[i]->getIsOnRotateGround())
			y += m_pEnemy[i]->getGravity() * dt;
		m_pEnemy[i]->getPhysicsBody()->setVelocity(Vec2(x, y));

		if (m_pEnemy[i]->getCurrActionState() == ACTION_STATE_MOVE && m_pEnemy[i]->isInMoveAction(MOVE_STATE_UP) && m_pEnemy[i]->getPosition().y < m_pEnemy[i]->getPrePosition().y)
		{
			m_pEnemy[i]->runJumpAction(false);
		}

		if (m_pEnemy[i]->getIsAttacking())
		{
			m_shootTime += dt;
			if (m_shootTime >= m_pEnemy[i]->getBulletInterval())
			{
				Bullet* bullet = getUnusedBullet();
				bullet->launch(m_pEnemy[i]);
				this->addChild(bullet);
				m_shootTime = 0.f;
				if (m_pEnemy[i]->getShootDirection().x != 0)
					m_pEnemy[i]->setFlippedX(m_pEnemy[i]->getShootDirection().x < 0);

			}
		}
		//CCLOG("enemy[%d]:%f, %f", i, m_pEnemy[i]->getPositionX(), m_pEnemy[i]->getPositionY());
	}
	
}

void GameLayer::updateBullet(float dt)
{
	for (auto sp_obj : m_vecBullets)
	{
		if (sp_obj->getIsActive())
		{
			sp_obj->update(dt);
		}
	}
}

void GameLayer::updateForesight(float dt)
{
	m_pForesight->setPosition(m_pHero->getPosition() + Point(0.f, -20.f));
	float angle = CC_RADIANS_TO_DEGREES(m_pHero->getShootDirection().getAngle());
	m_pForesight->setRotation(-angle);
}

void GameLayer::updatePhysicsWorld(float dt)
{
	for (int i = 0; i < 3; ++i)
	{

		getScene()->getPhysicsWorld()->step(1 / 180.0f);

	}
}

Bullet* GameLayer::getUnusedBullet()
{
	for (auto sp_obj : m_vecBullets)
	{
		if (!sp_obj->getIsActive())
		{
			return sp_obj;
		}
	}

	auto bullet = Bullet::create();
	m_vecBullets.pushBack(bullet);
	return bullet;
}

void GameLayer::setViewPointCenter() {
	auto winSize = Director::getInstance()->getWinSize();
	auto centerOfView = Point(winSize.width / 2, winSize.height / 2);
	

	auto heroPos = m_pHero->getPosition();
	auto layerPos = this->getPosition();
	auto heroPosInScreen = heroPos + layerPos;
	this->setPositionX(centerOfView.x - heroPos.x);

    auto diff = heroPosInScreen - centerOfView;
    if(fabs(diff.y) > 25.f)
    {
		auto heroPrePosY = m_pHero->getPrePosition().y;
		this->setPositionY(heroPrePosY + layerPos.y - heroPos.y);
    }
	
}

void GameLayer::importGroundData(cocos2d::TMXTiledMap* data)
{
	TMXObjectGroup *objects = m_pTiledMap->getObjectGroup("Grounds");
	const ValueVector _objects = objects->getObjects();
	if (!_objects.empty())
	{
		for (const auto& v : _objects)
		{
			const ValueMap& dict = v.asValueMap();
			if (dict.find("name") != dict.end())
			{
				if (dict.at("name").asString() == "Ground")
				{
					Size boxSize(dict.at("width").asFloat(), dict.at("height").asFloat());
					auto ground = Ground::create();
					if(dict.find("rotation") != dict.end())
						ground->initPhysics(boxSize, Point(dict.at("x").asFloat(), dict.at("y").asFloat()), dict.at("rotation").asInt());
					else
						ground->initPhysics(boxSize, Point(dict.at("x").asFloat(), dict.at("y").asFloat()), 0);
					this->addChild(ground);
				}
				else if (dict.at("name").asString() == "Box")
				{
					Size boxSize(dict.at("width").asFloat(), dict.at("height").asFloat());
					auto box = Box::create();
					if (dict.find("rotation") != dict.end())
						box->initPhysics(boxSize, Point(dict.at("x").asFloat(), dict.at("y").asFloat()), dict.at("rotation").asInt());
					else
						box->initPhysics(boxSize, Point(dict.at("x").asFloat(), dict.at("y").asFloat()), 0);
					this->addChild(box);
				}
				else if (dict.at("name").asString() == "Slope")
				{
					Size boxSize(dict.at("width").asFloat(), dict.at("height").asFloat());
					auto slope = Slope::create();
					if (dict.find("rotation") != dict.end())
						slope->initPhysics(boxSize, Point(dict.at("x").asFloat(), dict.at("y").asFloat()), dict.at("rotation").asInt());
					else
						slope->initPhysics(boxSize, Point(dict.at("x").asFloat(), dict.at("y").asFloat()), 0);
					this->addChild(slope);
				}
			}
		}
	}
}

void GameLayer::removeAllEventListener()
{
	for (auto sp_obj : m_vecEventListener)
	{
		_eventDispatcher->removeEventListener(sp_obj);
	}
	m_vecEventListener.clear();
}

void GameLayer::removeAllBullets()
{
	for (auto sp_obj : m_vecBullets)
	{
		sp_obj->setIsActive(false);
		sp_obj->removeFromParent();
	}
}

BaseSprite* GameLayer::createHero(int role, cocos2d::Point pos)
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	BaseSprite* sprite;
	if(role == ROLE_HERO)
		sprite = Hero::create();
	else if(role == ROLE_GUN)
		sprite = Gunner::create();
	else
		sprite = Princess::create();
	sprite->setInitPos(pos);
	sprite->setScale(0.5f);
	sprite->setPosition(pos);
	sprite->runIdleAction();
	sprite->setLocalZOrder(visibleSize.height - sprite->getPositionY());
	sprite->setHP(100);
	sprite->setIsAttacking(false);
	sprite->setJumpStage(0);
	return sprite;
}

BaseSprite*  GameLayer::createEnemy(int role, cocos2d::Point pos)
{
	auto sprite = createHero(role, pos);
	ProgressTimer* blood = ProgressTimer::create(Sprite::create("blood.png"));
	blood->setName("blood");
	blood->setType(ProgressTimer::Type::BAR);
	blood->setMidpoint(Point(0, 0));
	blood->setBarChangeRate(Point(1, 0));
	blood->setAnchorPoint(Point(0, 1));
	blood->setPosition(50, 150);
	blood->setPercentage(100);


	ProgressTimer *bloodBg = ProgressTimer::create(Sprite::create("bloodBg.png"));
	bloodBg->setType(ProgressTimer::Type::BAR);
	bloodBg->setMidpoint(Point(0, 0));
	bloodBg->setBarChangeRate(Point(1, 0));
	bloodBg->setAnchorPoint(Point(0, 1));
	bloodBg->setPosition(blood->getPosition());
	bloodBg->setPercentage(100);

	sprite->addChild(bloodBg);
	sprite->addChild(blood);

	return sprite;
}

BaseSprite* GameLayer::getNearestEnemy()
{
	float distance = m_pHero->getPosition().getDistanceSq(m_pEnemy[0]->getPosition());
    int index = 0;
	for (int i = 1; i < 3; ++i)
	{
        if(m_pEnemy[i] == nullptr)
            continue;
		float d = m_pHero->getPosition().getDistanceSq(m_pEnemy[i]->getPosition());
		if (d < distance)
		{
			distance = d;
            index = i;
        }
	}
	return m_pEnemy[index];
}
