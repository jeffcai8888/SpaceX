#include "Macro.h"
#include "Box.h"

USING_NS_CC;

Box::Box()
{
}


Box::~Box()
{
}

bool Box::init()
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

void Box::initPhysics(Size size, Point pos, int rotation)
{
	const PhysicsMaterial m(1.f, 0.f, 0.f);
	auto body = PhysicsBody::createBox(size, m);
	body->setCategoryBitmask(PC_Box);
	body->setContactTestBitmask(PC_Hero | PC_Bullet | PC_Bomb);
	body->setCollisionBitmask(PC_Hero | PC_Bullet | PC_Bomb);
	body->setDynamic(false);
	
	this->setContentSize(size);
	this->setAnchorPoint(Vec2(0.f, 1.f));
	pos.y = pos.y + size.height;
	this->setPosition(pos);
	this->setPhysicsBody(body);
	this->setRotation(rotation);
}
