#include "Ground.h"

using namespace cocos2d;

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
	body->setCategoryBitmask(0x04);
	body->setContactTestBitmask(0x01);
	body->setCollisionBitmask(0x03);
	body->setDynamic(false);
	this->setPosition(pos);
	this->setPhysicsBody(body);
}
