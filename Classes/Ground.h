#ifndef _SPACEX_CLASSES_GROUND_H_
#define _SPACEX_CLASSES_GROUND_H_

#include <cocos2d.h>
#include "extensions/cocos-ext.h"

class Ground : public cocos2d::extension::PhysicsSprite
{
public:
	Ground();
	virtual ~Ground();
	virtual bool init();

	void initPhysics(cocos2d::Size size, cocos2d::Point pos, int rotaion);

	CREATE_FUNC(Ground);
};

#endif

