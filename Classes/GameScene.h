#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include <cocos2d.h>

using namespace cocos2d;

enum LayerTag
{
	LT_Game = 0,
	LT_Operate
};

class GameScene
{
public:
	static Scene* createScene();

};


#endif