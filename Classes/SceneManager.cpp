#include "SceneManager.h"
#include "GameScene.h"
#include "GameOverScene.h"
#include "DebugScene.h"
#include "SocketManager.h"

USING_NS_CC;

SceneManager *SceneManager::instance = nullptr;

SceneManager* SceneManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new SceneManager();
	}
	return instance;
}


void SceneManager::showScene(SceneType sceneType, bool isReplace)
{
	Scene *pScene = NULL;
	switch(sceneType)
	{
	case GAME_SCENE:
	{
		int networkType = SocketManager::getInstance()->getNetworkType();
		pScene = GameScene::createScene(networkType);
	}
		break;
	case GAME_OVER_SCENE:
		pScene = GameOverScene::createScene();
		break;
	case DEBUG_SCENE:
		pScene = DebugScene::createScene();
		break;
	}

	if(pScene)
	{
		if (isReplace)
			Director::getInstance()->replaceScene(pScene);
		else
			Director::getInstance()->pushScene(pScene);
	}
}

void SceneManager::popScene()
{
	Director::getInstance()->popScene();
}