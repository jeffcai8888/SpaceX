#ifndef _SPACEX_CLASSES_DEBUGSCENE_H_
#define _SPACEX_CLASSES_DEBUGSCENE_H_

#include <cocos2d.h>
#include "cocostudio/WidgetCallBackHandlerProtocol.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"
#include "ui/CocosGUI.h"

class DebugLayer : public cocos2d::Layer, public cocostudio::WidgetCallBackHandlerProtocol
{
public:
	DebugLayer();
	virtual ~DebugLayer();

	CREATE_FUNC(DebugLayer);

	bool m_isFirst;


	virtual cocos2d::ui::Widget::ccWidgetTouchCallback onLocateTouchCallback(const std::string &callBackName);
	virtual cocos2d::ui::Widget::ccWidgetClickCallback onLocateClickCallback(const std::string &callBackName);
	virtual cocos2d::ui::Widget::ccWidgetEventCallback onLocateEventCallback(const std::string &callBackName);
	virtual void onEnter();
	virtual void onExit();

	//bool init();
	void exitDebug(cocos2d::Ref* pSender);
	void textFieldEvent(cocos2d::Ref *pSender, int type);

private:
	cocos2d::MenuItemImage *m_pCloseItem;
};

class DebugLayerReader : public cocostudio::NodeReader
{
public :
	DebugLayerReader() {};
	~DebugLayerReader() {};

	static DebugLayerReader* getInstance();
	static void purge();
	cocos2d::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions);
};



class DebugScene
{
public:
	static cocos2d::Scene* createScene(bool isFirst = false);
};

#endif