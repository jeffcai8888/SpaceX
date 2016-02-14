#include "GameLayer.h"
#include "Hero.h"
#include "Bullet.h"
#include "Ground.h"

#include "SceneManager.h"
#include "SimpleAudioEngine.h"
using namespace cocos2d;

GameLayer::GameLayer()
	:m_pTiledMap(nullptr),
	m_pSpriteNodes(nullptr),
	m_pHero(nullptr)
{
	m_vecBullets.clear();
}

GameLayer::~GameLayer()
{
	_eventDispatcher->removeAllEventListeners();
	this->unscheduleUpdate();
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
	
		m_pHero->onDeadCallback = CC_CALLBACK_0(GameLayer::onHeroDead, this, m_pHero);
		m_pHero->attack = CC_CALLBACK_0(GameLayer::onHeroAttack, this);
		m_pHero->stop = CC_CALLBACK_0(GameLayer::onHeroStop, this);
		m_pHero->walk = CC_CALLBACK_2(GameLayer::onHeroWalk, this);
		m_pHero->jump = CC_CALLBACK_2(GameLayer::onHeroJump, this);
		m_pSpriteNodes->addChild(m_pHero);

		const PhysicsMaterial m(1.f, 0.f, 1.f);
		Size boxSize(m_pTiledMap->getMapSize().width * m_pTiledMap->getTileSize().width, m_pTiledMap->getMapSize().height * m_pTiledMap->getTileSize().height);
		auto body = PhysicsBody::createEdgeBox(boxSize, m, 3);
		body->setCategoryBitmask(0x04);
		body->setCollisionBitmask(0x01);
		auto edgeNode = Node::create();
		edgeNode->setPosition(Point(boxSize.width / 2, boxSize.height / 2));
		edgeNode->setPhysicsBody(body);
		this->addChild(edgeNode);

		importGroundData(m_pTiledMap);

		auto listener = EventListenerCustom::create("bullet_disappear", [this](EventCustom* event) {
			Bullet* bullet = static_cast<Bullet *>(event->getUserData());
			if(bullet)
				this->removeChild(bullet);
		});

		_eventDispatcher->addEventListenerWithFixedPriority(listener, 1);

		m_shootTime = 1.f;
		//CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(PATH_BG_MUSIC, true);
		//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(PATH_HERO_TALK_EFFECT);

		this->scheduleUpdate();

		ret = true;
	} while(0);

	return ret;
}

/*void GameLayer::onEnter()
{
	int i = 0;
}

void GameLayer::onExit()
{
	int i = 0;
}*/

void GameLayer::onHeroWalk(Point direction, float distance)
{
	if(m_pHero->isLive() && !m_pHero->isJump())
	{
		m_pHero->runWalkAction();

		//Point velocity;
		if (direction.x > 0)
		{
			m_pHero->setVelocity(distance < 78 ? 1 : 3);
			m_pHero->setMoveDirection(Vec2(1.f, 0.f));
		}
		else
		{
			m_pHero->setVelocity(distance < 78 ? 1 : 3);
			m_pHero->setMoveDirection(Vec2(-1.f, 0.f));
		}	
		CCLOG("WALK");
	}
}

void GameLayer::onHeroJump(Point direction, float distance)
{
	; 
	if (m_pHero->isLive() && !m_pHero->isJump())
	{
		//m_pHero->setFlippedX(direction.x < 0 ? true : false);
		m_pHero->runJumpAction(true);

		m_pHero->setVelocity(distance < 78 ? 150 : 450);
		m_pHero->setMoveDirection(direction);
		//m_pHero->getPhysicsBody()->applyForce(direction * m_pHero->getVelocity());
		m_pHero->getPhysicsBody()->setVelocity(direction * m_pHero->getVelocity());
		CCLOG("JUMP");
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
}

void GameLayer::updateHero(float dt)
{
	if(m_pHero->getCurrActionState() == ACTION_STATE_WALK)
	{
		
		Point expectP = m_pHero->getPosition() + m_pHero->getVelocity() * m_pHero->getMoveDirection();
		Point actualP = expectP;
		//can not walk on the wall or out of map		
		float mapWidth = m_pTiledMap->getContentSize().width;
		float halfWinWidth = m_fScreenWidth / 2;
		float halfHeroFrameWidth = (m_pHero->getSpriteFrame()->getRect().size.width) / 2;
		if(expectP.x > halfWinWidth && expectP.x <= (mapWidth - halfWinWidth))
		{
			this->setPositionX(this->getPositionX() - (m_pHero->getVelocity() * m_pHero->getMoveDirection()).x);
		}else if(expectP.x < halfHeroFrameWidth || expectP.x >= mapWidth - halfHeroFrameWidth)
		{
			actualP.x = m_pHero->getPositionX();
		}
		m_pHero->setPosition(actualP);
		m_pHero->setLocalZOrder(m_fScreenHeight - m_pHero->getPositionY());
		m_pHero->setFlippedX(m_pHero->getShootDirection().x < 0 ? true : false);
	}
	else if (m_pHero->isJump())
	{
		m_pHero->setFlippedX(m_pHero->getShootDirection().x < 0 ? true : false);
	}

	setViewPointCenter(m_pHero->getPosition());

	if (m_pHero->getPhysicsBody()->getVelocity().y < 0.000000001f && m_pHero->getPhysicsBody()->getVelocity().y > -0.000000001f)
	{
		if(m_pHero->getCurrActionState() == ACTION_STATE_JUMP_UP)
			m_pHero->runJumpAction(false);
		else if(m_pHero->getCurrActionState() == ACTION_STATE_JUMP_DOWN)
			m_pHero->runIdleAction();
	}


	if (m_pHero->getIsAttacking())
	{
		m_shootTime += dt;
		if (m_shootTime >= 1.f)
		{
			Bullet* bullet = getUnusedBullet();
			bullet->setVelocity(200.f);
			bullet->setDirection(m_pHero->getShootDirection());
			bullet->setDisappearDistance(90000.f);
			bullet->launch(m_pHero);
			this->addChild(bullet);
			m_shootTime = 0.f;
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

void GameLayer::setViewPointCenter(Point position) {
	auto winSize = Director::getInstance()->getWinSize();

	int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);
	int w = m_pTiledMap->getMapSize().width;
	int h = m_pTiledMap->getMapSize().height;
	x = MIN(x, (m_pTiledMap->getMapSize().width * m_pTiledMap->getTileSize().width) - winSize.width / 2);
	y = MIN(y, (m_pTiledMap->getMapSize().height * m_pTiledMap->getTileSize().height) - winSize.height / 2);
	auto actualPosition = Point(x, y);

	auto centerOfView = Point(winSize.width / 2, winSize.height / 2);
	auto viewPoint = centerOfView - actualPosition;

	this->setPosition(viewPoint);
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
