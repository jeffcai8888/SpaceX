#include "DebugScene.h"
#include "GameScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "SceneManager.h"
#include "JsonParser.h"
#include "SocketManager.h"
#include "GameData.h"
#include "BaseSprite.h"

USING_NS_CC;
using namespace cocos2d::ui;

DebugLayer::DebugLayer()
:m_pCloseItem(nullptr)
{

}

DebugLayer::~DebugLayer()
{

}

void DebugLayer::onEnter()
{
	Layer::onEnter();

	JsonParser* parser;
	JsonParser* parser1;
	if (GameData::getInstance()->getRoleType() == ROLE_HERO)
	{
		parser = JsonParser::createWithFile("Hero.json");
		parser1 = JsonParser::createWithFile("Bullet1Config.json");
		parser->decodeDebugData();
		auto list = parser->getList();
		for (auto& v : list)
		{
			ValueMap row = v.asValueMap();

			for (auto& pair : row)
			{
				if (pair.first.compare("WalkSpeed") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_1"))->setString(Value(pair.second.asFloat()).asString());
				}
				else if (pair.first.compare("JumpSpeed") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_2"))->setString(Value(pair.second.asFloat()).asString());
				}
				else if (pair.first.compare("Gravity") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_4"))->setString(Value(pair.second.asFloat()).asString());
				}
				else if (pair.first.compare("Skill1V") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_5"))->setString(Value(pair.second.asFloat()).asString());
				}
				else if (pair.first.compare("Skill2V") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_6"))->setString(Value(pair.second.asFloat()).asString());
				}
				else if (pair.first.compare("Skill1CD") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_7"))->setString(Value(pair.second.asFloat()).asString());
				}
				else if (pair.first.compare("Skill2CD") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_8"))->setString(Value(pair.second.asFloat()).asString());
				}
				else if (pair.first.compare("Skill1Time") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_9"))->setString(Value(pair.second.asFloat()).asString());
				}
				else if (pair.first.compare("Skill2Time") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_10"))->setString(Value(pair.second.asFloat()).asString());
				}
			}
		}
	}
	else if (GameData::getInstance()->getRoleType() == ROLE_GUN)
	{
		parser = JsonParser::createWithFile("Gunner.json");
		parser1 = JsonParser::createWithFile("Bullet2Config.json");
		parser->decodeDebugData();
		auto list = parser->getList();
		for (auto& v : list)
		{
			ValueMap row = v.asValueMap();

			for (auto& pair : row)
			{
				if (pair.first.compare("WalkSpeed") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_1"))->setString(Value(pair.second.asFloat()).asString());
					static_cast<TextField *>(getChildByName("TextField_1_1"))->setVisible(true);
				}
				else if (pair.first.compare("JumpSpeed") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_2"))->setString(Value(pair.second.asFloat()).asString());
					static_cast<TextField *>(getChildByName("TextField_1_2"))->setVisible(true);
				}
				else if (pair.first.compare("Gravity") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_4"))->setString(Value(pair.second.asFloat()).asString());
					static_cast<TextField *>(getChildByName("TextField_1_4"))->setVisible(true);
				}

				static_cast<TextField *>(getChildByName("TextField_1_5"))->setVisible(false);
				static_cast<TextField *>(getChildByName("TextField_1_6"))->setVisible(false);
				static_cast<TextField *>(getChildByName("TextField_1_7"))->setVisible(false);
				static_cast<TextField *>(getChildByName("TextField_1_8"))->setVisible(false);
				static_cast<TextField *>(getChildByName("TextField_1_9"))->setVisible(false);
				static_cast<TextField *>(getChildByName("TextField_1_10"))->setVisible(false);
			}
		}
	}
	else if(GameData::getInstance()->getRoleType() == ROLE_PRINCESS)
	{
		parser = JsonParser::createWithFile("Gunner.json");
		parser1 = JsonParser::createWithFile("Bullet1Config.json");
		parser->decodeDebugData();
		auto list = parser->getList();	
		for (auto& v : list)
		{
			ValueMap row = v.asValueMap();

			for (auto& pair : row)
			{
				if (pair.first.compare("WalkSpeed") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_1"))->setString(Value(pair.second.asFloat()).asString());
					static_cast<TextField *>(getChildByName("TextField_1_1"))->setVisible(true);
				}
				else if (pair.first.compare("JumpSpeed") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_2"))->setString(Value(pair.second.asFloat()).asString());
					static_cast<TextField *>(getChildByName("TextField_1_2"))->setVisible(true);
				}
				else if (pair.first.compare("Gravity") == 0)
				{
					static_cast<TextField *>(getChildByName("TextField_1_4"))->setString(Value(pair.second.asFloat()).asString());
					static_cast<TextField *>(getChildByName("TextField_1_4"))->setVisible(true);
				}

				static_cast<TextField *>(getChildByName("TextField_1_5"))->setVisible(false);
				static_cast<TextField *>(getChildByName("TextField_1_6"))->setVisible(false);
				static_cast<TextField *>(getChildByName("TextField_1_7"))->setVisible(false);
				static_cast<TextField *>(getChildByName("TextField_1_8"))->setVisible(false);
				static_cast<TextField *>(getChildByName("TextField_1_9"))->setVisible(false);
				static_cast<TextField *>(getChildByName("TextField_1_10"))->setVisible(false);
			}
		}
	}
	
    
    parser1->decodeDebugData();
    auto list1 = parser1->getList();
    for (auto& v : list1)
    {
        ValueMap row = v.asValueMap();
        
        for (auto& pair : row)
        {
            
            if (pair.first.compare("Power") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_2_1"))->setString(Value(pair.second.asInt()).asString());
            }
            else if (pair.first.compare("Speed") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_2_2"))->setString(Value(pair.second.asFloat()).asString());
            }
            else if (pair.first.compare("DisappearTime") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_2_3"))->setString(Value(pair.second.asFloat()).asString());
            }
            else if (pair.first.compare("Angle") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_2_4"))->setString(Value(pair.second.asInt()).asString());
            }
            else if (pair.first.compare("Interval") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_2_5"))->setString(Value(pair.second.asFloat()).asString());
            }
            else if (pair.first.compare("Gravity") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_2_6"))->setString(Value(pair.second.asFloat()).asString());
            }
            else if (pair.first.compare("Capacity") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_2_7"))->setString(Value(pair.second.asInt()).asString());
            }
        }
    }
    
    JsonParser* parser2 = JsonParser::createWithFile("Bomb1Config.json");
    parser2->decodeDebugData();
    auto list2 = parser2->getList();
    for (auto& v : list2)
    {
        ValueMap row = v.asValueMap();
        
        for (auto& pair : row)
        {
            if (pair.first.compare("Speed") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_4_1"))->setString(Value(pair.second.asFloat()).asString());
            }
            else if (pair.first.compare("Range") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_4_2"))->setString(Value(pair.second.asFloat()).asString());
            }
            else if (pair.first.compare("StartTime") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_4_3"))->setString(Value(pair.second.asFloat()).asString());
            }
            else if (pair.first.compare("CDTime") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_4_4"))->setString(Value(pair.second.asFloat()).asString());
            }
            else if (pair.first.compare("Power") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_4_5"))->setString(Value(pair.second.asInt()).asString());
            }
            else if (pair.first.compare("Gravity") == 0)
            {
                static_cast<TextField *>(getChildByName("TextField_4_6"))->setString(Value(pair.second.asInt()).asString());
            }
        }
    }
}

void DebugLayer::onExit()
{
    Layer::onExit();
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    
    ValueVector listData;
	ValueMap m;

	if (GameData::getInstance()->getRoleType() == ROLE_HERO)
	{
		m["Attr"] = Value("WalkSpeed");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_1"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("JumpSpeed");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_2"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("Gravity");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_4"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("Skill1V");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_5"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("Skill2V");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_6"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("Skill1CD");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_7"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("Skill2CD");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_8"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("Skill1Time");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_9"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("Skill2Time");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_10"))->getString());
		listData.push_back(Value(m));
		auto parser1 = JsonParser::createWithArray(listData);
		std::string fileName = writablePath + "Hero.json";
		parser1->encodeDebugData(fileName.c_str());
	}
	else if (GameData::getInstance()->getRoleType() == ROLE_GUN)
	{
		m["Attr"] = Value("WalkSpeed");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_1"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("JumpSpeed");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_2"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("Gravity");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_4"))->getString());
		listData.push_back(Value(m));
		auto parser1 = JsonParser::createWithArray(listData);
		std::string fileName = writablePath + "Gunner.json";
		parser1->encodeDebugData(fileName.c_str());
	}
	else if (GameData::getInstance()->getRoleType() == ROLE_PRINCESS)
	{
		m["Attr"] = Value("WalkSpeed");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_1"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("JumpSpeed");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_2"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("Gravity");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_4"))->getString());
		listData.push_back(Value(m));
		auto parser1 = JsonParser::createWithArray(listData);
		std::string fileName = writablePath + "Princess.json";
		parser1->encodeDebugData(fileName.c_str());
	}
    
	
    
    
    
    listData.clear();
    m.clear();
	m["Attr"] = Value("Power");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2_1"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("Speed");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2_2"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("DisappearTime");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2_3"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("Angle");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2_4"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("Interval");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2_5"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("Gravity");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2_6"))->getString());
	listData.push_back(Value(m));
    m["Attr"] = Value("Capacity");
    m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2_7"))->getString());
    listData.push_back(Value(m));
    auto parser2 = JsonParser::createWithArray(listData);
	if (GameData::getInstance()->getRoleType() == ROLE_HERO)
	{
		std::string fileName = writablePath + "Bullet1Config.json";
		parser2->encodeDebugData(fileName.c_str());
	}
	else if (GameData::getInstance()->getRoleType() == ROLE_GUN)
	{
		std::string fileName = writablePath + "Bullet2Config.json";
		parser2->encodeDebugData(fileName.c_str());
	}
	else if (GameData::getInstance()->getRoleType() == ROLE_PRINCESS)
	{
		std::string fileName = writablePath + "Bullet1Config.json";
		parser2->encodeDebugData(fileName.c_str());
	}
    
	{
		listData.clear();
		m.clear();
		m["Attr"] = Value("Speed");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_4_1"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("Range");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_4_2"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("StartTime");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_4_3"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("CDTime");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_4_4"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("Power");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_4_5"))->getString());
		listData.push_back(Value(m));
		m["Attr"] = Value("Gravity");
		m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_4_6"))->getString());
		listData.push_back(Value(m));
		auto parser3 = JsonParser::createWithArray(listData);
		std::string fileName = writablePath + "Bomb1Config.json";
		parser3->encodeDebugData(fileName.c_str());
	}
    
}

Widget::ccWidgetClickCallback DebugLayer::onLocateClickCallback(const std::string &callBackName)
{
	if (callBackName == "ClickExitButton")//判断事件名，返回对应的函数。下同 
	{
		return CC_CALLBACK_1(DebugLayer::exitDebug, this);
	}
	return nullptr;
}


void DebugLayer::exitDebug(Ref* pSender)
{
	SceneManager::getInstance()->popScene();		
}

static DebugLayerReader* _instanceDebugLayerReader = nullptr;
DebugLayerReader* DebugLayerReader::getInstance()
{
	if (_instanceDebugLayerReader == nullptr)
	{
		_instanceDebugLayerReader = new DebugLayerReader();
	}
	return _instanceDebugLayerReader;
}

void DebugLayerReader::purge()
{
	CC_SAFE_DELETE(_instanceDebugLayerReader);
}

Node* DebugLayerReader::createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)
{
	DebugLayer* node = DebugLayer::create();
	setPropsWithFlatBuffers(node, nodeOptions);
	return node;
}

Scene* DebugScene::createScene()
{
	auto scene = Scene::create();
	CSLoader::getInstance()->registReaderObject("DebugLayerReader", (ObjectFactory::Instance)DebugLayerReader::getInstance);
	auto layer = CSLoader::createNode("DebugScene.csb");
	scene->addChild(layer);
	return scene;
}
