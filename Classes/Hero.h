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
	void exploreBomb();

	CREATE_FUNC(Hero);

	CC_SYNTHESIZE(int, m_curSkillState, CurSkillState);
	CC_SYNTHESIZE(cocos2d::Point, m_skillActivePos, SkillActivePos);
	CC_SYNTHESIZE(float, m_skillState1Speed, SkillState1Speed);
	CC_SYNTHESIZE(float, m_skillState2Speed, SkillState2Speed);
	CC_SYNTHESIZE(float, m_skillState1CDTime, SkillState1CDTime);
	CC_SYNTHESIZE(float, m_skillState2CDTime, SkillState2CDTime);
	CC_SYNTHESIZE(float, m_skillState1LastTime, SkillState1LastTime);
	CC_SYNTHESIZE(float, m_skillState2LastTime, SkillState2LastTime);
	CC_SYNTHESIZE(float, m_bombSpeed, BombSpeed);
	CC_SYNTHESIZE(float, m_bombRange, BombRange);
	CC_SYNTHESIZE(float, m_bombStartTime, BombStartTime);
	CC_SYNTHESIZE(float, m_bombCDTime, BombCDTime);
	CC_SYNTHESIZE(int,	 m_bombPower, BombPower);
	CC_SYNTHESIZE(float, m_fBombGravity, BombGravity);
	CC_SYNTHESIZE(float, m_curSkillLastTime, CurSkillLastTime);
	CC_SYNTHESIZE(float, m_curSkillCDTime,	CurSkillCDTime);
	CC_SYNTHESIZE(bool,  m_isThrowBomb, IsThrowBomb);
	CC_SYNTHESIZE(bool,  m_isBombExplore, IsBombExplore);
};


#endif