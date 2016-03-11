#ifndef _SPACEX_CLASSES_HERO_H_
#define _SPACEX_CLASSES_HERO_H_

#include "BaseSprite.h"
class Bullet;
class b2Body;
class Hero : public BaseSprite
{
public:
	Hero();
	~Hero();

	bool init();

	CREATE_FUNC(Hero);

	std::function<void(float)> walk;
	std::function<void(float)> jump;
	std::function<void(void)> stop;	

	CC_SYNTHESIZE(float, m_walkVelocity, WalkVelocity);
	CC_SYNTHESIZE(float, m_jumpVelocity, JumpVelocity);
	CC_SYNTHESIZE(float, m_bulletDisappearTime, BulletDisappearTime);
	CC_SYNTHESIZE(float, m_bulletLaunchVelocity, BulletLaunchVelocity);
	CC_SYNTHESIZE(int,   m_bullletAngle, BullletAngle);
	CC_SYNTHESIZE(int,   m_bullletPower, BullletPower);
	CC_SYNTHESIZE(float, m_bulletInterval, BulletInterval);
	CC_SYNTHESIZE(int,	 m_ammoCapacity, AmmoCapacity);
	CC_SYNTHESIZE(int, m_maxAmmoCapacity, MaxAmmoCapacity);
};


#endif