#include <cocos2d.h>
#include <Box2D/Box2D.h>
#include "Macro.h"
#include "Gunner.h"
#include "Bullet.h"

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

		Animation *pJumpAnim = this->createAnimation("BS_jump_%02d.png", 3, 15);;
		this->setJumpAction(Sequence::create(Animate::create(pJumpAnim), nullptr));

		Animation *pJumpRotateAnim = this->createAnimation("BS_idle_01.png", 1, 30);
		this->setJump2Action(Sequence::create(Animate::create(pJumpRotateAnim), RotateBy::create(0.2f, 360), RepeatForever::create(Animate::create(pJumpAnim)), nullptr));

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
		auto shape = PhysicsShapeBox::create(Size(this->getContentSize().width / 4, this->getContentSize().height / 2), m, Vec2(0.f, -this->getContentSize().height / 4));
		body->addShape(shape);
		body->setCategoryBitmask(PC_Hero);
		body->setContactTestBitmask(PC_Ground);
		body->setCollisionBitmask(PC_Ground);
		this->setPhysicsBody(body);
		ret = true;
	} while(0);

	return ret;
}



