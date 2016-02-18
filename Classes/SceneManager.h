#ifndef _SPACEX_CLASSES_SCENEMANAGER_H_
#define _SPACEX_CLASSES_SCENEMANAGER_H_

#include <cocos2d.h>


typedef enum {
	GAME_SCENE = 0,
	GAME_OVER_SCENE = 1,
	DEBUG_SCENE = 2,
}SceneType;


class SceneManager
{
public:
	SceneManager(){};
	~SceneManager(){};

	static SceneManager* getInstance();

	void showScene(SceneType sceneType, bool isReplace = true);
	void popScene();

};

#endif
