#include <math.h>
#include "Macro.h"
#include "ServerGameLayer.h"
#include "Hero.h"
#include "Gunner.h"
#include "Princess.h"
#include "Bullet.h"
#include "Foresight.h"
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

void ServerGameLayer::onEnter()
{
	GameLayer::onEnter();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	this->m_origin = Director::getInstance()->getVisibleOrigin();

	TMXObjectGroup *objects = m_pTiledMap->getObjectGroup("Objects");
	CCASSERT(NULL != objects, "'Objects' object group not found");

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("hero.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("gunner.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("princess.plist");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui.plist");


	auto spawnPoint = objects->getObject("SpawnPoint");
	CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
	Point heroInitPos = m_origin + Point(spawnPoint["x"].asFloat(), spawnPoint["y"].asFloat());
	m_pHero = Hero::create();
	m_pHero->setInitPos(heroInitPos);
	m_pHero->setTag(0);
	m_pHero->setScale(0.5f);
	m_pHero->setPosition(heroInitPos);
	m_pHero->runIdleAction();
	m_pHero->setLocalZOrder(visibleSize.height - m_pHero->getPositionY());
	m_pHero->setHP(100);
	m_pHero->setIsAttacking(false);
	m_pHero->setJumpStage(0);

	this->addChild(m_pHero);
	auto centerOfView = Point(visibleSize.width / 2, visibleSize.height / 2);
	this->setPosition(centerOfView - m_pHero->getPosition());


	spawnPoint = objects->getObject("SpawnPoint1");
	CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
	heroInitPos = m_origin + Point(spawnPoint["x"].asFloat(), spawnPoint["y"].asFloat());
	m_pEnemy[0] = Princess::create();
	m_pEnemy[0]->setInitPos(heroInitPos);
	m_pEnemy[0]->setTag(1);
	m_pEnemy[0]->setScale(0.5f);
	m_pEnemy[0]->setPosition(heroInitPos);
	m_pEnemy[0]->runIdleAction();
	m_pEnemy[0]->setLocalZOrder(visibleSize.height - m_pHero->getPositionY());
	m_pEnemy[0]->setHP(100);
	m_pEnemy[0]->setIsAttacking(false);
	m_pEnemy[0]->setJumpStage(0);
	this->addChild(m_pEnemy[0]);

	ProgressTimer* blood = ProgressTimer::create(Sprite::createWithSpriteFrameName("blood.png"));
	blood->setName("blood");
	blood->setType(ProgressTimer::Type::BAR);
	blood->setMidpoint(Point(0, 0));
	blood->setBarChangeRate(Point(1, 0));
	blood->setAnchorPoint(Point(0, 1));
	blood->setPosition(50, 150);
	blood->setPercentage(100);


	ProgressTimer *bloodBg = ProgressTimer::create(Sprite::createWithSpriteFrameName("bloodBg.png"));
	bloodBg->setType(ProgressTimer::Type::BAR);
	bloodBg->setMidpoint(Point(0, 0));
	bloodBg->setBarChangeRate(Point(1, 0));
	bloodBg->setAnchorPoint(Point(0, 1));
	bloodBg->setPosition(blood->getPosition());
	bloodBg->setPercentage(100);

	m_pEnemy[0]->addChild(bloodBg);
	m_pEnemy[0]->addChild(blood);

	m_pHero->addChild(m_pForesight);

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
				m_pEnemy[0]->setWalkVelocity(s);
			}
			else if (pair.first.compare("HeroVSpeed") == 0)
			{
				m_pHero->setJumpVelocity(pair.second.asFloat());
				m_pEnemy[0]->setJumpVelocity(pair.second.asFloat());
			}
			else if (pair.first.compare("HeroG") == 0)
			{
				m_pHero->setGravity(pair.second.asFloat());
				m_pEnemy[0]->setGravity(pair.second.asFloat());
			}
			else if (pair.first.compare("BulletPower") == 0)
			{
				m_pHero->setBullletPower(pair.second.asInt());
				m_pEnemy[0]->setBullletPower(pair.second.asInt());
			}
			else if (pair.first.compare("BulletSpeed") == 0)
			{
				m_pHero->setBulletLaunchVelocity(pair.second.asFloat());
				m_pEnemy[0]->setBulletLaunchVelocity(pair.second.asFloat());
			}
			else if (pair.first.compare("BulletDisappearTime") == 0)
			{
				m_pHero->setBulletDisappearTime(pair.second.asFloat());
				m_pEnemy[0]->setBulletDisappearTime(pair.second.asFloat());
			}
			else if (pair.first.compare("BulletAngle") == 0)
			{
				m_pHero->setBullletAngle(pair.second.asInt());
				m_pEnemy[0]->setBullletAngle(pair.second.asInt());
			}
			else if (pair.first.compare("BulletInterval") == 0)
			{
				m_pHero->setBulletInterval(pair.second.asFloat());
				m_pEnemy[0]->setBulletInterval(pair.second.asFloat());
			}
			else if (pair.first.compare("BulletG") == 0)
			{
				m_pHero->setBulletGravity(pair.second.asFloat());
				m_pEnemy[0]->setBulletGravity(pair.second.asFloat());
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
	m_shootTime = m_pHero->getBulletInterval();

	SocketServer::getInstance()->onRecv = CC_CALLBACK_3(ServerGameLayer::onRecv, this);
	SocketServer::getInstance()->onDisconnect = CC_CALLBACK_1(ServerGameLayer::onDisconnect, this);
}

void ServerGameLayer::onRecv(HSocket socket, const char* data, int count)
{
	NetworkData* networkData = (NetworkData*)data;
	if (networkData->dataSize == sizeof(NetworkData))
	{
		switch (networkData->dataType)
		{
		case NDT_HeroWalk:
			//m_pHero->runWalkAction(!m_pHero->isInAir());
			//m_pHero->setPosition(networkData->position);
			//m_pHero->getPhysicsBody()->setVelocity(networkData->velocity);
			m_pEnemy[0]->walk(networkData->vec.x);
			break;
		case NDT_HeroJumpUp:
			//m_pHero->runJumpAction(true);
			//m_pHero->setPosition(networkData->position);
			//m_pHero->getPhysicsBody()->setVelocity(networkData->velocity);
			m_pEnemy[0]->jump(m_pHero->getJumpVelocity());
			break;
		case NDT_HeroJumpDown:
			m_pEnemy[0]->runJumpAction(false);
			//m_pHero->setPosition(networkData->position);
			m_pEnemy[0]->getPhysicsBody()->setVelocity(networkData->vec);
			break;
		case NDT_HeroStop:
			//m_pHero->runIdleAction();
			//m_pHero->setPosition(networkData->position);
			//m_pHero->getPhysicsBody()->setVelocity(networkData->velocity);
			m_pEnemy[0]->stop();
			m_pEnemy[0]->stopMoveAction(MOVE_STATE_WALK, true);
			break;
		case NDT_HeroPos:
			m_dequeShadow.push_back(networkData->vec);
			break;
		case NDT_HeroAttack:
			m_pEnemy[0]->setShootDirection(networkData->vec);
			m_pEnemy[0]->attack(true);
			break;
		case NDT_HeroStopAttack:
			m_pEnemy[0]->attack(false);
			break;
		default:
			break;
		}
	}
}

void ServerGameLayer::onDisconnect(HSocket socket)
{
	CCLOG("Client disconnect");
}
