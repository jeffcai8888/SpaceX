#ifndef _SPACEX_CLASSES_GUNNER_H_
#define _SPACEX_CLASSES_GUNNER_H_

#include "BaseSprite.h"
class Gunner : public BaseSprite
{
public:
	Gunner();
	~Gunner();

	bool init();

	CREATE_FUNC(Gunner);
};


#endif