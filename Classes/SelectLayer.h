#ifndef _SPACEX_CLASSES_SELECTSCENE_H_
#define _SPACEX_CLASSES_SELECTSCENE_H_

#include <cocos2d.h>

class SelectLayer : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(SelectLayer);
	void menuServerClick(cocos2d::Ref* sender);
	void menuClientClick(cocos2d::Ref* sender);
	void menuOfflineClick(cocos2d::Ref* sender);

private:
	void initMenu();
};

#endif