#ifndef _SPACEX_CLASSES_GAMELAYER_H_
#define _SPACEX_CLASSES_GAMELAYER_H_

#include <cocos2d.h>


class BaseSprite;
class Hero;
class Bullet;
class GameData;
class Foresight;

class GameLayer : public cocos2d::Layer
{
public:
	GameLayer();
	virtual ~GameLayer();

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	void onHeroWalk(float velocity);
	void onHeroJump(float velocity);
	void onHeroAttack();
	void onHeroStop();
	void onHeroDead(BaseSprite *pTarget);

	virtual void update(float dt);
	virtual void updateHero(float dt);
	virtual void updateBullet(float dt);
	virtual void updateForesight(float dt);
	void updatePhysicsWorld(float dt);


	void callBack(cocos2d::Ref *pSender);

	CC_SYNTHESIZE_READONLY(Hero*, m_pHero, Hero);
	CC_SYNTHESIZE_READONLY(Foresight*, m_pForesight, Foresight);
	CC_SYNTHESIZE_READONLY(cocos2d::Size, m_TiledMapSize, TiledMapSize);

	CREATE_FUNC(GameLayer);

protected:
	cocos2d::TMXTiledMap *m_pTiledMap;
	cocos2d::TMXLayer*	m_pMetaLayer;

	float m_shootTime;
	cocos2d::Point m_origin;

	cocos2d::Vector<Bullet *> m_vecBullets;
	cocos2d::SpriteBatchNode *m_pSpriteNodes;

	cocos2d::Vector<cocos2d::EventListener *> m_vecEventListener;

	Bullet* getUnusedBullet();
	void setViewPointCenter();
	void importGroundData(cocos2d::TMXTiledMap*);
	void removeAllEventListener();
	void initForesight();
};

#endif