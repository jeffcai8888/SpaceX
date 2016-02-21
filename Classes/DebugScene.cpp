#include "DebugScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "SceneManager.h"
#include "JsonParser.h"

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
			else if (pair.first.compare("BulletPower") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_2_1"))->setString(Value(pair.second.asInt()).asString());
			}
			else if (pair.first.compare("BulletSpeed") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_2_2"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("BulletDisappearTime") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_2_3"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("BulletAngle") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_2_4"))->setString(Value(pair.second.asInt()).asString());
			}
			else if (pair.first.compare("BulletInterval") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_2_5"))->setString(Value(pair.second.asFloat()).asString());
			}
			else if (pair.first.compare("WorldG") == 0)
			{
				static_cast<TextField *>(getChildByName("TextField_3_1"))->setString(Value(pair.second.asFloat()).asString());
			}
		}
	}

}

void DebugLayer::onExit()
{
	Layer::onExit();
	ValueVector listData;
	//listData.push_back(Value(createValueMap("HeroHSpeed", 213.3f)));
	ValueMap m;
	m["Attr"] = Value("HeroHSpeed");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_1"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("HeroVSpeed");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_1_2"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("BulletPower");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2_1"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("BulletSpeed");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2_2"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("BulletDisappearTime");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2_3"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("BulletAngle");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2_4"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("BulletInterval");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_2_5"))->getString());
	listData.push_back(Value(m));
	m["Attr"] = Value("WorldG");
	m["Value"] = Value(static_cast<TextField *>(getChildByName("TextField_3_1"))->getString());
	listData.push_back(Value(m));
	auto parser = JsonParser::createWithArray(listData);
	std::string writablePath = FileUtils::getInstance()->getWritablePath();
	std::string fileName = writablePath + "Debug.json";
	parser->encodeDebugData(fileName.c_str());
}

Widget::ccWidgetTouchCallback DebugLayer::onLocateTouchCallback(const std::string &callBackName)
{
	return nullptr;
}

Widget::ccWidgetClickCallback DebugLayer::onLocateClickCallback(const std::string &callBackName)
{
	if (callBackName == "ClickExitButton")//判断事件名，返回对应的函数。下同 
	{
		return CC_CALLBACK_1(DebugLayer::exitDebug, this);
	}
	return nullptr;
}

Widget::ccWidgetEventCallback DebugLayer::onLocateEventCallback(const std::string &callBackName)
{
	if (callBackName == "TextFieldEvent")
	{
		return CC_CALLBACK_2(DebugLayer::textFieldEvent, this);
	}
	return nullptr;
}

void DebugLayer::exitDebug(Ref* pSender)
{
	SceneManager::getInstance()->popScene();
}

void DebugLayer::textFieldEvent(Ref *pSender, int type)
{
	switch (type)
	{
	case TextField::EventType::ATTACH_WITH_IME:
		CCLOG("attach with IME");
		break;

	case TextField::EventType::DETACH_WITH_IME:
		CCLOG("detach with IME");
		break;

	case TextField::EventType::INSERT_TEXT:
		CCLOG("insert words");
		break;

	case TextField::EventType::DELETE_BACKWARD:
		CCLOG("delete words");
		break;

	default:
		break;
	}
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
