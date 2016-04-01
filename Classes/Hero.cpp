#include <cocos2d.h>
#include <Box2D/Box2D.h>
#include "Macro.h"
#include "Hero.h"
#include "Bullet.h"

USING_NS_CC;


Hero::Hero()
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

		Animation *pJumpAnim = this->createAnimation("SQ_jump_%02d.png", 5, 15);;
		this->setJumpAction(Sequence::create(Animate::create(pJumpAnim), nullptr));

		Animation *pJumpRotateAnim = this->createAnimation("SQ_jump02.png", 1, 30);
		this->setJump2Action(Sequence::create(Animate::create(pJumpRotateAnim), RotateBy::create(0.2f, 360), RepeatForever::create(Animate::create(pJumpAnim)), nullptr));

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



