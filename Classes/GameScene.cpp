#include "GameScene.h"
#include "GameLayer.h"
#include "OperateLayer.h"

using namespace cocos2d;

Scene* GameScene::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	
	auto gameLayer = GameLayer::create();
	gameLayer->setPhyWorld(scene->getPhysicsWorld());
	scene->addChild(gameLayer, 0);

	auto operateLayer = OperateLayer::create();
	scene->addChild(operateLayer, 1);
	operateLayer->setHero(gameLayer->getHero());

	Size visibleSize = Director::getInstance()->getVisibleSize();

	PhysicsMaterial materail(0.1f, 0.0f, 0.0f);
	auto body = PhysicsBody::createEdgeBox(visibleSize, PHYSICSSHAPE_MATERIAL_DEFAULT, 3);
	body->setCategoryBitmask(0x04);
	body->setCollisionBitmask(0x01);
	auto edgeNode = Node::create();
	edgeNode->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	edgeNode->setPhysicsBody(body);
	scene->addChild(edgeNode);

	return scene;
}


