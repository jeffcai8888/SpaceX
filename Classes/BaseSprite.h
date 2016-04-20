#ifndef _SPACEX_CLASSES_BASESPRITE_H_
#define _SPACEX_CLASSES_BASESPRITE_H_

#include <cocos2d.h>

typedef enum {
	ACTION_STATE_NONE = 0,
	ACTION_STATE_IDLE,
	ACTION_STATE_WALK,
	ACTION_STATE_JUMP_UP,
	ACTION_STATE_JUMP_DOWN,
	ACTION_STATE_HURT,
	ACTION_STATE_DEAD,
	ACTION_STATE_REMOVE,
}ActionState;

enum {
	ROLE_NONE = -1,
	ROLE_HERO,
	ROLE_GUN,
	ROLE_PRINCESS
};


class Bullet;
class Foresight;
class BaseSprite : public cocos2d::Sprite
{
public:
	BaseSprite();
	~BaseSprite();

	void runIdleAction();
	void runWalkAction();
	void runJumpUpAction();
	void runJumpDownAction();
	void runHurtAction();
	void removeSprite();
	void runDeadAction();
	void runAttackAction();
	void stopAttackAction();

	virtual bool isInSplash() = 0;

	cocos2d::Point getShootPosition();

	void walk(float);
	void jump(float);
	void stop();
	void hurt(int);
	void attack(bool);
	virtual void activeSkill1() {}
	virtual void activeSkill2() {}

	void reset();

	virtual void update(float dt);


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
	CC_SYNTHESIZE(bool, m_isMe, IsMe);

	CC_SYNTHESIZE(unsigned int, m_maxHp, MaxHP);
	CC_SYNTHESIZE(unsigned int, m_hp, HP);
	CC_SYNTHESIZE(bool, m_isAttacking, IsAttacking);
	CC_SYNTHESIZE(cocos2d::Vec2, m_fShootDirection, ShootDirection);
    CC_SYNTHESIZE(cocos2d::Vec2, m_fPrePosition, PrePosition);
	CC_SYNTHESIZE(int, m_JumpStage, JumpStage);
	CC_SYNTHESIZE(bool, m_isWalking, IsWalk);

	CC_SYNTHESIZE(float, m_walkVelocity, WalkVelocity);
	CC_SYNTHESIZE(float, m_jumpVelocity, JumpVelocity);
	CC_SYNTHESIZE(int, m_ammoCapacity, AmmoCapacity);
	CC_SYNTHESIZE(float, m_fGravity, Gravity);
	CC_SYNTHESIZE(bool, m_isOnRotateGround, IsOnRotateGround);
	CC_SYNTHESIZE(cocos2d::Point, m_initPos, InitPos);
    CC_SYNTHESIZE(bool, m_isLocked, IsLocked);
	CC_SYNTHESIZE(cocos2d::Sprite*, m_pLockMark, LockMark);
	CC_SYNTHESIZE(std::string, m_bulletType, BulletType);
	CC_SYNTHESIZE(bool, m_isAutoShoot, IsAutoShoot);
	CC_SYNTHESIZE(Foresight*, m_pForesight, Foresight);
	CC_SYNTHESIZE(cocos2d::Sprite*, m_pRange, Range);
	
	CC_SYNTHESIZE(int, m_curSkill, CurSkill);
    
	virtual void onDead();

	virtual bool isLive();
	virtual bool isInAir();

	cocos2d::CallFunc* createDeadCallbackFunc();
	cocos2d::CallFunc* createIdleCallbackFunc();

protected:
	static cocos2d::Animation* createAnimation(const char* formatStr, int frameCount, int fps);

private:
	bool changeState(ActionState actionState);
	float m_shootTime;
};

#endif