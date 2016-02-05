#ifndef _GAME_LAYER_H_
#define _GAME_LAYER_H_

#include <cocos2d.h>
#include <Box2D/Box2D.h>

class BaseSprite;
class Hero;
class Bullet;
class GLESDebugDraw;

class GameLayer : public cocos2d::Layer
{
public:
	GameLayer();
	~GameLayer();

	virtual bool init();

	void onHeroWalk(cocos2d::Point direction, float distance);
	void onHeroJump(cocos2d::Point direction, float distance);
	void onHeroAttack();
	void onHeroStop();
	void onHeroDead(BaseSprite *pTarget);

	void update(float dt);
	void updateHero(float dt);
	void updateBullet(float dt);



	void exitApp(cocos2d::Ref* pSender);

	void callBack(cocos2d::Ref *pSender);

	CC_SYNTHESIZE_READONLY(Hero*, m_pHero, Hero);
	CC_SYNTHESIZE(b2World*, m_pWorld, PhyWorld);

	CREATE_FUNC(GameLayer);
protected:
	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);

private:
	cocos2d::TMXTiledMap *m_pTiledMap;
	float m_fTileWidth;
	float m_fTileHeight;

	float m_fScreenWidth;
	float m_fScreenHeight;

	float m_shootTime;
	cocos2d::Point m_origin;

	cocos2d::Vector<Bullet *> m_vecBullets;
	cocos2d::SpriteBatchNode *m_pSpriteNodes;

	cocos2d::ProgressTimer *m_pBlood;
	cocos2d::ProgressTimer *m_pBloodBg;
	cocos2d::MenuItemImage *m_pCloseItem;

	GLESDebugDraw *m_pDebugDraw;

	Bullet* getUnusedBullet();
};

#endif