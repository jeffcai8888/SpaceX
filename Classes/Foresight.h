#ifndef _SPACEX_CLASSES_FORESIGHT_H_
#define _SPACEX_CLASSES_FORESIGHT_H_

#include <cocos2d.h>

class Hero;
class Foresight : public cocos2d::Sprite
{
public:
	Foresight();
	virtual ~Foresight();

	virtual bool init();

	CREATE_FUNC(Foresight);
private:
};

#endif