#include "StartScene.h"
#include "GameScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "SceneManager.h"
#include "SocketManager.h"
#include "JsonParser.h"
#include "BaseSprite.h"
#include "GameData.h"

USING_NS_CC;
using namespace cocos2d::ui;

StartLayer::StartLayer()
{

}

StartLayer::~StartLayer()
{

}

void StartLayer::onEnter()
{
	Layer::onEnter();

	JsonParser* parser = JsonParser::createWithFile("Config.json");
	parser->decodeDebugData();
	auto list = parser->getList();
	if (SocketManager::getInstance()->getNetworkType() == NT_Client)
	{
		static_cast<Text *>(getChildByName("Text_1"))->setVisible(true);
		static_cast<Text *>(getChildByName("Text_2"))->setVisible(true);
		static_cast<Text *>(getChildByName("Text_3"))->setVisible(true);
		static_cast<Text *>(getChildByName("Text_4"))->setVisible(true);
		static_cast<TextField *>(getChildByName("TextField_1"))->setVisible(true);
		static_cast<TextField *>(getChildByName("TextField_2"))->setVisible(true);
		static_cast<TextField *>(getChildByName("TextField_3"))->setVisible(true);
		static_cast<TextField *>(getChildByName("TextField_4"))->setVisible(true);

		/*for (auto& v : list)
		{
			ValueMap row = v.asValueMap();
			for (auto& pair : row)
			{
				if (pair.first.compare("ServerIP0") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1"))->setString(Value(pair.second.asInt()).asString());
				}
				else if (pair.first.compare("ServerIP1") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_2"))->setString(Value(pair.second.asInt()).asString());
				}
				else if (pair.first.compare("ServerIP2") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_3"))->setString(Value(pair.second.asInt()).asString());
				}
				else if (pair.first.compare("ServerIP3") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_4"))->setString(Value(pair.second.asInt()).asString());
				}
			}
		}*/
	}
	else
	{
		static_cast<Text *>(getChildByName("Text_1"))->setVisible(false);
		static_cast<Text *>(getChildByName("Text_2"))->setVisible(false);
		static_cast<Text *>(getChildByName("Text_3"))->setVisible(false);
		static_cast<Text *>(getChildByName("Text_4"))->setVisible(false);
		static_cast<TextField *>(getChildByName("TextField_1"))->setVisible(false);
		static_cast<TextField *>(getChildByName("TextField_2"))->setVisible(false);
		static_cast<TextField *>(getChildByName("TextField_3"))->setVisible(false);
		static_cast<TextField *>(getChildByName("TextField_4"))->setVisible(false);
	}
	if(SocketManager::getInstance()->getNetworkType() == NT_Client)
		SocketManager::getInstance()->getSocketClient()->onNewConnection = CC_CALLBACK_1(StartLayer::onNewConnection, this);
}

void StartLayer::onExit()
{
	Layer::onExit();
	ValueVector listData;
	ValueMap m;
	m["Attr"] = Value("ServerIP0");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1"))->getString());
	listData.push_back(Value(m));
	
	m["Attr"] = Value("ServerIP1");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2"))->getString());
	listData.push_back(Value(m));

	m["Attr"] = Value("ServerIP2");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_3"))->getString());
	listData.push_back(Value(m));

	m["Attr"] = Value("ServerIP3");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_4"))->getString());
	listData.push_back(Value(m));

	auto parser = JsonParser::createWithArray(listData);
	std::string writablePath = FileUtils::getInstance()->getWritablePath();
	std::string fileName = writablePath + "Config.json";
	parser->encodeDebugData(fileName.c_str());
}

Widget::ccWidgetClickCallback StartLayer::onLocateClickCallback(const std::string &callBackName)
{
	if (callBackName == "onClick1")//判断事件名，返回对应的函数。下同 
	{
		return CC_CALLBACK_1(StartLayer::onClick1, this);
	}
	else if (callBackName == "onClick2")//判断事件名，返回对应的函数。下同 
	{
		return CC_CALLBACK_1(StartLayer::onClick2, this);
	}
	else if (callBackName == "onClick3")//判断事件名，返回对应的函数。下同 
	{
		return CC_CALLBACK_1(StartLayer::onClick3, this);
	}
	return nullptr;
}

void StartLayer::onClick1(Ref *pSender)
{
	GameData::getInstance()->setRoleType(ROLE_HERO);
	startGame();
}

void StartLayer::onClick2(Ref *pSender)
{
	GameData::getInstance()->setRoleType(ROLE_GUN);
	startGame();
}

void StartLayer::onClick3(Ref *pSender)
{
	GameData::getInstance()->setRoleType(ROLE_PRINCESS);
	startGame();
}

void StartLayer::onNewConnection(const char* data)
{
	CCLOG("onNewConnection %s", data);
	JsonParser* parser = JsonParser::createWithString(data);
	parser->decodeLoginProtocol();
	importStartGameData(parser);
	auto scene = GameScene::createScene();
	Director::getInstance()->replaceScene(scene);
}

void StartLayer::importStartGameData(JsonParser* parser)
{
	auto list = parser->getList();
	for (auto& v : list)
	{
		ValueMap row = v.asValueMap();
		for (auto& pair : row)
		{
			if (pair.first.compare("0") == 0)
			{
				GameData::getInstance()->m_playerTypes[0] = pair.second.asInt();
			}
			else if (pair.first.compare("1") == 0)
			{
				GameData::getInstance()->m_playerTypes[1] = pair.second.asInt();
			}
			else if (pair.first.compare("2") == 0)
			{
				GameData::getInstance()->m_playerTypes[2] = pair.second.asInt();
			}
			else if (pair.first.compare("3") == 0)
			{
				GameData::getInstance()->m_playerTypes[3] = pair.second.asInt();
			}
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		if (GameData::getInstance()->m_playerTypes[i] == ROLE_NONE)
		{
			GameData::getInstance()->m_playerTypes[i] = GameData::getInstance()->getRoleType();
			GameData::getInstance()->setRoleIndex(i);
			break;
		}
	}
}

void StartLayer::startGame()
{	
	if (SocketManager::getInstance()->getNetworkType() == NT_Client)
	{
		std::string addr0, addr1, addr2, addr3;
		addr0 = static_cast<TextField *>(getChildByName("TextField_1"))->getString();
		addr1 = static_cast<TextField *>(getChildByName("TextField_2"))->getString();
		addr2 = static_cast<TextField *>(getChildByName("TextField_3"))->getString();
		addr3 = static_cast<TextField *>(getChildByName("TextField_4"))->getString();
		SocketManager::getInstance()->setServerAddr(addr0 + "." + addr1 + "." + addr2 + "." + addr3);
		SocketManager::getInstance()->start();
	}
	else if(SocketManager::getInstance()->getNetworkType() == NT_Server)
	{
		SocketManager::getInstance()->start();
		auto scene = GameScene::createScene();
		Director::getInstance()->replaceScene(scene);
	}
	else
	{
		JsonParser* parser = JsonParser::createWithFile("start0.json");
		parser->decodeLoginProtocol();
		importStartGameData(parser);
		auto scene = GameScene::createScene();
		Director::getInstance()->replaceScene(scene);
	}	
}

static StartLayerReader* _instanceStartLayerReader = nullptr;
StartLayerReader* StartLayerReader::getInstance()
{
	if (_instanceStartLayerReader == nullptr)
	{
		_instanceStartLayerReader = new StartLayerReader();
	}
	return _instanceStartLayerReader;
}

void StartLayerReader::purge()
{
	CC_SAFE_DELETE(_instanceStartLayerReader);
}

Node* StartLayerReader::createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)
{
	StartLayer* node = StartLayer::create();
	setPropsWithFlatBuffers(node, nodeOptions);
	return node;
}

Scene* StartScene::createScene()
{
	auto scene = Scene::create();
	CSLoader::getInstance()->registReaderObject("StartLayerReader", (ObjectFactory::Instance)StartLayerReader::getInstance);
	auto layer = CSLoader::createNode("StartScene.csb");
	scene->addChild(layer);
	return scene;
}
