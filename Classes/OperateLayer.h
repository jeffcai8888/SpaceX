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
	cocos2d::Sprite *m_pJoystick[2];
	cocos2d::Sprite *m_pJoystickBg[2];
};

#endif