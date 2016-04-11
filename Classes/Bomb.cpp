#include "Macro.h"
#include "Bomb.h"
#include "BaseSprite.h"

USING_NS_CC;

Bomb::Bomb()
{
}


Bomb::~Bomb()
{
}

bool Bomb::init()
{
	bool ret = false;
	do 
	{
		this->initWithSpriteFrameName("bullet.png");
		auto body = PhysicsBody::create();
		auto shape = PhysicsShapeBox::create(Size(this->getContentSize().width, this->getContentSize().height), PHYSICSSHAPE_MATERIAL_DEFAULT);
		body->addShape(shape);
		body->setCategoryBitmask(PC_Bomb);
        body->setContactTestBitmask(PC_Ground | PC_Box | PC_Slope);
		body->setCollisionBitmask(PC_Ground | PC_Box | PC_Slope);
		this->setPhysicsBody(body);
		ret = true;
	} while (false);
	return ret;
}

void Bomb::update(float dt)
{
	if (this->m_isActive)
	{
		float x = this->getPhysicsBody()->getVelocity().x;
		float y = this->getPhysicsBody()->getVelocity().y;
		y += m_gravity * dt;
		CCLOG("m_gravity = %f y = %f", this->m_gravity, y);
		this->getPhysicsBody()->setVelocity(Vec2(x, y));

		m_launchTime += dt;
		if (m_launchTime >= m_fDisappearTime)
		{
			m_launchTime = 0.f;
			this->m_isActive = false;
			EventCustom event("bullet_disappear");
			event.setUserData(this);
			_eventDispatcher->dispatchEvent(&event);
		}
	}
}

void Bomb::launch(BaseSprite* pHero)
{
	this->m_isActive = true;
	Point pos = pHero->getShootPosition();
	this->setPosition(pos);	
	this->m_fVelocity = pHero->getBulletLaunchVelocity();
	this->m_fDirection = pHero->getShootDirection().rotateByAngle(Vec2(0.f, 0.f), rotation);
	this->m_power = pHero->getBullletPower();
	this->m_gravity = pHero->getBulletGravity();
	this->m_ownerTag = pHero->getTag();
	this->getPhysicsBody()->setVelocity(m_fVelocity * m_fDirection);
	m_launchTime = 0.f;
}
