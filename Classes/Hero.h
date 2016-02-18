#ifndef _SPACEX_CLASSES_HERO_H_
#define _SPACEX_CLASSES_HERO_H_

#include "BaseSprite.h"
class Bullet;
class Hero : public BaseSprite
{
public:
	Hero();
	~Hero();

	bool init();

	CREATE_FUNC(Hero);

	std::function<void(cocos2d::Vec2)> walk;
	std::function<void(cocos2d::Vec2)> jump;
	std::function<void(void)> stop;	
};


#endif