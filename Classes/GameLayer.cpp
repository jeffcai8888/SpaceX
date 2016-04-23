#include <math.h>
#include "Macro.h"
#include "GameLayer.h"
#include "Hero.h"
#include "Gunner.h"
#include "Princess.h"
#include "Bullet.h"
#include "Bomb.h"
#include "Ground.h"
#include "Box.h"
#include "Slope.h"
#include "Foresight.h"
#include "OperateLayer.h"
#include "JsonParser.h"
#include "SceneManager.h"
#include "SimpleAudioEngine.h"
#include "SocketManager.h"
#include "ConfigCenter.h"
#include "GameData.h"

USING_NS_CC;

GameLayer::GameLayer()
	:m_pTiledMap(nullptr)
    ,m_pTarget(nullptr)
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


		//CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("bgm.mp3", true);
		//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(PATH_HERO_TALK_EFFECT);
		ret = true;
	} while(0);

	return ret;
}

void GameLayer::onEnter()
{
	Layer::onEnter();

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("hero.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("gunner.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("princess.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui.plist");

	ConfigCenter::getInstance()->reload();

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

	auto visibleSize = Director::getInstance()->getVisibleSize();
	this->m_origin = Director::getInstance()->getVisibleOrigin();

	TMXObjectGroup *objects = m_pTiledMap->getObjectGroup("Objects");
	CCASSERT(NULL != objects, "'Objects' object group not found");

	int playerNum = GameData::getInstance()->getPlayerNum();
	for (int i = 0; i < playerNum; ++i)
	{
		auto spawnPoint = objects->getObject("SpawnPoint" + Value(i).asString());
		CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
		Point heroInitPos = m_origin + Point(spawnPoint["x"].asFloat(), spawnPoint["y"].asFloat());
		int roleType = GameData::getInstance()->m_playerTypes[i];
		auto player = createHero(roleType, heroInitPos);
		player->setTag(i);
		if (GameData::getInstance()->getRoleIndex() == i)
			player->setIsMe(true);
		this->addChild(player);
		if (player->getIsMe())
		{
			auto centerOfView = Point(visibleSize.width / 2, visibleSize.height / 2);
			this->setPosition(centerOfView - player->getPosition());
		}

		auto skillStartPos = Sprite::createWithSpriteFrameName("skill_flash.png");
		skillStartPos->setScale(0.4f);
		skillStartPos->setVisible(false);
		this->addChild(skillStartPos);

		auto bomb = Bomb::create();
		this->addChild(bomb);
		bomb->setPosition(10000.f, 10000.f);

		if (roleType == ROLE_HERO)
		{
			static_cast<Hero *>(player)->setSkillStartPos(skillStartPos);
			static_cast<Hero *>(player)->setBomb(bomb);
		}

		auto foresight = Foresight::create();
		foresight->setScale(0.4f);
		foresight->setVisible(false);
		player->setForesight(foresight);
		this->addChild(foresight);

		GameData::getInstance()->m_pPlayers[i] = player;
	}

	importGroundData(m_pTiledMap);

	auto listener = EventListenerCustom::create("bullet_disappear", [this](EventCustom* event) {
		Bullet* bullet = static_cast<Bullet *>(event->getUserData());
		if (bullet)
			this->removeChild(bullet);
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
	m_vecEventListener.pushBack(listener);

	auto listener1 = EventListenerCustom::create("shoot_bullet", [this](EventCustom* event) {
		BaseSprite* player = static_cast<BaseSprite *>(event->getUserData());
		Bullet* bullet = getUnusedBullet();
		bullet->launch(player);
		this->addChild(bullet);
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listener1, 1);
	m_vecEventListener.pushBack(listener1);

	auto listener2 = EventListenerCustom::create("throw_bomb", [this](EventCustom* event) {
		Hero* hero = static_cast<Hero *>(event->getUserData());
		hero->getBomb()->launch(hero);
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listener2, 1);
	m_vecEventListener.pushBack(listener2);
    
    auto listener3 = EventListenerCustom::create("bomb_explode", [this](EventCustom* event) {
        
		Bomb* bomb = static_cast<Bomb *>(event->getUserData());
		explodeEnemy(bomb);
		bomb->setPosition(10000.f, 10000.f);
		bomb->getBombRange()->setVisible(false);
    });
    _eventDispatcher->addEventListenerWithFixedPriority(listener3, 1);
    m_vecEventListener.pushBack(listener3);
    
    auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = [this](PhysicsContact& contact)->bool
	{
		if ((contact.getShapeA()->getCategoryBitmask() == PC_Hero && contact.getShapeB()->getCategoryBitmask() == PC_Ground) ||
			(contact.getShapeA()->getCategoryBitmask() == PC_Ground && contact.getShapeB()->getCategoryBitmask() == PC_Hero) || 
			(contact.getShapeA()->getCategoryBitmask() == PC_Hero && contact.getShapeB()->getCategoryBitmask() == PC_Box)    ||
			(contact.getShapeA()->getCategoryBitmask() == PC_Box && contact.getShapeB()->getCategoryBitmask() == PC_Hero)    ||
			(contact.getShapeA()->getCategoryBitmask() == PC_Hero && contact.getShapeB()->getCategoryBitmask() == PC_Slope)  ||
			(contact.getShapeA()->getCategoryBitmask() == PC_Slope && contact.getShapeB()->getCategoryBitmask() == PC_Hero)
		)
		{
			BaseSprite* hero;
			if (contact.getShapeA()->getCategoryBitmask() == PC_Hero)
			{
				hero = static_cast<BaseSprite *>(contact.getShapeA()->getBody()->getNode());
			}
			else
			{
				hero = static_cast<BaseSprite *>(contact.getShapeB()->getBody()->getNode());
			}
			if (hero->getCurrActionState() == ACTION_STATE_JUMP_DOWN)
			{
				hero->stop();
				EventCustom event("collision");
				event.setUserData(hero);
				_eventDispatcher->dispatchEvent(&event);
				hero->setJumpStage(0);
			}
			//CCLOG("collision %f, %f", hero->getPhysicsBody()->getVelocity().x, hero->getPhysicsBody()->getVelocity().y);
			return true;
		}
        else if((contact.getShapeA()->getCategoryBitmask() == PC_Bullet && contact.getShapeB()->getCategoryBitmask() == PC_Ground) ||
				(contact.getShapeA()->getCategoryBitmask() == PC_Ground && contact.getShapeB()->getCategoryBitmask() == PC_Bullet) ||
				(contact.getShapeA()->getCategoryBitmask() == PC_Bullet && contact.getShapeB()->getCategoryBitmask() == PC_Box)	   ||
				(contact.getShapeA()->getCategoryBitmask() == PC_Box && contact.getShapeB()->getCategoryBitmask() == PC_Bullet)    ||
				(contact.getShapeA()->getCategoryBitmask() == PC_Bullet && contact.getShapeB()->getCategoryBitmask() == PC_Slope)  ||
				(contact.getShapeA()->getCategoryBitmask() == PC_Slope && contact.getShapeB()->getCategoryBitmask() == PC_Bullet)
		)
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
				
			if (bullet->getOwnerTag() % 2 == hero->getTag() % 2)
			{
				return false;
			}
			else
			{
				bullet->setIsActive(false);
				this->removeChild(bullet);

				if (SocketManager::getInstance()->getNetworkType() == NT_Offline)
				{
					hero->hurt(bullet->getPower());
					if (hero->getHP() <= 0)
					{
						hero->reset();
					}
				}
				else
				{
					BaseSprite* self = GameData::getInstance()->getMySelf();
					if (bullet->getOwnerTag() == self->getTag())
					{
						hero->hurt(bullet->getPower());
						if (hero->getHP() <= 0)
						{
							hero->reset();
							SocketManager::getInstance()->sendData(NDT_HeroDead, hero->getTag(), hero->getCurrActionState(), hero->getPosition(), Vec2(0.f, 0.f));
						}
						else
						{
							SocketManager::getInstance()->sendData(NDT_HeroHurt, hero->getTag(), hero->getCurrActionState(), hero->getPosition(), Vec2(bullet->getPower(), 0.f));
						}
					}
				}
				return true;
			}		
		}
		else if (	(contact.getShapeA()->getCategoryBitmask() == PC_Bomb && contact.getShapeB()->getCategoryBitmask() == PC_Ground)	||
					(contact.getShapeA()->getCategoryBitmask() == PC_Ground && contact.getShapeB()->getCategoryBitmask() == PC_Bomb)	||
					(contact.getShapeA()->getCategoryBitmask() == PC_Bomb && contact.getShapeB()->getCategoryBitmask() == PC_Box)		||
					(contact.getShapeA()->getCategoryBitmask() == PC_Box && contact.getShapeB()->getCategoryBitmask() == PC_Bomb)		||
					(contact.getShapeA()->getCategoryBitmask() == PC_Bomb && contact.getShapeB()->getCategoryBitmask() == PC_Slope)		||
					(contact.getShapeA()->getCategoryBitmask() == PC_Slope && contact.getShapeB()->getCategoryBitmask() == PC_Bomb)
		)
		{
			Bomb* bomb;
			if (contact.getShapeA()->getCategoryBitmask() == PC_Bomb)
				bomb = static_cast<Bomb *>(contact.getShapeA()->getBody()->getNode());
			else
				bomb = static_cast<Bomb *>(contact.getShapeB()->getBody()->getNode());
			if (bomb)
			{
				bomb->start();
				bomb->getBombRange()->setVisible(true);
			}
			return true;
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

			if (hero->getCurrActionState() == ACTION_STATE_WALK || hero->getCurrActionState() == ACTION_STATE_IDLE)
			{
				hero->runJumpDownAction();
			}
		}
		else if ((contact.getShapeA()->getCategoryBitmask() == PC_Hero && contact.getShapeB()->getCategoryBitmask() == PC_Ground) ||
			(contact.getShapeA()->getCategoryBitmask() == PC_Ground && contact.getShapeB()->getCategoryBitmask() == PC_Hero) ||
			(contact.getShapeA()->getCategoryBitmask() == PC_Hero && contact.getShapeB()->getCategoryBitmask() == PC_Box) ||
			(contact.getShapeA()->getCategoryBitmask() == PC_Box && contact.getShapeB()->getCategoryBitmask() == PC_Hero)
			)
		{
			BaseSprite* hero;
			if (contact.getShapeA()->getCategoryBitmask() == PC_Hero)
			{
				hero = static_cast<BaseSprite *>(contact.getShapeA()->getBody()->getNode());
			}
			else
			{
				hero = static_cast<BaseSprite *>(contact.getShapeB()->getBody()->getNode());
			}
			EventCustom event("collision");
			event.setUserData(hero);
			_eventDispatcher->dispatchEvent(&event);
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
	this->updatePlayer(dt);
	this->updateBullet(dt);
	this->updatePhysicsWorld(dt);
}

void GameLayer::updatePlayer(float dt)
{
	int playerNum = GameData::getInstance()->getPlayerNum();
	for (int i = 0; i < playerNum; ++i)
	{
		BaseSprite* player = GameData::getInstance()->m_pPlayers[i];
		if (player)
		{
			player->update(dt);
			if (player->getIsMe())
			{
				setViewPointCenter();
#if 1
				if (player->getIsLocked() && m_pTarget == nullptr)
				{
					m_pTarget = getNearestEnemy();
					if (m_pTarget)
						m_pTarget->getLockMark()->setVisible(true);
				}
				else if (!player->getIsLocked() && m_pTarget != nullptr)
				{
					m_pTarget->getLockMark()->setVisible(false);
					m_pTarget = nullptr;
				}

				if (m_pTarget != nullptr)
				{
					Vec2 direction = m_pTarget->getPosition() - player->getPosition();
					direction.normalize();
					player->setShootDirection(direction);
				}
#else
				if (player->getIsAutoShoot())
				{
					auto target = getNearestEnemy();
					if (m_pTarget != nullptr && target == nullptr)
					{
						m_pTarget->getLockMark()->setVisible(false);
						m_pTarget = nullptr;
					}
					else if (m_pTarget == nullptr)
					{
						m_pTarget = target;
					}

					if (m_pTarget)
					{
						m_pTarget->getLockMark()->setVisible(true);
					}
				}
				else
				{
					if (m_pTarget != nullptr)
					{
						m_pTarget->getLockMark()->setVisible(false);
						m_pTarget = nullptr;
					}
				}
				if (player->getIsAutoShoot())
				{
					if (m_pTarget != nullptr)
					{
						Vec2 direction = m_pTarget->getPosition() - player->getPosition();
						direction.normalize();
						player->setShootDirection(direction);
						EventCustom event("auto_shoot");
						event.setUserData(this);
						_eventDispatcher->dispatchEvent(&event);
					}
					else
					{
						EventCustom event("auto_shoot_finish");
						event.setUserData(this);
						_eventDispatcher->dispatchEvent(&event);
					}
				}
#endif
			}
			player->setPrePosition(player->getPosition());
		}
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
	bullet->getPhysicsBody()->setGravityEnable(false);
	m_vecBullets.pushBack(bullet);
	return bullet;
}

void GameLayer::setViewPointCenter() {
	auto winSize = Director::getInstance()->getWinSize();
	auto centerOfView = Point(winSize.width / 2, winSize.height / 2);
	

	auto heroPos = GameData::getInstance()->getMySelf()->getPosition();
	auto layerPos = this->getPosition();
	auto heroPosInScreen = heroPos + layerPos;
	this->setPositionX(centerOfView.x - heroPos.x);
	//this->setPosition(centerOfView - heroPos);

    auto diff = heroPosInScreen - centerOfView;
	if (diff.y > 0)
	{
		this->setPositionY(centerOfView.y - heroPos.y);
	}

    else if(diff.y < -25.f)
    {     
		auto heroPrePosY = GameData::getInstance()->getMySelf()->getPrePosition().y;
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
	sprite->setScale(0.6f);
	sprite->setPosition(pos);
	sprite->runIdleAction();
	sprite->setLocalZOrder(visibleSize.height - sprite->getPositionY());
	sprite->setHP(100);
	sprite->setIsAttacking(false);
	sprite->setJumpStage(0);

	ProgressTimer* blood = ProgressTimer::create(Sprite::create("blood.png"));
	blood->setName("blood");
	blood->setType(ProgressTimer::Type::BAR);
	blood->setMidpoint(Point(0, 0));
	blood->setBarChangeRate(Point(1, 0));
	blood->setAnchorPoint(Point(0, 1));
	blood->setPosition(60, 150);
	blood->setPercentage(100);
    blood->setScale(0.375f);


	ProgressTimer *bloodBg = ProgressTimer::create(Sprite::create("bloodBg.png"));
	bloodBg->setType(ProgressTimer::Type::BAR);
	bloodBg->setMidpoint(Point(0, 0));
	bloodBg->setBarChangeRate(Point(1, 0));
	bloodBg->setAnchorPoint(Point(0, 1));
	bloodBg->setPosition(blood->getPosition() + Point(-25.f, 3.f));
	bloodBg->setPercentage(100);
    bloodBg->setScale(0.5f);

	sprite->addChild(bloodBg);
	sprite->addChild(blood);

	return sprite;
}

BaseSprite* GameLayer::getNearestEnemy()
{
    BaseSprite* target = nullptr;
	BaseSprite* self = GameData::getInstance()->getMySelf();
    
    float distance = 100000.f;
	int playerNum = GameData::getInstance()->getPlayerNum();
	for (int i = 0; i < playerNum; ++i)
	{
		BaseSprite* player = GameData::getInstance()->m_pPlayers[i];
        if(player == nullptr)
            continue;

		if(player->getIsMe())
			continue;
        
        if(fabsf(self->getPosition().x - player->getPosition().x) > 450.f ||  fabsf(self->getPosition().y - player->getPosition().y) > 320.f)
            continue;
        
		float d = self->getPosition().getDistanceSq(player->getPosition());
		if (d < distance)
		{
			distance = d;
            target = player;
        }
	}
	return target;
}

void GameLayer::explodeEnemy(Bomb* bomb)
{
	int playerNum = GameData::getInstance()->getPlayerNum();
	for (int i = 0; i < playerNum; ++i)
	{
		BaseSprite* player = GameData::getInstance()->m_pPlayers[i];
		if (player == nullptr || player->getTag() % 2 == bomb->getOwner()->getTag() % 2)
			continue;

		float d = bomb->getPosition().getDistanceSq(player->getPosition());
		if (d < bomb->getRange() * bomb->getRange())
		{
			if (SocketManager::getInstance()->getNetworkType() == NT_Offline)
			{
				player->hurt(bomb->getPower());
				if (player->getHP() <= 0)
				{
					player->reset();
				}
			}
			else
			{
				if (bomb->getOwner()->getTag() == GameData::getInstance()->getMySelf()->getTag())
				{
					player->hurt(bomb->getPower());
					if (player->getHP() <= 0)
					{
						player->reset();
						SocketManager::getInstance()->sendData(NDT_HeroDead, player->getTag(), player->getCurrActionState(), player->getPosition(), Vec2(0.f, 0.f));
					}
					else
					{
						SocketManager::getInstance()->sendData(NDT_HeroHurt, player->getTag(), player->getCurrActionState(), player->getPosition(), Vec2(bomb->getPower(), 0.f));
					}
				}
			}			
		}
	}
}

