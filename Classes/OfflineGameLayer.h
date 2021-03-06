#ifndef _SPACEX_CLASSES_OFFLINEGAMELAYER_H_
#define _SPACEX_CLASSES_OFFLINEGAMELAYER_H_

#include <cocos2d.h>
#include "GameLayer.h"


class BaseSprite;
class Hero;
class Bullet;
class GameData;

class OfflineGameLayer : public GameLayer
{
public:
	OfflineGameLayer();
	virtual ~OfflineGameLayer();

	virtual void onExit();

	CREATE_FUNC(OfflineGameLayer);

protected:
};

#endif