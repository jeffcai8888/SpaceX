#include "GameLayer.h"
#include "Hero.h"
#include "Bullet.h"

#include "SceneManager.h"
#include "SimpleAudioEngine.h"
using namespace cocos2d;
bool collisionDetection(const BoundingBox &hitBox, const BoundingBox &bodyBox)
{
	Rect hitRect = hitBox.actual;
	Rect bodyRect = bodyBox.actual;
	if(hitRect.intersectsRect(bodyRect))
	{
		return true;
	}

	return false;
}


GameLayer::GameLayer()
	:m_pTiledMap(NULL),
	m_pSpriteNodes(NULL),
	m_pHero(NULL),
	m_pBlood(NULL),
	m_pBloodBg(NULL),
	m_pWorld(NULL)
{
	m_vecBullets.clear();
}

GameLayer::~GameLayer()
{
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

		m_pCloseItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(GameLayer::exitApp, this));
		m_pCloseItem->setPosition(this->m_origin + Point(visibleSize) - Point(m_pCloseItem->getContentSize() / 2));
		auto menu = Menu::create(m_pCloseItem, NULL);
		menu->setPosition(Point::ZERO);
		this->addChild(menu, 1);

		m_pTiledMap = TMXTiledMap::create("pd_tilemap.tmx");
		this->addChild(m_pTiledMap);
		Size tileSize = m_pTiledMap->getTileSize();
		m_fTileWidth = tileSize.width;
		m_fTileHeight = tileSize.height;

		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("pd_sprites.plist");
		m_pSpriteNodes = SpriteBatchNode::create("pd_sprites.pvr.ccz");
		this->addChild(m_pSpriteNodes);

		m_pHero = Hero::create();
		m_pHero->setPosition( m_origin + Point(100, 100) );
		m_pHero->runIdleAction();
		m_pHero->setLocalZOrder(m_fScreenHeight - m_pHero->getPositionY());
		m_pHero->setAttack(5);
		m_pHero->setHP(100);
		m_pHero->onDeadCallback = CC_CALLBACK_0(GameLayer::onHeroDead, this, m_pHero);
		m_pHero->attack = CC_CALLBACK_0(GameLayer::onHeroAttack, this);
		m_pHero->stop = CC_CALLBACK_0(GameLayer::onHeroStop, this);
		m_pHero->walk = CC_CALLBACK_2(GameLayer::onHeroWalk, this);
		m_pHero->jump = CC_CALLBACK_2(GameLayer::onHeroJump, this);

		Sprite *pBloodSprite = Sprite::create("blood.png");
		this->m_pBlood = ProgressTimer::create(pBloodSprite);
		this->m_pBlood->setType(ProgressTimer::Type::BAR);
		this->m_pBlood->setMidpoint(Point(0, 0));
		this->m_pBlood->setBarChangeRate(Point(1, 0));
		this->m_pBlood->setAnchorPoint(Point(0, 1));
		this->m_pBlood->setPosition(m_origin + Point(2, m_fScreenHeight - 10));
		this->m_pBlood->setPercentage(100);

		this->m_pBloodBg = ProgressTimer::create(Sprite::create("bloodBg.png"));
		this->m_pBloodBg->setType(ProgressTimer::Type::BAR);
		this->m_pBloodBg->setMidpoint(Point(0, 0));
		this->m_pBloodBg->setBarChangeRate(Point(1, 0));
		this->m_pBloodBg->setAnchorPoint(Point(0, 1));
		this->m_pBloodBg->setPosition(this->m_pBlood->getPosition());
		this->m_pBloodBg->setPercentage(100);

		this->addChild(m_pBloodBg, 100);
		this->addChild(m_pBlood, 100);


		m_pSpriteNodes->addChild(m_pHero);

		//CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(PATH_BG_MUSIC, true);
		//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(PATH_HERO_TALK_EFFECT);

		this->scheduleUpdate();

		ret = true;
	} while(0);

	return ret;
}

void GameLayer::exitApp(Ref* pSender)
{
	Director::getInstance()->end();
}


void GameLayer::onHeroWalk(Point direction, float distance)
{
	if(m_pHero->isLive())
	{
		m_pHero->setFlippedX(direction.x < 0 ? true : false);
		m_pHero->runWalkAction();

		//Point velocity;
		if (direction.x > 0)
		{
			m_pHero->setVelocity(distance < 78 ? 1 : 3);
			m_pHero->setDirection(Point(1.f, 0.f));
		}
		else
		{
			m_pHero->setVelocity(distance < 78 ? 1 : 3);
			m_pHero->setDirection(Point(-1.f, 0.f));
		}		
	}
}

void GameLayer::onHeroJump(Point direction, float distance)
{
	if (m_pHero->isLive())
	{
		m_pHero->setFlippedX(direction.x < 0 ? true : false);
		m_pHero->runJumpAction();

		m_pHero->setVelocity(distance < 78 ? 1 : 3);
		m_pHero->setDirection(direction);
	}
}

void GameLayer::onHeroAttack()
{

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
}

void GameLayer::updateHero(float dt)
{
	if(m_pHero->getCurrActionState() == ACTION_STATE_WALK || m_pHero->getCurrActionState() == ACTION_STATE_JUMP)
	{
		float halfHeroFrameHeight = (m_pHero->getSpriteFrame()->getRect().size.height) / 2;
		Point expectP = m_pHero->getPosition() + m_pHero->getVelocity() * m_pHero->getDirection();
		Point actualP = expectP;
		//can not walk on the wall or out of map
		if(expectP.y < halfHeroFrameHeight || expectP.y > (m_fTileHeight * 3 + halfHeroFrameHeight) )
		{
			actualP.y = m_pHero->getPositionY();
		}
		float mapWidth = m_pTiledMap->getContentSize().width;
		float halfWinWidth = m_fScreenWidth / 2;
		float halfHeroFrameWidth = (m_pHero->getSpriteFrame()->getRect().size.width) / 2;
		if(expectP.x > halfWinWidth && expectP.x <= (mapWidth - halfWinWidth))
		{
			this->setPositionX(this->getPositionX() - (m_pHero->getVelocity() * m_pHero->getDirection()).x);
			this->m_pBlood->setPositionX(this->m_pBlood->getPositionX() + (m_pHero->getVelocity() * m_pHero->getDirection()).x);
			this->m_pBloodBg->setPositionX(this->m_pBloodBg->getPositionX() + (m_pHero->getVelocity() * m_pHero->getDirection()).x);
			this->m_pCloseItem->setPositionX(this->m_pCloseItem->getPositionX() + (m_pHero->getVelocity() * m_pHero->getDirection()).x);
		}else if(expectP.x < halfHeroFrameWidth || expectP.x >= mapWidth - halfHeroFrameWidth)
		{
			actualP.x = m_pHero->getPositionX();
		}
		m_pHero->setPosition(actualP);
		m_pHero->setLocalZOrder(m_fScreenHeight - m_pHero->getPositionY());
	}
}

void GameLayer::updateBullet(float dt)
{

}
