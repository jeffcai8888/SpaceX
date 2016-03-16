#ifndef _SPACEX_CLASSES_BOX_H_
#define _SPACEX_CLASSES_BOX_H_

#include <cocos2d.h>
class Box : public cocos2d::Sprite
{
public:
	Box();
	virtual ~Box();
	virtual bool init();

	void initPhysics(cocos2d::Size size, cocos2d::Point pos, int rotaion);

	CREATE_FUNC(Box);
};

#endif

