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
	virtual void update(float dt);

	CREATE_FUNC(Hero);

	CC_SYNTHESIZE(int, m_curSkillState, CurSkillState);
	CC_SYNTHESIZE(cocos2d::Point, m_skillActivePos, SkillActivePos);
	CC_SYNTHESIZE(float, m_skillState1Speed, SkillState1Speed);
	CC_SYNTHESIZE(float, m_skillState2Speed, SkillState2Speed);
	CC_SYNTHESIZE(float, m_skillState1CDTime, SkillState1CDTime);
	CC_SYNTHESIZE(float, m_skillState2CDTime, SkillState2CDTime);
	CC_SYNTHESIZE(float, m_curSkillCDTime,	CurSkillCDTime);
};


#endif