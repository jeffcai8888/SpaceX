#ifndef _SPACEX_CLASSES_BOMB_H_
#define _SPACEX_CLASSES_BOMB_H_

#include <cocos2d.h>

class Hero;
class Bomb : public cocos2d::Sprite
{
public:
	Bomb();
	virtual ~Bomb();

	virtual bool init();
	void update(float dt);
	void launch(Hero *);
	void start();

	CREATE_FUNC(Bomb);

	CC_SYNTHESIZE(float, m_fVelocity, Velocity);
	CC_SYNTHESIZE(cocos2d::Point, m_fDirection, Direction);
	CC_SYNTHESIZE(float, m_gravity, Gravity);
	CC_SYNTHESIZE(unsigned int, m_power, Power);
	CC_SYNTHESIZE(float, m_range, Range);
	CC_SYNTHESIZE(bool, m_isActive, IsActive);
	CC_SYNTHESIZE(bool, m_isStart, IsStart);
	CC_SYNTHESIZE(Hero*, m_owner, Owner);
	CC_SYNTHESIZE(float, m_startTime, StartTime);

private:
	void reset();
	void explode();
};

#endif