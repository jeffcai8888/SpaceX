#ifndef _SPACEX_CLASSES_GAMELAYER_H_
#define _SPACEX_CLASSES_GAMELAYER_H_

#include <Box2D/Box2D.h>
#include <cocos2d.h>


class BaseSprite;
class Hero;
class Bullet;
class GameData;
class Foresight;
class Bomb;

class GameLayer : public cocos2d::Layer
{
public:
	GameLayer();
	virtual ~GameLayer();

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	virtual void update(float dt);
	virtual void updateBullet(float dt);
	virtual void updatePhysicsWorld(float dt);
	virtual void updatePlayer(float dt);

	void callBack(cocos2d::Ref *pSender);

	BaseSprite* getPlayer(int index);

	//CC_SYNTHESIZE_READONLY(BaseSprite*, m_pHero, Hero);
	CC_SYNTHESIZE_READONLY(cocos2d::Size, m_TiledMapSize, TiledMapSize);

	CREATE_FUNC(GameLayer);

protected:
	cocos2d::TMXTiledMap *m_pTiledMap;
	cocos2d::TMXLayer*	m_pMetaLayer;
    
	cocos2d::Point m_origin;

	cocos2d::Vector<Bullet *> m_vecBullets;

	cocos2d::Vector<cocos2d::EventListener *> m_vecEventListener;

    BaseSprite* m_pTarget;
	


	Bullet* getUnusedBullet();
	void setViewPointCenter();
	void importGroundData(cocos2d::TMXTiledMap*);
	void removeAllEventListener();
	void removeAllBullets();
	BaseSprite* createHero(int, cocos2d::Point, int);
    BaseSprite* getNearestEnemy();
	void explodeEnemy(Bomb* bomb);
};

#endif