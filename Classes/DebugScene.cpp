#include "DebugScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "SceneManager.h"

USING_NS_CC;
using namespace cocos2d::ui;

DebugLayer::DebugLayer()
:m_pCloseItem(nullptr)
{

}

DebugLayer::~DebugLayer()
{

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
