#ifndef _OPERATE_LAYER_H_
#define _OPERATE_LAYER_H_

#include <cocos2d.h>

class Hero;
class GameLayer;
class OperateLayer : public cocos2d::Layer
{
public:
	enum JoystickType
	{
		JT_Player,
		JT_Bullet
	};

	enum KeyBoard
	{
		KB_Front = 1 << 0,
		KB_Back = 1 << 1,
		KB_Up = 1 << 2
	};

	OperateLayer();
	~OperateLayer();

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	void exitApp(Ref* pSender);
	void gotoDebug(Ref* pSender);
	void resetTarget();

	CREATE_FUNC(OperateLayer);

	CC_SYNTHESIZE(Hero*, m_pHero, Hero);

private:
	void showJoystick(cocos2d::Point pos);
    void resetJoystick();
	void updateJoystick(cocos2d::Point direction, float distance);
	void updateTarget(cocos2d::Point pos);
	bool isTap(cocos2d::Node*, cocos2d::Point);
	void dealWithKeyBoard();
	void removeAllEventListener();
	cocos2d::Sprite *m_pJoystick;
	cocos2d::Sprite *m_pJoystickBg;
	cocos2d::Sprite *m_pTarget;
	cocos2d::Sprite *m_pFront;
	cocos2d::Sprite *m_pBack;
	cocos2d::Sprite *m_pUp;
	cocos2d::ProgressTimer *m_pBlood;
	cocos2d::ProgressTimer *m_pBloodBg;
	cocos2d::MenuItemImage *m_pCloseItem;
	cocos2d::MenuItemImage *m_pDebugItem;
	cocos2d::Vector<cocos2d::EventListener *> m_vecEventListener;

	int m_KeyPressedValue;
};

#endif