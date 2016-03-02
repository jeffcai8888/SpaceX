#ifndef _SPACEX_CLASSES_FORESIGHT_H_
#define _SPACEX_CLASSES_FORESIGHT_H_

#include <cocos2d.h>

class Hero;
class Foresight : public cocos2d::Sprite
{
public:
	Foresight();
	virtual ~Foresight();

	virtual bool init();
	void update(float dt);

	CREATE_FUNC(Foresight);

	CC_SYNTHESIZE(float, m_fVelocity, Velocity);
	CC_SYNTHESIZE(cocos2d::Point, m_fDirection, Direction);
	
private:
};

#endif