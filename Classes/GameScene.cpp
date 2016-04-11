#include "Macro.h"
#include "GameScene.h"
#include "OfflineGameLayer.h"
#include "ClientGameLayer.h"
#include "ServerGameLayer.h"
#include "OperateLayer.h"
#include "SocketManager.h"
#include "cocostudio/CocoStudio.h"
#include "JsonParser.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	int networkType = SocketManager::getInstance()->getNetworkType();

	auto layer = CSLoader::createNode("BackgroundLayer.csb");
	scene->addChild(layer);
	
	if (networkType == NT_Client)
	{
		std::string addr0, addr1, addr2, addr3;
		JsonParser* parser = JsonParser::createWithFile("Config.json");
		parser->decodeDebugData();
		auto list = parser->getList();
		for (auto& v : list)
		{
			ValueMap row = v.asValueMap();

			for (auto& pair : row)
			{
				if (pair.first.compare("ServerIP0") == 0)
				{
					addr0 = Value(pair.second.asInt()).asString();
				}
				if (pair.first.compare("ServerIP1") == 0)
				{
					addr1 = Value(pair.second.asInt()).asString();
				}
				if (pair.first.compare("ServerIP2") == 0)
				{
					addr2 = Value(pair.second.asInt()).asString();
				}
				if (pair.first.compare("ServerIP3") == 0)
				{
					addr3 = Value(pair.second.asInt()).asString();
				}
			}
		}

		SocketManager::getInstance()->setServerAddr(addr0 + "." + addr1 + "." + addr2 + "." + addr3);
		SocketManager::getInstance()->init();
		auto gameLayer = ClientGameLayer::create();
		scene->addChild(gameLayer, 0, LT_Game);
	}
	else if (networkType == NT_Server)
	{
		SocketManager::getInstance()->init();
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


