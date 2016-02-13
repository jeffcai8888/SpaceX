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

	/*const PhysicsMaterial m(1.f, 0.f, 1.f);
	auto body = PhysicsBody::createEdgeSegment(Vec2(0.f, 0.f), Vec2(visibleSize.width, 0.f), PHYSICSSHAPE_MATERIAL_DEFAULT);
	body->setCategoryBitmask(0x04);
	body->setCollisionBitmask(0x01);
	auto edgeNode = Node::create();
	edgeNode->setPosition(Point(0.f, 0.f));
	edgeNode->setPhysicsBody(body);
	scene->addChild(edgeNode);*/
	return scene;
}


