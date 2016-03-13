#ifndef _SPACEX_CLASSES_OPERATELAYER_H_
#define _SPACEX_CLASSES_OPERATELAYER_H_

#include <cocos2d.h>

class BaseSprite;
class Foresight;
class OperateLayer : public cocos2d::Layer
{
public:
	enum ButtonType
	{
		BT_None,
		BT_Right,
		BT_Left,
		BT_Jump
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

	CREATE_FUNC(OperateLayer);

private:
	void showJoystick(cocos2d::Point pos);
	void updateJoystick(cocos2d::Point direction, float distance);
	void updateTarget(cocos2d::Point pos);
	bool isTap(cocos2d::Node*, cocos2d::Point);
	void dealWithKeyBoard();
	void removeAllEventListener();
	void switchButtonStatus(int type, bool isPressed);
	cocos2d::Sprite *m_pJoystick;
	cocos2d::Sprite *m_pJoystickBg;
	cocos2d::Sprite *m_pFront;
	cocos2d::Sprite *m_pBack;
	cocos2d::Sprite *m_pUp;
	cocos2d::ProgressTimer *m_pBlood;
	cocos2d::ProgressTimer *m_pBloodBg;
	cocos2d::MenuItemImage *m_pCloseItem;
	cocos2d::MenuItemImage *m_pDebugItem;
	cocos2d::Vector<cocos2d::EventListener *> m_vecEventListener;
    cocos2d::Point m_firstTouchJoystickLocation;
    int           m_firstTouchJoystickID;
	cocos2d::Point m_preTouchJoystickLocation;
	int			  m_PressType;

	BaseSprite*	m_pHero;
	Foresight *m_pTarget;

	int m_KeyPressedValue;

	cocos2d::ValueMapIntKey m_mapPressType;
};

#endif