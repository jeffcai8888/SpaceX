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

	CC_SYNTHESIZE(int, m_curSkillState, CurSkillState);
	CC_SYNTHESIZE(cocos2d::Point, m_skillActivePos, SkillActivePos);
};


#endif