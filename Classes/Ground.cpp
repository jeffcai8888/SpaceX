#include "Macro.h"
#include "Ground.h"

USING_NS_CC;

Ground::Ground()
{
}


Ground::~Ground()
{
}

bool Ground::init()
{
	bool ret = false;
	do
	{
		bool ret = false;
		do
		{
			CC_BREAK_IF(!Sprite::init());
			//this->initWithSpriteFrameName("Foresight.png");
			ret = true;
		} while (false);
		return ret;
	} while (false);
	return ret;
}

void Ground::initPhysics(Size size, Point pos, int rotation)
{
	const PhysicsMaterial m(1.f, 0.f, 0.f);
	auto body = PhysicsBody::createBox(size, m);
	body->setCategoryBitmask(PC_Ground);
	body->setContactTestBitmask(PC_Hero | PC_Bullet);
	body->setCollisionBitmask(PC_Hero | PC_Bullet);
	body->setDynamic(false);
	
	this->setContentSize(size);
	this->setAnchorPoint(Vec2(0.f, 1.f));
	pos.y = pos.y + size.height;
	this->setPosition(pos);
	this->setPhysicsBody(body);
	this->setRotation(rotation);
}
