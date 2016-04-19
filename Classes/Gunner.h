#ifndef _SPACEX_CLASSES_GUNNER_H_
#define _SPACEX_CLASSES_GUNNER_H_

#include "BaseSprite.h"
class Gunner : public BaseSprite
{
public:
	Gunner();
	~Gunner();

	bool init();
	virtual bool isInSplash() { return false; }

	CREATE_FUNC(Gunner);
};


#endif