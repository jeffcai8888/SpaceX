#pragma once
#include <cocos2d.h>
class Bullet :
	public cocos2d::Sprite
{
public:
	Bullet();
	virtual ~Bullet();

	CC_SYNTHESIZE(float, m_fVelocity, Velocity);
	CC_SYNTHESIZE(cocos2d::Point, m_fDirection, Direction);
	CC_SYNTHESIZE(float, m_fDisappearDistance, DisappearDistance);
	CC_SYNTHESIZE(unsigned int, m_gravity, Gravity);
	CC_SYNTHESIZE(unsigned int, m_power, Power);
};

