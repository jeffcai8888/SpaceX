#include <math.h>
#include "Macro.h"
#include "OfflineGameLayer.h"
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

OfflineGameLayer::OfflineGameLayer()
	:GameLayer()
{
	
}

OfflineGameLayer::~OfflineGameLayer()
{
	
}

void OfflineGameLayer::onEnter()
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
    auto spawnPoint = objects->getObject("SpawnPoint");
    CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
    Point heroInitPos = m_origin + Point(spawnPoint["x"].asFloat(), spawnPoint["y"].asFloat());
    m_pHero = createHero(ROLE_HERO, heroInitPos);
	m_pHero->setTag(0);
    this->addChild(m_pHero);
    auto centerOfView = Point(visibleSize.width / 2, visibleSize.height / 2);
    this->setPosition(centerOfView - m_pHero->getPosition());
    
    m_pRange = Sprite::createWithSpriteFrameName("range.png");
    m_pRange->setVisible(false);
    m_pRange->setPosition(Point(110.f, 25.f));
	m_pRange->setScale(2.0f);
    m_pHero->addChild(m_pRange);
    
    m_pForesight = Foresight::create();
    m_pForesight->setScale(0.75f);
    m_pForesight->setVisible(false);
    this->addChild(m_pForesight);
    
    
    spawnPoint = objects->getObject("SpawnPoint1");
    CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
    heroInitPos = m_origin + Point(spawnPoint["x"].asFloat(), spawnPoint["y"].asFloat());
    m_pEnemy[0] = createHero(ROLE_PRINCESS, heroInitPos);
	m_pEnemy[0]->setLockMark(Sprite::createWithSpriteFrameName("lock.png"));
	m_pEnemy[0]->addChild(m_pEnemy[0]->getLockMark());
	m_pEnemy[0]->getLockMark()->setPosition(m_pEnemy[0]->getContentSize().width / 2, 50.f);
	m_pEnemy[0]->getLockMark()->setVisible(false);
	m_pEnemy[0]->setTag(1);
    this->addChild(m_pEnemy[0]);
    
    
    heroInitPos = m_origin + Point(spawnPoint["x"].asFloat() - 300.f, spawnPoint["y"].asFloat());
    m_pEnemy[1] = createHero(ROLE_GUN, heroInitPos);
	m_pEnemy[1]->setLockMark(Sprite::createWithSpriteFrameName("lock.png"));
	m_pEnemy[1]->addChild(m_pEnemy[1]->getLockMark());
	m_pEnemy[1]->getLockMark()->setPosition(m_pEnemy[0]->getContentSize().width / 2, 50.f);
	m_pEnemy[1]->getLockMark()->setVisible(false);
	m_pEnemy[1]->setTag(2);
    this->addChild(m_pEnemy[1]);
    
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
			else if (pair.first.compare("HeroSkill1V") == 0)
			{
				Hero* hero = dynamic_cast<Hero*>(m_pHero);
				if (hero)
				{
					hero->setSkillState1Speed(pair.second.asFloat());
				}
			}
			else if (pair.first.compare("HeroSkill2V") == 0)
			{
				Hero* hero = dynamic_cast<Hero*>(m_pHero);
				if (hero)
				{
					hero->setSkillState2Speed(pair.second.asFloat());
				}
			}
			else if (pair.first.compare("HeroSkill1CD") == 0)
			{
				Hero* hero = dynamic_cast<Hero*>(m_pHero);
				if (hero)
				{
					hero->setSkillState1CDTime(pair.second.asFloat());
				}
			}
			else if (pair.first.compare("HeroSkill2CD") == 0)
			{
				Hero* hero = dynamic_cast<Hero*>(m_pHero);
				if (hero)
				{
					hero->setSkillState2CDTime(pair.second.asFloat());
				}
			}
            else if (pair.first.compare("BulletPower") == 0)
            {
                m_pHero->setBullletPower(pair.second.asInt());
                for (int i = 0; i < 3; ++i)
                {
                    if (m_pEnemy[i])
                    {
                        m_pEnemy[i]->setBullletPower(pair.second.asInt());
                    }
                }
            }
            else if (pair.first.compare("BulletSpeed") == 0)
            {
                m_pHero->setBulletLaunchVelocity(pair.second.asFloat());
                for (int i = 0; i < 3; ++i)
                {
                    if (m_pEnemy[i])
                    {
                        m_pEnemy[i]->setBulletLaunchVelocity(pair.second.asFloat());
                    }
                }
            }
            else if (pair.first.compare("BulletDisappearTime") == 0)
            {
                m_pHero->setBulletDisappearTime(pair.second.asFloat());
                for (int i = 0; i < 3; ++i)
                {
                    if (m_pEnemy[i])
                    {
                        m_pEnemy[i]->setBulletDisappearTime(pair.second.asFloat());
                    }
                }
            }
            else if (pair.first.compare("BulletAngle") == 0)
            {
                m_pHero->setBullletAngle(pair.second.asInt());
                for (int i = 0; i < 3; ++i)
                {
                    if (m_pEnemy[i])
                    {
                        m_pEnemy[i]->setBullletAngle(pair.second.asInt());
                    }
                }
            }
            else if (pair.first.compare("BulletInterval") == 0)
            {
                m_pHero->setBulletInterval(pair.second.asFloat());
                for (int i = 0; i < 3; ++i)
                {
                    if (m_pEnemy[i])
                    {
                        m_pEnemy[i]->setBulletInterval(pair.second.asFloat());
                    }
                }
            }
            else if (pair.first.compare("BulletG") == 0)
            {
                m_pHero->setBulletGravity(pair.second.asFloat());
                for (int i = 0; i < 3; ++i)
                {
                    if (m_pEnemy[i])
                    {
                        m_pEnemy[i]->setBulletGravity(pair.second.asFloat());
                    }
                }
            }
            else if (pair.first.compare("AmmoCapacity") == 0)
            {
                m_pHero->setMaxAmmoCapacity(pair.second.asInt());
                m_pHero->setAmmoCapacity(pair.second.asInt());
            }
			else if (pair.first.compare("BombSpeed") == 0)
			{
				Hero* hero = dynamic_cast<Hero*>(m_pHero);
				if (hero)
				{
					hero->setBombSpeed(pair.second.asFloat());
				}
			}
			else if (pair.first.compare("BombRange") == 0)
			{
				Hero* hero = dynamic_cast<Hero*>(m_pHero);
				if (hero)
				{
					hero->setBombRange(pair.second.asFloat());
				}
			}
			else if (pair.first.compare("BombStartTime") == 0)
			{
				Hero* hero = dynamic_cast<Hero*>(m_pHero);
				if (hero)
				{
					hero->setBombStartTime(pair.second.asFloat());
				}
			}
			else if (pair.first.compare("BombCDTime") == 0)
			{
				Hero* hero = dynamic_cast<Hero*>(m_pHero);
				if (hero)
				{
					hero->setBombCDTime(pair.second.asFloat());
				}
			}
			else if (pair.first.compare("BombPower") == 0)
			{
				Hero* hero = dynamic_cast<Hero*>(m_pHero);
				if (hero)
				{
					hero->setBombPower(pair.second.asInt());
				}
			}
			else if (pair.first.compare("BombG") == 0)
			{
				Hero* hero = dynamic_cast<Hero*>(m_pHero);
				if (hero)
				{
					hero->setBombGravity(pair.second.asFloat());
				}
			}
        }
    }
    m_shootTime = m_pHero->getBulletInterval();
}

void OfflineGameLayer::onExit()
{
    GameLayer::onExit();
}
