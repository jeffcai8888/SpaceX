#include "Bullet.h"
#include "BaseSprite.h"

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
		this->initWithSpriteFrameName("bullet.png");
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
		float x = this->getPhysicsBody()->getVelocity().x;
		float y = this->getPhysicsBody()->getVelocity().y;
		y += this->m_owner->getBulletGravity() * dt;
		this->getPhysicsBody()->setVelocity(Vec2(x, y));

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

void Bullet::launch(BaseSprite* pHero)
{
	this->m_isActive = true;
	this->m_owner = pHero;
	Point pos = pHero->getPosition();
	if(pHero->isFlippedX())
		this->setPosition(pos + Vec2(-15.f, -20.f));
	else
		this->setPosition(pos + Vec2(15.f, -20.f));
	this->m_startPostion = pos;
	this->m_fVelocity = pHero->getBulletLaunchVelocity();
	this->m_fDisappearTime = pHero->getBulletDisappearTime();
	this->m_fDirection = pHero->getShootDirection().rotateByAngle(Vec2(0.f, 0.f), CC_DEGREES_TO_RADIANS((int)(-rand_0_1() * pHero->getBullletAngle())));
	this->m_power = pHero->getBullletPower();
	this->getPhysicsBody()->setVelocity(Vec2(0.f, 0.f));
	this->getPhysicsBody()->applyImpulse(m_fVelocity * m_fDirection + pHero->getPhysicsBody()->getVelocity());
	m_launchTime = 0.f;
}
