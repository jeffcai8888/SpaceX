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
		BT_Left,
		BT_Right,
		BT_Joystick,
		BT_Jump
	};

	enum KeyBoard
	{
		KB_Front = 1 << 0,
		KB_Back = 1 << 1,
		//KB_Up = 1 << 2
	};

	OperateLayer();
	~OperateLayer();

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	void gotoDebug(Ref* pSender);
	void revivePlayer(Ref* pSender);

	CREATE_FUNC(OperateLayer);

private:
	void showJoystick(cocos2d::Point pos);
	void updateJoystick(cocos2d::Point direction, float distance);
	bool isTap(cocos2d::Node*, cocos2d::Point);
	bool isInRange(cocos2d::Point, float, float, cocos2d::Point);
	bool isInRange(cocos2d::Point, float, cocos2d::Point);
	void dealWithKeyBoard();
	void removeAllEventListener();
	void switchButtonStatus(int type, bool isPressed);
	void dealWithJoystick(cocos2d::Point, cocos2d::Point);
	cocos2d::Animation* createAnimation(const char* formatStr, int frameCount, int fps);
	cocos2d::Sprite *m_pJoystick;
	cocos2d::Sprite *m_pJoystickBg;
	cocos2d::Sprite *m_pJoystickBg1;
	cocos2d::Sprite *m_pFront;
	cocos2d::Sprite *m_pBack;
	cocos2d::Sprite *m_pUp;
	cocos2d::Sprite *m_pSkill;
	cocos2d::Sprite *m_pSkill1;

	cocos2d::Sprite *m_pRevive;
	cocos2d::Action *m_pReviveAction;


	
	cocos2d::Vector<cocos2d::EventListener *> m_vecEventListener;
	int m_PressType;

	int m_KeyPressedValue;

	bool m_isAutoShootPressed;

	cocos2d::ValueMapIntKey m_mapPressType;
};

#endif