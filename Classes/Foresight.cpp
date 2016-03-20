#include "Foresight.h"
#include "Hero.h"

USING_NS_CC;

Foresight::Foresight()
	:m_fVelocity(0.0f)
	,m_fDirection(Vec2(0.f, 0.f))
	//,m_isStatic(true)
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
	//if(!m_isStatic)
	{
		this->setPosition(this->getPosition() + this->m_fDirection * dt);
		this->m_fDirection = Vec2(0.f, 0.f);
		//m_isStatic = true;
	}
	
}
