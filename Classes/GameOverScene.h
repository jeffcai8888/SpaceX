#ifndef _SPACEX_CLASSES_GAMEOVERSCENE_
#define _SPACEX_CLASSES_GAMEOVERSCENE_

#include <cocos2d.h>


class GameOverLayer : public cocos2d::LayerColor
{
public:
	GameOverLayer();
	virtual ~GameOverLayer();

	CREATE_FUNC(GameOverLayer);

	void restartGame();
	bool init();

	CC_SYNTHESIZE_READONLY(cocos2d::Label*, m_pLabel, GameOverLabel);
};

class GameOverScene
{
public:
	static cocos2d::Scene* createScene();
};

#endif