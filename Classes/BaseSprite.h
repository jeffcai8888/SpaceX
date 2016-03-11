#ifndef _SPACEX_CLASSES_BASESPRITE_H_
#define _SPACEX_CLASSES_BASESPRITE_H_

#include <cocos2d.h>
#include "extensions/cocos-ext.h"

typedef enum {
	ACTION_STATE_NONE = 0,
	ACTION_STATE_IDLE,
	ACTION_STATE_MOVE,
	ACTION_STATE_HURT,
	ACTION_STATE_DEAD,
	ACTION_STATE_REMOVE,
}ActionState;

enum {
    MOVE_STATE_WALK = 1 << 0,
    MOVE_STATE_UP = 1 << 1,
    MOVE_STATE_DOWN = 1 << 2
};

enum {
	ACTION_TAG_IDEL = 0,
	ACTION_TAG_WALK,
	ACTION_TAG_UP,
	ACTION_TAG_DOWN,
	ACTION_TAG_WALK_ATTACK,
	ACTION_TAG_JUMP_ATTACK
};

typedef struct _BoundingBox
{
	cocos2d::Rect actual;
	cocos2d::Rect original;
}BoundingBox;

class BaseSprite : public cocos2d::extension::PhysicsSprite
{
public:
	BaseSprite();
	~BaseSprite();

	void runIdleAction();
	void runWalkAction(bool isPlayAnim);
	void runJumpAction(bool isUp);
	void runHurtAction();
	void removeSprite();
	void runDeadAction();
	void runAttackAction();
	void stopAttackAction();
	void runWalkFireAction();
	void runIdleFireAction();
    
    int  stopMoveAction(int moveAction, bool clear);
    bool isInMoveAction(int moveAction);


	CC_SYNTHESIZE_RETAIN(cocos2d::Action*, m_pIdleAction, IdleAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::Action*, m_pWalkAction, WalkAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::Action*, m_pHurtAction, HurtAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::Action*, m_pDeadAction, DeadAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::Action*, m_pJumpAction, JumpAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::Action*, m_pJump2Action, Jump2Action);
	CC_SYNTHESIZE_RETAIN(cocos2d::Action*, m_pDownAction, DownAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::Action*, m_pWalkFireAction, WalkFireAction);
	CC_SYNTHESIZE_RETAIN(cocos2d::Action*, m_pIdleFireAction, IdleFireAction);

	CC_SYNTHESIZE(ActionState, m_currActionState, CurrActionState);
    CC_SYNTHESIZE(int, m_currMoveState, CurrMoveState);
	CC_SYNTHESIZE(unsigned int, m_hp, HP);
	CC_SYNTHESIZE(bool, m_isAttacking, IsAttacking);
	CC_SYNTHESIZE(cocos2d::Vec2, m_fShootDirection, ShootDirection);
    CC_SYNTHESIZE(cocos2d::Vec2, m_fPrePosition, PrePosition);
	CC_SYNTHESIZE(int, m_JumpStage, JumpStage);
	CC_SYNTHESIZE(bool, m_isWalking, IsWalk);

	virtual void onDead();

	virtual bool isLive();
	virtual bool isInAir();

	cocos2d::CallFunc* createDeadCallbackFunc();
	cocos2d::CallFunc* createIdleCallbackFunc();

	std::function<void(void)> onDeadCallback;
	std::function<void(void)> attack;

protected:
	static cocos2d::Animation* createAnimation(const char* formatStr, int frameCount, int fps);


	BoundingBox createBoundingBox(cocos2d::Point origin, cocos2d::Size size);

private:
	bool changeState(ActionState actionState);
};

#endif