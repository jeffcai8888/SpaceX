#ifndef _SPACEX_CLASSES_HERO_H_
#define _SPACEX_CLASSES_HERO_H_

#include "BaseSprite.h"
class Bullet;
class Bomb;
class Hero : public BaseSprite
{
public:
	Hero();
	~Hero();

	bool init();
	virtual void update(float dt);
	virtual void activeSkill1();
	virtual void activeSkill2();

	void exploreBomb();
	virtual bool isInSplash() {
		return m_bIsInSplash;
	}

	CREATE_FUNC(Hero);

	CC_SYNTHESIZE(int, m_curSkillState, CurSkillState);
	CC_SYNTHESIZE(cocos2d::Point, m_skillActivePos, SkillActivePos);
	CC_SYNTHESIZE(float, m_skillState1Speed, SkillState1Speed);
	CC_SYNTHESIZE(float, m_skillState2Speed, SkillState2Speed);
	CC_SYNTHESIZE(float, m_skillState1CDTime, SkillState1CDTime);
	CC_SYNTHESIZE(float, m_skillState2CDTime, SkillState2CDTime);
	CC_SYNTHESIZE(float, m_skillState1LastTime, SkillState1LastTime);
	CC_SYNTHESIZE(float, m_skillState2LastTime, SkillState2LastTime);
	
	CC_SYNTHESIZE(float, m_curSkillLastTime, CurSkillLastTime);
	CC_SYNTHESIZE(float, m_curSkillCDTime,	CurSkillCDTime);
	CC_SYNTHESIZE(bool,  m_isThrowBomb, IsThrowBomb);
	CC_SYNTHESIZE(bool, m_bIsInSplash, IsInSplash);
	CC_SYNTHESIZE(std::string, m_bombType, BombType);

	CC_SYNTHESIZE(cocos2d::Sprite*, m_pSkillStartPos, SkillStartPos);
	CC_SYNTHESIZE(Bomb*, m_pBomb, Bomb);
};


#endif