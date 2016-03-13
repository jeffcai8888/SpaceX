#ifndef _SPACEX_CLASSES_PRINCESS_H_
#define _SPACEX_CLASSES_PRINCESS_H_

#include "BaseSprite.h"
class Princess : public BaseSprite
{
public:
	Princess();
	~Princess();

	bool init();

	CREATE_FUNC(Princess);
};


#endif