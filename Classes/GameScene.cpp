#include "GameScene.h"
#include "GameLayer.h"
#include "OperateLayer.h"

using namespace cocos2d;

Scene* GameScene::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setSpeed(2.f);
	
	auto gameLayer = GameLayer::create();
	scene->addChild(gameLayer, 0, LT_Game);

	auto operateLayer = OperateLayer::create();
	scene->addChild(operateLayer, 1, LT_Operate);
	operateLayer->setHero(gameLayer->getHero());

	return scene;
}


