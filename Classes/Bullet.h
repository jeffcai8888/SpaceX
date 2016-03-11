#ifndef _SPACEX_CLASSES_BULLET_H_
#define _SPACEX_CLASSES_BULLET_H_

#include <cocos2d.h>
#include "extensions/cocos-ext.h"

class Hero;
class Bullet : public cocos2d::extension::PhysicsSprite
{
public:
	Bullet();
	virtual ~Bullet();

	virtual bool init();
	void update(float dt);
	void launch(Hero *);

	CREATE_FUNC(Bullet);

	CC_SYNTHESIZE(float, m_fVelocity, Velocity);
	CC_SYNTHESIZE(cocos2d::Point, m_fDirection, Direction);
	CC_SYNTHESIZE(float, m_fDisappearTime, DisappearTime);
	CC_SYNTHESIZE(unsigned int, m_gravity, Gravity);
	CC_SYNTHESIZE(unsigned int, m_power, Power);
	CC_SYNTHESIZE(bool, m_isActive, IsActive);
	CC_SYNTHESIZE(Hero*,  m_owner, Owner);
	
private:
	cocos2d::Point m_startPostion;
	float m_launchTime;
};

#endif