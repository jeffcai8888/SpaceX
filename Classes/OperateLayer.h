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

	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *unused_event);

	CC_SYNTHESIZE(Hero*, m_pHero, Hero);

private:
	void showJoystick(int type, cocos2d::Point pos);
	void hideJoystick(int type);
	void updateJoystick(int type, cocos2d::Point direction, float distance);

	cocos2d::Sprite *m_pJoystick[2];
	cocos2d::Sprite *m_pJoystickBg[2];
};

#endif