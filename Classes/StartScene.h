#ifndef _SPACEX_CLASSES_STARTSCENE_H_
#define _SPACEX_CLASSES_STARTSCENE_H_

#include <cocos2d.h>
#include "cocostudio/WidgetCallBackHandlerProtocol.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"
#include "ui/CocosGUI.h"

class StartLayer : public cocos2d::Layer, public cocostudio::WidgetCallBackHandlerProtocol
{
public:
	StartLayer();
	virtual ~StartLayer();

	CREATE_FUNC(StartLayer);


	virtual cocos2d::ui::Widget::ccWidgetClickCallback onLocateClickCallback(const std::string &callBackName);
	virtual void onEnter();
	virtual void onExit();

	//bool init();
	void onClick1(cocos2d::Ref* pSender);
	void onClick2(cocos2d::Ref* pSender);
	void onClick3(cocos2d::Ref* pSender);

private:
	int m_selectRole;
};

class StartLayerReader : public cocostudio::NodeReader
{
public :
	StartLayerReader() {};
	~StartLayerReader() {};

	static StartLayerReader* getInstance();
	static void purge();
	cocos2d::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions);
};



class StartScene
{
public:
	static cocos2d::Scene* createScene();
};

#endif