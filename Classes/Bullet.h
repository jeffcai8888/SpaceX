#ifndef _SPACEX_CLASSES_BULLET_H_
#define _SPACEX_CLASSES_BULLET_H_

#include <cocos2d.h>

class BaseSprite;
class Bullet : public cocos2d::Sprite
{
public:
	Bullet();
	virtual ~Bullet();

	virtual bool init();
	void update(float dt);
	void launch(BaseSprite *);

	CREATE_FUNC(Bullet);

	CC_SYNTHESIZE(float, m_fVelocity, Velocity);
	CC_SYNTHESIZE(cocos2d::Point, m_fDirection, Direction);
	CC_SYNTHESIZE(float, m_fDisappearTime, DisappearTime);
	CC_SYNTHESIZE(unsigned int, m_gravity, Gravity);
	CC_SYNTHESIZE(unsigned int, m_power, Power);
	CC_SYNTHESIZE(bool, m_isActive, IsActive);
	CC_SYNTHESIZE(int, m_ownerTag, OwnerTag);
	
private:
	float m_launchTime;
};

#endif