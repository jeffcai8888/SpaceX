#include "Foresight.h"
#include "Hero.h"

USING_NS_CC;

Foresight::Foresight()
	:m_fVelocity(0.0f)
	,m_fDirection(Vec2(0.f, 0.f))
{
}


Foresight::~Foresight()
{
}

bool Foresight::init()
{
	bool ret = false;
	do 
	{
		CC_BREAK_IF(!Sprite::init());
		this->initWithSpriteFrameName("Foresight.png");
		ret = true;
	} while (false);
	return ret;
}

void Foresight::update(float dt)
{
	Sprite::update(dt);
	this->setPosition(this->getPosition() + this->m_fVelocity * this->m_fDirection * dt * 100);
	m_fVelocity = 0.f;
	m_fDirection.x = 0.f;
	m_fDirection.y = 0.f;
}
