#include "Foresight.h"
#include "Hero.h"

USING_NS_CC;

Foresight::Foresight()
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
