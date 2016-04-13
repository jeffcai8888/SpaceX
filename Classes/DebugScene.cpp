#include "DebugScene.h"
#include "GameScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "SceneManager.h"
#include "JsonParser.h"
#include "SocketManager.h"

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

	JsonParser* parser = JsonParser::createWithFile("Debug.json");
	parser->decodeDebugData();
	auto list = parser->getList();
	for (auto& v : list)
	{
		ValueMap row = v.asValueMap();

		for (auto& pair : row)
		{
			if (pair.first.compare("HeroHSpeed") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_1_1"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("HeroVSpeed") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_1_2"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("HeroG") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_1_4"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("HeroSkill1V") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_1_5"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("HeroSkill2V") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_1_6"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("HeroSkill1CD") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_1_7"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("HeroSkill2CD") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_1_8"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("HeroSkill1Time") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_1_9"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("HeroSkill2Time") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_1_10"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("ForesightSpeed") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_3_2"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("JoystickX") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_3_3"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("JoystickY") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_3_4"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("JoystickScale") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_3_5"))->setString(Value(pair.second.asFloat()).asString());
			}
		}
	}
    
    JsonParser* parser1 = JsonParser::createWithFile("Bullet1Config.json");
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
    
	m["Attr"] = Value("HeroHSpeed");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_1"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("HeroVSpeed");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_2"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("HeroG");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_4"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("HeroSkill1V");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_5"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("HeroSkill2V");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_6"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("HeroSkill1CD");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_7"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("HeroSkill2CD");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_8"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("HeroSkill1Time");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_9"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("HeroSkill2Time");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_10"))->getString());
	listData.push_back(Value(m));
    m["Attr"] = Value("ForesightSpeed");
    m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_3_2"))->getString());
    listData.push_back(Value(m));
    m["Attr"] = Value("JoystickX");
    m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_3_3"))->getString());
    listData.push_back(Value(m));
    m["Attr"] = Value("JoystickY");
    m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_3_4"))->getString());
    listData.push_back(Value(m));
    m["Attr"] = Value("JoystickScale");
    m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_3_5"))->getString());
    listData.push_back(Value(m));
    auto parser1 = JsonParser::createWithArray(listData);
    std::string fileName = writablePath + "Debug.json";
    parser1->encodeDebugData(fileName.c_str());
    
    
    
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
    fileName = writablePath + "Bullet1Config.json";
    parser2->encodeDebugData(fileName.c_str());
    
    
    
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
	fileName = writablePath + "Bomb1Config.json";
	parser3->encodeDebugData(fileName.c_str());
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
