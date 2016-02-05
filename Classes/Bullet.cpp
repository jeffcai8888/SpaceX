#include "Bullet.h"
#include "Hero.h"

using namespace cocos2d;

Bullet::Bullet()
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
		/*PhysicsMaterial materail(0.1f, 0.0f, 0.5f);
		auto body = PhysicsBody::create();
		auto shape = PhysicsShapeBox::create(Size(this->getContentSize().width, this->getContentSize().height), materail);
		body->addShape(shape);
		body->setCategoryBitmask(0x02);
		body->setCollisionBitmask(0x04);
		this->setPhysicsBody(body);*/
		ret = true;
	} while (false);
	return ret;
}

void Bullet::update(float dt)
{
	if (this->m_isActive)
	{
		Point curPos = this->getPosition();
		//this->setPosition(curPos + m_fDirection * m_fVelocity);

		float distance = curPos.distanceSquared(this->m_startPostion);
		if (distance >= m_fDisappearDistance)
		{
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
	//this->getPhysicsBody()->setVelocity(Vec2(0.f, 0.f));
	//this->getPhysicsBody()->applyImpulse(m_fVelocity * m_fDirection);
	CCLOG("Bullet::launch (%f, %f)", m_fDirection.x, m_fDirection.y);
}
