#ifndef _HERO_H_
#define _HERO_H_

#include "BaseSprite.h"
class Bullet;
class Hero : public BaseSprite
{
public:
	Hero();
	~Hero();

	bool init();
	bool initPhysics(b2World*);

	CREATE_FUNC(Hero);

	std::function<void(cocos2d::Point, float)> walk;
	std::function<void(cocos2d::Point, float)> jump;
	std::function<void(void)> stop;	
};


#endif