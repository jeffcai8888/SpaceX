#ifndef _SPACEX_CLASSES_SLOPE_H_
#define _SPACEX_CLASSES_SLOPE_H_

#include <cocos2d.h>
class Slope : public cocos2d::Sprite
{
public:
	Slope();
	virtual ~Slope();
	virtual bool init();

	void initPhysics(cocos2d::Size size, cocos2d::Point pos, int rotaion);

	CREATE_FUNC(Slope);
};

#endif

