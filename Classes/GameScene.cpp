#include "Macro.h"
#include "GameScene.h"
#include "OfflineGameLayer.h"
#include "ClientGameLayer.h"
#include "ServerGameLayer.h"
#include "OperateLayer.h"
#include "SocketManager.h"

USING_NS_CC;

Scene* GameScene::createScene(int networkType)
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setSpeed(2.f);

	SocketManager::getInstance()->setNetworkType(networkType);
	SocketManager::getInstance()->init();
	
	if (networkType == NT_Client)
	{
		auto gameLayer = ClientGameLayer::create();
		scene->addChild(gameLayer, 0, LT_Game);
	}
	else if (networkType == NT_Server)
	{
		auto gameLayer = ServerGameLayer::create();
		scene->addChild(gameLayer, 0, LT_Game);
	}
	else
	{
		auto gameLayer = OfflineGameLayer::create();
		scene->addChild(gameLayer, 0, LT_Game);
	}
	
	auto operateLayer = OperateLayer::create();
	scene->addChild(operateLayer, 1, LT_Operate);

	

	return scene;
}


