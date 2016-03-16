#include <cocos2d.h>
#include <Box2D/Box2D.h>
#include "Macro.h"
#include "Princess.h"
#include "Bullet.h"

USING_NS_CC;


Princess::Princess()
{}

Princess::~Princess()
{}

bool Princess::init()
{
	bool ret = false;
	do {
		this->initWithSpriteFrameName("QW_idle_01.png");

		Animation *pIdleAnim = this->createAnimation("QW_idle_%02d.png", 4, 5);
		this->setIdleAction(RepeatForever::create(Animate::create(pIdleAnim)));

		Animation *pWalkAnim = this->createAnimation("QW_walk_%02d.png", 12, 15);
		this->setWalkAction(RepeatForever::create(Animate::create(pWalkAnim)));

		Animation *pJumpAnim = this->createAnimation("QW_jump_%02d.png", 6, 15);;
		this->setJumpAction(Sequence::create(Animate::create(pJumpAnim), nullptr));

		Animation *pJumpRotateAnim = this->createAnimation("QW_jump_06.png", 1, 30);
		this->setJump2Action(Sequence::create(Animate::create(pJumpRotateAnim), RepeatForever::create(Animate::create(pJumpAnim)), nullptr));

		Animation *pDown1Anim = this->createAnimation("QW_down_%02d.png", 2, 15);
		Animation *pDown2Anim = this->createAnimation("QW_down_02.png", 1, 15);
		this->setDownAction(Sequence::create(Animate::create(pDown1Anim), RepeatForever::create(Animate::create(pDown2Anim)), nullptr));

		Animation *pWalkFireAnim = this->createAnimation("QW_walk_%02d.png", 12, 15);
		this->setWalkFireAction(RepeatForever::create(Animate::create(pWalkFireAnim)));

		Animation *pIdleFireAnim = this->createAnimation("QW_idle_%02d.png", 4, 5);
		this->setIdleFireAction(RepeatForever::create(Animate::create(pIdleFireAnim)));

		auto body = PhysicsBody::create();
		body->setGravityEnable(false);
		body->setRotationEnable(false);
		const PhysicsMaterial m(1.0f, 0.f, 0.f);
		auto shape = PhysicsShapeBox::create(Size(this->getContentSize().width / 6, this->getContentSize().height / 4), m, Vec2(0.f, -this->getContentSize().height / 2.5));
		body->addShape(shape);
		body->setCategoryBitmask(PC_Hero);
		body->setContactTestBitmask(PC_Ground | PC_Box | PC_Slope);
		body->setCollisionBitmask(PC_Ground | PC_Box | PC_Slope);
		this->setPhysicsBody(body);
		ret = true;
	} while(0);

	return ret;
}



