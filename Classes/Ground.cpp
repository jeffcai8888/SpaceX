#include "Macro.h"
#include "Ground.h"
#include "PhysicsWorldManager.h"

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
			CC_BREAK_IF(!PhysicsSprite::init());
			ret = true;
		} while (false);
		return ret;
	} while (false);
	return ret;
}

void Ground::initPhysics(Size size, Point pos, int rotation)
{
	/*const PhysicsMaterial m(1.f, 0.f, 0.f);
	auto body = PhysicsBody::createBox(size, m);
	body->setTag(0);
	body->setCategoryBitmask(PC_Ground);
	body->setContactTestBitmask(PC_Hero | PC_Bullet);
	body->setCollisionBitmask(PC_Hero | PC_Bullet);
	body->setDynamic(false);*/

	b2BodyDef bd;
	bd.type = b2_staticBody;
	auto body = PhysicsWorldManager::getInstance()->getWorld()->CreateBody(&bd);
	this->setB2Body(body);
	this->setPTMRatio(PTM_RADIO);
	this->ignoreAnchorPointForPosition(false);
	this->setContentSize(size);
	this->setAnchorPoint(Vec2(0.f, 1.f));
	pos.y = pos.y + size.height;
	this->setPosition(pos);
	this->setRotation(rotation);

	b2PolygonShape polygon;
	polygon.SetAsBox(this->getContentSize().width / 2 / PTM_RADIO, this->getContentSize().height / 2 / PTM_RADIO);

	b2FixtureDef fd;
	fd.shape = &polygon;
	fd.density = 1.0f;
	fd.friction = 0.0f;
	fd.restitution = 0.0f;
	body->CreateFixture(&fd);


	
}
