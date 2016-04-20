#include <cocos2d.h>
#include "Macro.h"
#include "Hero.h"
#include "Bullet.h"
#include "Bomb.h"
#include "JsonParser.h"

USING_NS_CC;


Hero::Hero()
:m_curSkillState(0)
,m_curSkillCDTime(-1.f)
,m_curSkillLastTime(-1.f)
,m_isThrowBomb(false)
,m_bIsInSplash(false)
{}

Hero::~Hero()
{}

bool Hero::init()
{
	bool ret = false;
	do {
		this->initWithSpriteFrameName("SQ_idle_01.png");

		Animation *pIdleAnim = this->createAnimation("SQ_idle_%02d.png", 4, 5);
		this->setIdleAction(RepeatForever::create(Animate::create(pIdleAnim)));

		Animation *pWalkAnim = this->createAnimation("SQ_walk_%02d.png", 12, 15);
		this->setWalkAction(RepeatForever::create(Animate::create(pWalkAnim)));

		Animation *pJumpAnim = this->createAnimation("SQ_jump_%02d.png", 5, 15);
		Animation *pRollAnim = this->createAnimation("SQ_roll_%02d.png", 8, 20);
		this->setJumpAction(Sequence::create(Animate::create(pJumpAnim), nullptr));
		this->setJump2Action(Sequence::create(Animate::create(pRollAnim), nullptr));

		Animation *pDown1Anim = this->createAnimation("SQ_down_%02d.png", 8, 15);
		Animation *pDown2Anim = this->createAnimation("SQ_down_09.png", 1, 15);
		this->setDownAction(Sequence::create(Animate::create(pDown1Anim), RepeatForever::create(Animate::create(pDown2Anim)), nullptr));

		Animation *pWalkFireAnim = this->createAnimation("SQ_walkFire_%02d.png", 12, 15);
		this->setWalkFireAction(RepeatForever::create(Animate::create(pWalkFireAnim)));

		Animation *pIdleFireAnim = this->createAnimation("SQ_idleFire_%02d.png", 4, 5);
		this->setIdleFireAction(RepeatForever::create(Animate::create(pIdleFireAnim)));

		auto body = PhysicsBody::create();
		body->setGravityEnable(false);
		body->setRotationEnable(false);
		const PhysicsMaterial m(1.0f, 0.f, 0.f);
		auto shape0 = PhysicsShapeBox::create(Size(this->getContentSize().width / 5, this->getContentSize().height / 4), m, Vec2(0.f, -this->getContentSize().height /2.5));
        shape0->setTag(0);
        shape0->setCategoryBitmask(PC_Hero);
        shape0->setContactTestBitmask(PC_Ground | PC_Box | PC_Slope);
        shape0->setCollisionBitmask(PC_Ground | PC_Box | PC_Slope);
        body->addShape(shape0);
        auto shape1 = PhysicsShapeBox::create(Size(this->getContentSize().width / 5, this->getContentSize().height / 2), m, Vec2(0.f, -this->getContentSize().height / 4));
        shape1->setTag(1);
        shape1->setCategoryBitmask(PC_Damage);
        shape1->setContactTestBitmask(PC_Bullet);
        shape1->setCollisionBitmask(PC_Bullet);
		body->addShape(shape1);
		
		this->setPhysicsBody(body);

		JsonParser* parser = JsonParser::createWithFile("Hero.json");
		parser->decodeDebugData();
		auto list = parser->getList();
		for (auto& v : list)
		{
			ValueMap row = v.asValueMap();
			for (auto& pair : row)
			{
				if (pair.first.compare("WalkSpeed") == 0)
				{
					setWalkVelocity(pair.second.asFloat());
				}
				else if (pair.first.compare("JumpSpeed") == 0)
				{
					setJumpVelocity(pair.second.asFloat());
				}
				else if (pair.first.compare("Gravity") == 0)
				{
					setGravity(pair.second.asFloat());
				}
				else if (pair.first.compare("Skill1V") == 0)
				{
					setSkillState1Speed(pair.second.asFloat());
				}
				else if (pair.first.compare("Skill2V") == 0)
				{
					setSkillState2Speed(pair.second.asFloat());
				}
				else if (pair.first.compare("Skill1CD") == 0)
				{
					setSkillState1CDTime(pair.second.asFloat());
				}
				else if (pair.first.compare("Skill2CD") == 0)
				{
					setSkillState2CDTime(pair.second.asFloat());
				}
				else if (pair.first.compare("Skill1Time") == 0)
				{
					setSkillState1LastTime(pair.second.asFloat());
				}
				else if (pair.first.compare("Skill2Time") == 0)
				{
					setSkillState2LastTime(pair.second.asFloat());
				}
			}
		}

		setBulletType("Bullet1Config");
		setBombType("Bomb1Config");

		m_pLockMark = Sprite::createWithSpriteFrameName("lock.png");
		m_pLockMark->setPosition(getContentSize().width / 2, 50.f);
		m_pLockMark->setVisible(false);
		addChild(m_pLockMark);


		m_pRange = Sprite::createWithSpriteFrameName("range.png");
		m_pRange->setVisible(false);
		m_pRange->setPosition(Point(140.f, 25.f));
		addChild(m_pRange);

		ret = true;
	} while(0);

	return ret;
}

void Hero::activeSkill1()
{
	if (getCurSkillState() == 0)
	{
		setSkillActivePos(getPosition());
		if (isFlippedX())
		{
			getPhysicsBody()->setVelocity(Vec2(-getSkillState1Speed(), 0.f));
		}
		else
		{
			getPhysicsBody()->setVelocity(Vec2(getSkillState1Speed(), 0.f));
		}
		
		setCurSkillState(1);
		setCurSkillCDTime(getSkillState1CDTime());
		setCurSkillLastTime(getSkillState1LastTime());

		setIsInSplash(true);
		m_pSkillStartPos->setVisible(true);
		m_pSkillStartPos->setPosition(getPosition() + Vec2(0, -10.f));
	}
	else if (getCurSkillState() == 1)
	{
		if (isFlippedX())
		{
			getPhysicsBody()->setVelocity(Vec2(-getSkillState1Speed(), 0.f));
		}
		else
		{
			getPhysicsBody()->setVelocity(Vec2(getSkillState1Speed(), 0.f));
		}
		
		setCurSkillState(2);
		setIsInSplash(true);
		setCurSkillCDTime(getSkillState2CDTime());
		setCurSkillLastTime(getSkillState2LastTime());
	}
	else if (getCurSkillState() == 2)
	{
		setPosition(getSkillActivePos());
		setCurSkillState(0);
		//m_pSkill->setSpriteFrame("skill_flash1.png");
		m_pSkillStartPos->setVisible(false);
	}
}

void Hero::activeSkill2()
{
	setIsThrowBomb(true);
}

void Hero::update(float dt)
{
    BaseSprite::update(dt);
	if (m_curSkillState > 0)
	{
		if (m_curSkillCDTime > 0.f)
		{
			m_curSkillCDTime -= dt;
			if (m_curSkillCDTime < 0)
			{
				EventCustom event("heroSkillTimeOut");
				_eventDispatcher->dispatchEvent(&event);
				m_curSkillState = 0;
				m_curSkillCDTime = -1.f;
				m_pSkillStartPos->setVisible(false);
			}
		}
		
		if (m_curSkillLastTime > 0.f)
		{
			m_curSkillLastTime -= dt;
			if (m_curSkillLastTime <= 0.f)
			{
				if (getCurrActionState() == ACTION_STATE_WALK)
				{
					if(isFlippedX())
						getPhysicsBody()->setVelocity(-Vec2(getWalkVelocity(), 0.f));
					else
						getPhysicsBody()->setVelocity(Vec2(getWalkVelocity(), 0.f));
				}				
				else
					getPhysicsBody()->setVelocity(Vec2(0.f, 0.f));
				
				m_curSkillLastTime = -1.f;
				if (m_curSkillState == 1)
				{
					m_curSkillCDTime = m_skillState1CDTime;
				}
				else if (m_curSkillState == 2)
				{
					m_curSkillCDTime = m_skillState2CDTime;
				}
				m_bIsInSplash = false;
			}
		}		
	}

	if (getIsThrowBomb())
	{
		setIsThrowBomb(false);

		EventCustom event("throw_bomb");
		event.setUserData(this);
		_eventDispatcher->dispatchEvent(&event);
	}

	if (m_pBomb->getIsActive())
		m_pBomb->update(dt);
}



