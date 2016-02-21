#include <math.h>
#include "Macro.h"
#include "GameLayer.h"
#include "Hero.h"
#include "Bullet.h"
#include "Ground.h"
#include "OperateLayer.h"

#include "SceneManager.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

GameLayer::GameLayer()
	:m_pTiledMap(nullptr),
	m_pSpriteNodes(nullptr),
	m_pHero(nullptr)
{
	m_vecBullets.clear();
	m_vecEventListener.clear();
}

GameLayer::~GameLayer()
{
	m_vecBullets.clear();
}

bool GameLayer::init()
{
	bool ret = false;
	do {
		CC_BREAK_IF( !Layer::init());
		auto visibleSize = Director::getInstance()->getVisibleSize();
		this->m_origin = Director::getInstance()->getVisibleOrigin();
		this->m_fScreenWidth = visibleSize.width;
		this->m_fScreenHeight = visibleSize.height;

		m_pTiledMap = TMXTiledMap::create("spacex_tilemap.tmx");
		this->addChild(m_pTiledMap);

		TMXObjectGroup *objects = m_pTiledMap->getObjectGroup("Objects");
		CCASSERT(NULL != objects, "'Objects' object group not found");

		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("pd_sprites.plist");
		m_pSpriteNodes = SpriteBatchNode::create("pd_sprites.pvr.ccz");
		this->addChild(m_pSpriteNodes);

		auto spawnPoint = objects->getObject("SpawnPoint");
		CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
		m_pHero = Hero::create();
		m_pHero->setPosition(m_origin + Point(spawnPoint["x"].asFloat(), spawnPoint["y"].asFloat()));
		m_pHero->runIdleAction();
		m_pHero->setLocalZOrder(m_fScreenHeight - m_pHero->getPositionY());
		m_pHero->setAttack(5);
		m_pHero->setHP(100);
		m_pHero->setIsAttacking(false);
		m_pHero->setJumpStage(0);

		m_pHero->onDeadCallback = CC_CALLBACK_0(GameLayer::onHeroDead, this, m_pHero);
		m_pHero->attack = CC_CALLBACK_0(GameLayer::onHeroAttack, this);
		m_pHero->stop = CC_CALLBACK_0(GameLayer::onHeroStop, this);
		m_pHero->walk = CC_CALLBACK_1(GameLayer::onHeroWalk, this);
		m_pHero->jump = CC_CALLBACK_1(GameLayer::onHeroJump, this);
		m_pSpriteNodes->addChild(m_pHero);
		auto centerOfView = Point(visibleSize.width / 2, visibleSize.height / 2);
		this->setPosition(centerOfView - m_pHero->getPosition());
		//CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(PATH_BG_MUSIC, true);
		//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(PATH_HERO_TALK_EFFECT);

		

		ret = true;
	} while(0);

	return ret;
}

void GameLayer::onEnter()
{
	Layer::onEnter();


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

	m_shootTime = 0.1f;


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
		if (contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Hero && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Ground)
		{
			Point posA = contact.getShapeA()->getBody()->getPosition();
			Point posB = contact.getShapeB()->getBody()->getPosition();
			if (posA.y >= posB.y)
			{
				Hero* hero = static_cast<Hero *>(contact.getShapeA()->getBody()->getNode());
				if (hero->getCurrActionState() == ACTION_STATE_MOVE && hero->isInMoveAction(MOVE_STATE_DOWN))
				{
					if (hero->isInMoveAction(MOVE_STATE_WALK))
					{
						hero->stopMoveAction(MOVE_STATE_DOWN);
						Vec2 v = hero->getPhysicsBody()->getVelocity();
						hero->walk(v.x);
						CCLOG("Hero Walk");
					}
					else
					{
						CCLOG("Hero Stop");
						hero->stop();
					}
					hero->setJumpStage(0);
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		else if (contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Ground && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Hero)
		{
			Point posA = contact.getShapeA()->getBody()->getPosition();
			Point posB = contact.getShapeB()->getBody()->getPosition();
			if (posA.y <= posB.y)
			{
				Hero* hero = static_cast<Hero *>(contact.getShapeB()->getBody()->getNode());
				if (hero->getCurrActionState() == ACTION_STATE_MOVE && hero->isInMoveAction(MOVE_STATE_DOWN))
				{
					if (hero->isInMoveAction(MOVE_STATE_WALK))
					{
						hero->stopMoveAction(MOVE_STATE_DOWN);
						Vec2 v = hero->getPhysicsBody()->getVelocity();
						hero->walk(v.x);
						CCLOG("Hero Walk");
					}
					else
					{
						CCLOG("Hero Stop");
						hero->stop();
					}
					hero->setJumpStage(0);
				}
				return true;
			}
			else
			{
				return false;
			}
		}
        else if(contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Bullet && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Ground)
        {
            Bullet* bullet = static_cast<Bullet *>(contact.getShapeA()->getBody()->getNode());
            if(bullet)
            {
                bullet->setIsActive(false);
                this->removeChild(bullet);
            }
        }
        else if(contact.getShapeA()->getBody()->getCategoryBitmask() == PC_Ground && contact.getShapeB()->getBody()->getCategoryBitmask() == PC_Bullet)
        {
            Bullet* bullet = static_cast<Bullet *>(contact.getShapeB()->getBody()->getNode());
            if(bullet)
            {
                bullet->setIsActive(false);
                this->removeChild(bullet);
            }
        }
		return true;
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
}


void GameLayer::onHeroWalk(float horizontalVelocity)
{
	if(m_pHero->isLive())
	{
		if (!m_pHero->isInAir())
		{
			m_pHero->runWalkAction();
		}

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
		m_pHero->setJumpStage(m_pHero->getJumpStage() + 1);
        
		Vec2 velocity = m_pHero->getPhysicsBody()->getVelocity();
        velocity.y = verticalVelocity;
		if (!m_pHero->getIsAttacking() && velocity.x != 0)
		{
			m_pHero->setFlippedX(velocity.x < 0);
		}
		m_pHero->getPhysicsBody()->setVelocity(velocity);
		m_pHero->setPrePositionY(m_pHero->getPosition().y);
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
}

void GameLayer::updateHero(float dt)
{
	setViewPointCenter();

	if (m_pHero->getCurrActionState() == ACTION_STATE_MOVE && m_pHero->isInMoveAction(MOVE_STATE_UP) && m_pHero->getPosition().y < m_pHero->getPrePositionY())
	{
		m_pHero->runJumpAction(false);
		CCLOG("Up -> Down");
	}
    m_pHero->setPrePositionY(m_pHero->getPosition().y);

	if (m_pHero->getIsAttacking())
	{
		m_shootTime += dt;
		if (m_shootTime >= 0.1f)
		{
			Bullet* bullet = getUnusedBullet();
			bullet->setVelocity(750.f);
			bullet->setDirection(m_pHero->getShootDirection().rotateByAngle(Vec2(0.f,0.f), CC_DEGREES_TO_RADIANS((int)(-rand_0_1() * 10))));
			bullet->setDisappearDistance(900000.f);
			bullet->launch(m_pHero);
			this->addChild(bullet);
			m_shootTime = 0.f;
			if(m_pHero->getShootDirection().x != 0)
				m_pHero->setFlippedX(m_pHero->getShootDirection().x < 0);
		}
	}
	else
	{
		auto operatorLayer = static_cast<OperateLayer *>(this->getScene()->getChildByTag(LT_Operate));		
		operatorLayer->resetTarget();
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
	/*int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);
	x = MIN(x, (m_pTiledMap->getMapSize().width * m_pTiledMap->getTileSize().width) - winSize.width / 2);
	y = MIN(y, (m_pTiledMap->getMapSize().height * m_pTiledMap->getTileSize().height) - winSize.height / 2);
	auto actualPosition = Point(x, y);*/

	auto centerOfView = Point(winSize.width / 2, winSize.height / 2);
    //dauto heroPos = m_pHero->getPhysicsBody()->getPosition();
	

	auto heroPos = m_pHero->getPosition();
	auto layerPos = this->getPosition();
	auto heroPosInScreen = heroPos + layerPos;
	this->setPositionX(centerOfView.x - heroPos.x);

    auto diff = heroPosInScreen - centerOfView;
    if(fabs(diff.y) > 25.f)
    {
		//this->setPositionY(centerOfView.y - heroPos.y);
		auto heroPrePosY = m_pHero->getPrePositionY();
		//CCLOG("%f, %f, %f, %f", heroPrePosY+layerPos.y, heroPrePosY , layerPos.y, heroPos.y);
		this->setPositionY(heroPrePosY + layerPos.y - heroPos.y);
    }
	
}

void GameLayer::importGroundData(cocos2d::TMXTiledMap* data)
{
	TMXObjectGroup *objects = m_pTiledMap->getObjectGroup("Grounds");
	int cnt = 0;
	while (true)
	{
		std::string name = "Ground" + Value(cnt).asString();
		auto object = objects->getObject(name);
		//CCASSERT(!object.empty(), "Ground object not found");
		if (object.empty())
			return;
		Size boxSize(object["width"].asFloat(), object["height"].asFloat());
		auto ground = Ground::create();
		ground->initPhysics(boxSize, Point(object["x"].asFloat() + boxSize.width / 2, object["y"].asFloat() + boxSize.height / 2));
		this->addChild(ground, 0, name);
		++cnt;
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
