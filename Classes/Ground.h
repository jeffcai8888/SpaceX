#pragma once
#include <cocos2d.h>
class Ground : public cocos2d::Sprite
{
public:
	Ground();
	virtual ~Ground();

	void initPhysics(cocos2d::Size size, cocos2d::Point pos);

	CREATE_FUNC(Ground);
};

