#include <math.h>
#include "Macro.h"
#include "GameLayer.h"
#include "Hero.h"
#include "Gunner.h"
#include "Princess.h"
#include "Bullet.h"
#include "Ground.h"
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
{
	m_vecBullets.clear();
	m_vecEventListener.clear();
}

GameLayer::~GameLayer()
{
	
}

bool GameLayer::init()
{
	bool ret = false;
	do {
		CC_BREAK_IF( !Layer::init());


		//CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(PATH_BG_MUSIC, true);
		//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(PATH_HERO_TALK_EFFECT);
		ret = true;
	} while(0);

	return ret;
}

void GameLayer::onEnter()
{
	Layer::onEnter();

	auto visibleSize = Director::getInstance()->getVisibleSize();
	this->m_origin = Director::getInstance()->getVisibleOrigin();



	

	m_pTiledMap = TMXTiledMap::create("TYCHEs_COFEE.tmx");
	m_TiledMapSize.setSize(m_pTiledMap->getMapSize().width * m_pTiledMap->getTileSize().width, m_pTiledMap->getMapSize().height * m_pTiledMap->getTileSize().height);
	this->addChild(m_pTiledMap);

	

	TMXObjectGroup *objects = m_pTiledMap->getObjectGroup("Objects");
	CCASSERT(NULL != objects, "'Objects' object group not found");

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("hero.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("gunner.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("princess.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui.plist");

	
	auto spawnPoint = objects->getObject("SpawnPoint");
	CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
	Point heroInitPos = m_origin + Point(spawnPoint["x"].asFloat(), spawnPoint["y"].asFloat());
	m_pHero = Princess::create();
	m_pHero->setScale(0.5f);
	m_pHero->setPosition(heroInitPos);
	m_pHero->runIdleAction();
	m_pHero->setLocalZOrder(visibleSize.height - m_pHero->getPositionY());
	m_pHero->setHP(100);
	m_pHero->setIsAttacking(false);
	m_pHero->setJumpStage(0);
	m_pHero->onDeadCallback = CC_CALLBACK_0(GameLayer::onHeroDead, this, m_pHero);
	m_pHero->attack = CC_CALLBACK_0(GameLayer::onHeroAttack, this);
	m_pHero->stop = CC_CALLBACK_0(GameLayer::onHeroStop, this);
	m_pHero->walk = CC_CALLBACK_1(GameLayer::onHeroWalk, this);
	m_pHero->jump = CC_CALLBACK_1(GameLayer::onHeroJump, this);
	this->addChild(m_pHero);
	auto centerOfView = Point(visibleSize.width / 2, visibleSize.height / 2);
	this->setPosition(centerOfView - m_pHero->getPosition());

	m_pForesight = Foresight::create();
	this->addChild(m_pForesight);

	JsonParser* parser = JsonParser::createWithFile("Debug.json");
	parser->decodeDebugData();
	auto list = parser->getList();
	for (auto& v : list)
	{
		ValueMap row = v.asValueMap();

		for (auto& pair : row)
		{
			CCLOG("%s %s", pair.first.c_str(), pair.second.asString().c_str());
			if (pair.first.compare("HeroHSpeed") == 0)
			{
				float s = pair.second.asFloat();
				m_pHero->setWalkVelocity(s);
			}
			else if (pair.first.compare("HeroVSpeed") == 0)
			{
				m_pHero->setJumpVelocity(pair.second.asFloat());
			}
			else if (pair.first.compare("HeroG") == 0)
			{
				m_pHero->setGravity(pair.second.asFloat());
			}
			else if (pair.first.compare("BulletPower") == 0)
			{
				m_pHero->setBullletPower(pair.second.asInt());
			}
			else if (pair.first.compare("BulletSpeed") == 0)
			{
				m_pHero->setBulletLaunchVelocity(pair.second.asFloat());
			}
			else if (pair.first.compare("BulletDisappearTime") == 0)
			{
				m_pHero->setBulletDisappearTime(pair.second.asFloat());
			}
			else if (pair.first.compare("BulletAngle") == 0)
			{
				m_pHero->setBullletAngle(pair.second.asInt());
			}
			else if (pair.first.compare("BulletInterval") == 0)
			{
				m_pHero->setBulletInterval(pair.second.asFloat());
			}
			else if (pair.first.compare("BulletG") == 0)
			{
				m_pHero->setBulletGravity(pair.second.asFloat());
			}
			else if (pair.first.compare("ForesightSpeed") == 0)
			{
				initForesight(pair.second.asFloat());
			}
			else if (pair.first.compare("AmmoCapacity") == 0)
			{
				m_pHero->setMaxAmmoCapacity(pair.second.asInt());
				m_pHero->setAmmoCapacity(pair.second.asInt());
			}
		}
	}

	const PhysicsMaterial m(1.f, 0.f, 0.f);
	Size boxSize(m_pTiledMap->getMapSize().width * m_pTiledMap->getTileSize().width, m_pTiledMap->getMapSize().height * m_pTiledMap->getTileSize().height);
	auto body = PhysicsBody::createEdgeBox(boxSize, m, 3);
	body->setTag(0);
	body->setCategoryBitmask(0x04);
	body->setContactTestBitmask(0x02);
	body->setCollisionBitmask(0x03);
	auto edgeNode = Node::create();
	edgeNode->setPosition(Point(boxSize.width / 2, boxSize.height / 2));
	edgeNode->setPhysicsBody(body);
	this->addChild(edgeNode);

	importGroundData(m_pTiledMap);

	m_shootTime = m_pHero->getBulletInterval();


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
		if ((contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Hero && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Ground)
			|| (contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Ground && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Hero))
		{
			BaseSprite* hero;
			Ground* ground;
			if (contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Hero)
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
				}
				else
				{
					hero->stop();
				}
				hero->setJumpStage(0);
				if (ground->getRotation() > 0 || ground->getRotation() < 0)
				{
					hero->setIsOnRotateGround(true);
				}
				return true;
			}
			else if (hero->getCurrActionState() == ACTION_STATE_MOVE && hero->isInMoveAction(MOVE_STATE_UP))
			{
				return false;
			}
			else
			{
				if (ground->getRotation() > 0 || ground->getRotation() < 0)
				{
					hero->setIsOnRotateGround(true);
				}
			}	
		}
        else if((contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Bullet && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Ground) ||
			(contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Ground && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Bullet))
        {
			Bullet* bullet;
			if(contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Bullet)		
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
		return true;
	};

	contactListener->onContactSeparate = [this](PhysicsContact& contact)
	{
		if ((contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Hero && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Ground)
			|| (contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Ground && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Hero))
		{
			BaseSprite* hero;
			Ground* ground;
			if (contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Hero)
			{
				hero = static_cast<BaseSprite *>(contact.getShapeA()->getBody()->getNode());
				ground = static_cast<Ground *>(contact.getShapeB()->getBody()->getNode());
			}
			else
			{
				hero = static_cast<BaseSprite *>(contact.getShapeB()->getBody()->getNode());
				ground = static_cast<Ground *>(contact.getShapeA()->getBody()->getNode());
			}
			if (ground->getRotation() > 0 || ground->getRotation() < 0)
			{
				hero->setIsOnRotateGround(false);
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


void GameLayer::onHeroWalk(float horizontalVelocity)
{
	if(m_pHero->isLive())
	{
		bool isWalking = m_pHero->getIsWalk();
		m_pHero->runWalkAction(!m_pHero->isInAir() && !isWalking);
		if (!m_pHero->getIsAttacking() && horizontalVelocity != 0)
			m_pHero->setFlippedX(horizontalVelocity < 0);
        
        Vec2 velocity = m_pHero->getPhysicsBody()->getVelocity();
        velocity.x = horizontalVelocity;
        m_pHero->getPhysicsBody()->setVelocity(velocity);
	}
}

void GameLayer::onHeroJump(float verticalVelocity)
{
	if (m_pHero->isLive() && m_pHero->getJumpStage() < 2)
	{ 
		m_pHero->runJumpAction(true);
		//m_pHero->setJumpStage(m_pHero->getJumpStage() + 1);
        
		Vec2 velocity = m_pHero->getPhysicsBody()->getVelocity();
        velocity.y = verticalVelocity;
		if (!m_pHero->getIsAttacking() && velocity.x != 0)
		{
			m_pHero->setFlippedX(velocity.x < 0);
		}
		m_pHero->getPhysicsBody()->setVelocity(velocity);
		m_pHero->setPrePosition(m_pHero->getPosition());
	}
}

void GameLayer::onHeroAttack()
{
	if (m_pHero->isLive())
	{
		m_pHero->runAttackAction();
	}
}

void GameLayer::onHeroStop()
{
	if(m_pHero->isLive())
	{
		m_pHero->runIdleAction();
        m_pHero->getPhysicsBody()->setVelocity(Vec2(0.f, 0.f));
	}
}

void GameLayer::onHeroDead(BaseSprite *pTarget)
{
	if(m_pHero->getCurrActionState() == ACTION_STATE_DEAD)
	{
		pTarget->removeSprite();
		SceneManager::getInstance()->showScene(GAME_OVER_SCENE);
	}
}

void GameLayer::update(float dt)
{
	this->updateHero(dt);
	this->updateBullet(dt);
	this->updatePhysicsWorld(dt);
	this->updateForesight(dt);
}

void GameLayer::updateHero(float dt)
{
	setViewPointCenter();

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
			//int curAmmoCapacity = m_pHero->getAmmoCapacity();
			//if(curAmmoCapacity == 0)



			Bullet* bullet = getUnusedBullet();
			bullet->launch(m_pHero);
			this->addChild(bullet);
			m_shootTime = 0.f;
			if (m_pHero->getShootDirection().x != 0)
				m_pHero->setFlippedX(m_pHero->getShootDirection().x < 0);
			
		}
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
	auto visibleSize = Director::getInstance()->getVisibleSize();
	m_pForesight->update(dt);
	if (m_pForesight->getPositionX() + getPositionX() > visibleSize.width)
	{
		m_pForesight->setPositionX(visibleSize.width - getPositionX());
	}
	else if (m_pForesight->getPositionX() + getPositionX() < 0)
	{
		m_pForesight->setPositionX(-getPositionX());
	}
	if (m_pForesight->getPositionY() + getPositionY() > visibleSize.height)
	{
		m_pForesight->setPositionY(visibleSize.height - getPositionY());
	}
	else if(m_pForesight->getPositionY() + getPositionY() < 0)
	{
		m_pForesight->setPositionY(-getPositionY());
	}
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
				if (dict.at("name").asString() == "Ground" || dict.at("name").asString() == "Box")
				{
					Size boxSize(dict.at("width").asFloat(), dict.at("height").asFloat());
					auto ground = Ground::create();
					if(dict.find("rotation") != dict.end())
						ground->initPhysics(boxSize, Point(dict.at("x").asFloat(), dict.at("y").asFloat()), dict.at("rotation").asInt());
					else
						ground->initPhysics(boxSize, Point(dict.at("x").asFloat(), dict.at("y").asFloat()), 0);
					this->addChild(ground);
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

void GameLayer::initForesight(float vel)
{
	if (m_pHero->isFlippedX())
	{
		m_pForesight->setPosition(m_pHero->getPosition()  + Point(-1.f, 0.f) * 200);
	}
	else
	{
		m_pForesight->setPosition(m_pHero->getPosition()  + Point(1.f, 0.f) * 200);
	}

	m_pForesight->setVelocity(vel);
}

void GameLayer::removeAllBullets()
{
	for (auto sp_obj : m_vecBullets)
	{
		sp_obj->setIsActive(false);
		sp_obj->removeFromParent();
	}
}
