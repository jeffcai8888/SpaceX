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
		BT_Joystick,
		BT_Jump,
        BT_Shoot
	};

	enum KeyBoard
	{
		KB_Front = 1 << 0,
		KB_Back = 1 << 1,
		KB_Up = 1 << 2,
        KB_Shoot = 1 << 3
	};

	OperateLayer();
	~OperateLayer();

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	void gotoDebug(Ref* pSender);

	CREATE_FUNC(OperateLayer);

private:
	void showJoystick(cocos2d::Point pos);
	void updateJoystick(cocos2d::Point direction, float distance);
	bool isTap(cocos2d::Node*, cocos2d::Point);
	bool isInRange(cocos2d::Point, int, cocos2d::Point);
	void dealWithKeyBoard();
	void removeAllEventListener();
	void switchButtonStatus(int type, bool isPressed);
	void calJoyStickPos(cocos2d::Point& pos);
	void dealWithJoystick(cocos2d::Point, cocos2d::Point);
	void resetHero();
	cocos2d::Sprite *m_pJoystick;
	cocos2d::Sprite *m_pJoystickBg;
    cocos2d::Sprite *m_pShoot;
	cocos2d::Sprite *m_pUp;
	
	cocos2d::Vector<cocos2d::EventListener *> m_vecEventListener;
	cocos2d::Point m_JoyStickInitPos;
	cocos2d::Point m_JoyStickRange1Pos;
	cocos2d::Point m_JoyStickRange2Pos;
	int m_JoyStickRange1Width;
	int	m_JoyStickRange2Width;
	int m_PressType;

	BaseSprite*	m_pHero;

	int m_KeyPressedValue;

	cocos2d::ValueMapIntKey m_mapPressType;
};

#endif