#include <cocos2d.h>
#include "Macro.h"
#include "Gunner.h"
#include "Bullet.h"
#include "JsonParser.h"

USING_NS_CC;


Gunner::Gunner()
{}

Gunner::~Gunner()
{}

bool Gunner::init()
{
	bool ret = false;
	do {
		this->initWithSpriteFrameName("BS_idle_01.png");

		Animation *pIdleAnim = this->createAnimation("BS_idle_%02d.png", 12, 15);
		this->setIdleAction(RepeatForever::create(Animate::create(pIdleAnim)));

		Animation *pWalkAnim = this->createAnimation("BS_walk_%02d.png", 12, 15);
		this->setWalkAction(RepeatForever::create(Animate::create(pWalkAnim)));

		Animation *pJumpAnim = this->createAnimation("BS_jump_%02d.png", 3, 15);
		Animation *pRollAnim = this->createAnimation("BS_roll_%02d.png", 8, 20);
		this->setJumpAction(Sequence::create(Animate::create(pJumpAnim), nullptr));
		this->setJump2Action(Sequence::create(Animate::create(pRollAnim), nullptr));

		Animation *pDown1Anim = this->createAnimation("BS_down_%02d.png", 2, 15);
		Animation *pDown2Anim = this->createAnimation("BS_down_03.png", 1, 15);
		this->setDownAction(Sequence::create(Animate::create(pDown1Anim), RepeatForever::create(Animate::create(pDown2Anim)), nullptr));

		Animation *pWalkFireAnim = this->createAnimation("BS_walk_%02d.png", 12, 15);
		this->setWalkFireAction(RepeatForever::create(Animate::create(pWalkFireAnim)));

		Animation *pIdleFireAnim = this->createAnimation("BS_idle_%02d.png", 4, 5);
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

		JsonParser* parser = JsonParser::createWithFile("Gunner.json");
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
			}
		}

        ret = true;
	} while(0);

	return ret;
}



