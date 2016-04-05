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
		this->initWithSpriteFrameName("arrow.png");
		this->setAnchorPoint(Vec2(0.f, 0.5f));
		ret = true;
	} while (false);
	return ret;
}
