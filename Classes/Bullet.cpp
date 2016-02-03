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
		auto body = PhysicsBody::create();
		auto shape = PhysicsShapeBox::create(Size(this->getContentSize().width, this->getContentSize().height), PHYSICSSHAPE_MATERIAL_DEFAULT);
		body->addShape(shape);
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
		this->setPosition(curPos + m_fDirection * m_fVelocity);

		float distance = curPos.distanceSquared(this->m_startPostion);
		if (distance >= m_fDisappearDistance)
			this->m_isActive = false;
	}
}

void Bullet::launch(Hero* pHero)
{
	this->m_isActive = true;
	this->m_owner = pHero;
	Point pos = pHero->getPosition();
	this->setPosition(pos);
	this->m_startPostion = pos;
}
