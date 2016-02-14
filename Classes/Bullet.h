#pragma once
#include <cocos2d.h>

class Hero;
class Bullet : public cocos2d::Sprite
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
	CC_SYNTHESIZE(float, m_fDisappearDistance, DisappearDistance);
	CC_SYNTHESIZE(unsigned int, m_gravity, Gravity);
	CC_SYNTHESIZE(unsigned int, m_power, Power);
	CC_SYNTHESIZE_READONLY(bool, m_isActive, IsActive);
	CC_SYNTHESIZE(Hero*,  m_owner, Owner);
	
private:
	cocos2d::Point m_startPostion;
};

