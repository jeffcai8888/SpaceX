#ifndef _SPACEX_CLASSES_BOMB_H_
#define _SPACEX_CLASSES_BOMB_H_

#include <cocos2d.h>

class BaseSprite;
class Bomb : public cocos2d::Sprite
{
public:
	Bomb();
	virtual ~Bomb();

	virtual bool init();
	void update(float dt);
	void launch(BaseSprite *);

	CREATE_FUNC(Bomb);

	CC_SYNTHESIZE(float, m_fVelocity, Velocity);
	CC_SYNTHESIZE(cocos2d::Point, m_fDirection, Direction);
	CC_SYNTHESIZE(float, m_gravity, Gravity);
	CC_SYNTHESIZE(unsigned int, m_power, Power);
	CC_SYNTHESIZE(int, m_ownerTag, OwnerTag);
};

#endif