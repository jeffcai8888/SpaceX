#ifndef _SPACEX_CLASSES_GAMELAYER_H_
#define _SPACEX_CLASSES_GAMELAYER_H_

#include <cocos2d.h>

class BaseSprite;
class Hero;
class Bullet;

class GameLayer : public cocos2d::Layer
{
public:
	GameLayer();
	~GameLayer();

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	void onHeroWalk(float velocity);
	void onHeroJump(float velocity);
	void onHeroAttack();
	void onHeroStop();
	void onHeroDead(BaseSprite *pTarget);

	void update(float dt);
	void updateHero(float dt);
	void updateBullet(float dt);


	void callBack(cocos2d::Ref *pSender);

	CC_SYNTHESIZE_READONLY(Hero*, m_pHero, Hero);

	CREATE_FUNC(GameLayer);

private:
	cocos2d::TMXTiledMap *m_pTiledMap;
	cocos2d::TMXLayer*	m_pMetaLayer;

	float m_fScreenWidth;
	float m_fScreenHeight;

	float m_shootTime;
	cocos2d::Point m_origin;

	cocos2d::Vector<Bullet *> m_vecBullets;
	cocos2d::SpriteBatchNode *m_pSpriteNodes;

	cocos2d::Vector<cocos2d::EventListener *> m_vecEventListener;

	Bullet* getUnusedBullet();
	void setViewPointCenter(cocos2d::Point position);
	void importGroundData(cocos2d::TMXTiledMap*);
	void removeAllEventListener();
};

#endif