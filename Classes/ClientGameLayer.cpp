#include <math.h>
#include "Macro.h"
#include "ClientGameLayer.h"
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
#include "GameData.h"

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

void ClientGameLayer::onEnter()
{
	GameLayer::onEnter();

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("hero.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("gunner.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("princess.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui.plist");
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    this->m_origin = Director::getInstance()->getVisibleOrigin();
    
    TMXObjectGroup *objects = m_pTiledMap->getObjectGroup("Objects");
    CCASSERT(NULL != objects, "'Objects' object group not found");
    auto spawnPoint = objects->getObject("SpawnPoint1");
    CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
    Point heroInitPos = m_origin + Point(spawnPoint["x"].asFloat(), spawnPoint["y"].asFloat());
    m_pHero = createHero(ROLE_PRINCESS, heroInitPos);
	m_pHero->setTag(1);
    this->addChild(m_pHero);
    auto centerOfView = Point(visibleSize.width / 2, visibleSize.height / 2);
    this->setPosition(centerOfView - m_pHero->getPosition());
    
    m_pRange = Sprite::createWithSpriteFrameName("range.png");
    m_pRange->setVisible(false);
    m_pRange->setPosition(Point(140.f, 25.f));
    m_pHero->addChild(m_pRange);
    
    m_pForesight = Foresight::create();
    m_pForesight->setScale(0.2f);
    m_pForesight->setVisible(false);
    this->addChild(m_pForesight);
    
    
    spawnPoint = objects->getObject("SpawnPoint");
    CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
    heroInitPos = m_origin + Point(spawnPoint["x"].asFloat(), spawnPoint["y"].asFloat());
    m_pEnemy[0] = createHero(ROLE_HERO, heroInitPos);
	m_pEnemy[0]->setTag(0);
    this->addChild(m_pEnemy[0]);
    
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
                for (int i = 0; i < 3; ++i)
                {
                    if (m_pEnemy[i])
                    {
                        m_pEnemy[i]->setWalkVelocity(s);
                    }
                }
            }
            else if (pair.first.compare("HeroVSpeed") == 0)
            {
                m_pHero->setJumpVelocity(pair.second.asFloat());
                
                for (int i = 0; i < 3; ++i)
                {
                    if (m_pEnemy[i])
                    {
                        m_pEnemy[i]->setJumpVelocity(pair.second.asFloat());
                    }
                }
            }
            else if (pair.first.compare("HeroG") == 0)
            {
                m_pHero->setGravity(pair.second.asFloat());
                for (int i = 0; i < 3; ++i)
                {
                    if (m_pEnemy[i])
                    {
                        m_pEnemy[i]->setGravity(pair.second.asFloat());
                    }
                }
            }
        }
    }

   SocketClient::getInstance()->onRecv = CC_CALLBACK_2(ClientGameLayer::onRecv, this);
   SocketClient::getInstance()->onDisconnect = CC_CALLBACK_0(ClientGameLayer::onDisconnect, this);
}

void ClientGameLayer::onRecv(const char* data, int count)
{
	NetworkData* networkData = (NetworkData*)data;
	if (networkData->dataSize == sizeof(NetworkData))
	{
		switch (networkData->dataType)
		{
		case NDT_HeroWalk:
			//m_pHero->runWalkAction(!m_pHero->isInAir());
			m_pEnemy[0]->setPosition(networkData->position);
			m_pEnemy[0]->walk(networkData->vec.x);
			break;
		case NDT_HeroJumpUp:
			//m_pHero->runJumpAction(true);
			m_pEnemy[0]->setPosition(networkData->position);
			//m_pHero->getPhysicsBody()->setVelocity(networkData->velocity);
			m_pEnemy[0]->jump(m_pEnemy[0]->getJumpVelocity());
			break;
		case NDT_HeroJumpDown:
			m_pEnemy[0]->runJumpDownAction();
			m_pEnemy[0]->setPosition(networkData->position);
			m_pEnemy[0]->getPhysicsBody()->setVelocity(networkData->vec);
			break;
		case NDT_HeroStop:
			//m_pHero->runIdleAction();
			m_pEnemy[0]->setPosition(networkData->position);
			//m_pHero->getPhysicsBody()->setVelocity(networkData->velocity);
			m_pEnemy[0]->stop();
			break;
		case NDT_HeroPos:
			m_dequeShadow.push_back(networkData->vec);
			break;
		case NDT_HeroAttack:
			m_pEnemy[0]->setPosition(networkData->position);
			m_pEnemy[0]->setShootDirection(networkData->vec);
			m_pEnemy[0]->attack(true);
			break;
		case NDT_HeroStopAttack:
			m_pEnemy[0]->attack(false);
			m_pEnemy[0]->setPosition(networkData->position);
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
