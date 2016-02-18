#include "Macro.h"
#include "Ground.h"

USING_NS_CC;

Ground::Ground()
{
}


Ground::~Ground()
{
}

void Ground::initPhysics(Size size, Point pos)
{
	const PhysicsMaterial m(1.f, 0.f, 1.f);
	auto body = PhysicsBody::createBox(size, m);
	body->setTag(0);
	body->setCategoryBitmask(PC_Ground);
	body->setContactTestBitmask(PC_Hero | PC_Bullet);
	body->setCollisionBitmask(PC_Hero | PC_Bullet);
	body->setDynamic(false);
	this->setPosition(pos);
	this->setPhysicsBody(body);
}
