#include "Bullet.h"
#include "Hero.h"

USING_NS_CC;

Bullet::Bullet()
	:m_launchTime(0.f)
{
}


Bullet::~Bullet()
{
}

bool Bullet::init()
{
	bool ret = false;
	do 
	{
		this->initWithFile("bullet.jpg");
		auto body = PhysicsBody::create();
		auto shape = PhysicsShapeBox::create(Size(this->getContentSize().width, this->getContentSize().height), PHYSICSSHAPE_MATERIAL_DEFAULT);
		body->addShape(shape);
		body->setCategoryBitmask(0x02);
        body->setContactTestBitmask(0x4);
		body->setCollisionBitmask(0x04);
		this->setPhysicsBody(body);
		ret = true;
	} while (false);
	return ret;
}

void Bullet::update(float dt)
{
	if (this->m_isActive)
	{
		Point curPos = this->getPosition();

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

void Bullet::launch(Hero* pHero)
{
	this->m_isActive = true;
	this->m_owner = pHero;
	Point pos = pHero->getPosition();
	this->setPosition(pos);
	this->m_startPostion = pos;
	this->m_fVelocity = pHero->getBulletLaunchVelocity();
	this->m_fDisappearTime = pHero->getBulletDisappearTime();
	this->m_fDirection = pHero->getShootDirection().rotateByAngle(Vec2(0.f, 0.f), CC_DEGREES_TO_RADIANS((int)(-rand_0_1() * pHero->getBullletAngle())));
	this->m_power = pHero->getBullletPower();
	this->getPhysicsBody()->setVelocity(Vec2(0.f, 0.f));
	this->getPhysicsBody()->applyImpulse(m_fVelocity * m_fDirection + pHero->getPhysicsBody()->getVelocity());
	m_launchTime = 0.f;
}
