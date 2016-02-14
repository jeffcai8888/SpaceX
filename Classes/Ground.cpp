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
	body->setDynamic(false);
	this->setPosition(pos);
	this->setPhysicsBody(body);
}
