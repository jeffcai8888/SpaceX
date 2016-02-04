#ifndef _OPERATE_LAYER_H_
#define _OPERATE_LAYER_H_

#include <cocos2d.h>

class Hero;
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

	CREATE_FUNC(OperateLayer);

	CC_SYNTHESIZE(Hero*, m_pHero, Hero);

private:
	void showJoystick(int type, cocos2d::Point pos);
	void hideJoystick(int type);
	void updateJoystick(int type, cocos2d::Point direction, float distance);
	bool isTap(cocos2d::Node*, cocos2d::Point);
	void DealWithKeyBoard();
	cocos2d::Sprite *m_pJoystick[2];
	cocos2d::Sprite *m_pJoystickBg[2];

	int m_KeyPressedValue;
};

#endif