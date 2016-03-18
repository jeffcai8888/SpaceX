#include "BaseSprite.h"
#include "SocketManager.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

BaseSprite::BaseSprite() :
	m_pIdleAction(NULL),
	m_pWalkAction(NULL),
	m_pHurtAction(NULL),
	m_pDeadAction(NULL),
	m_pJumpAction(NULL),
	m_pJump2Action(NULL),
	m_pDownAction(NULL),
	m_pWalkFireAction(NULL),
	m_pIdleFireAction(NULL),
	m_isWalking(false),
	m_isOnRotateGround(false),
	m_maxHp(100)
{

}

BaseSprite::~BaseSprite()
{
	CC_SAFE_RELEASE_NULL(m_pIdleAction);
	CC_SAFE_RELEASE_NULL(m_pWalkAction);
	CC_SAFE_RELEASE_NULL(m_pHurtAction);
	CC_SAFE_RELEASE_NULL(m_pDeadAction);
	CC_SAFE_RELEASE_NULL(m_pJumpAction);
	CC_SAFE_RELEASE_NULL(m_pJump2Action);
	CC_SAFE_RELEASE_NULL(m_pDownAction);
	CC_SAFE_RELEASE_NULL(m_pWalkFireAction);
	CC_SAFE_RELEASE_NULL(m_pIdleFireAction);
}

void BaseSprite::reset()
{
	this->setPosition(m_initPos);
	this->runIdleAction();
	this->setHP(100);
	this->setIsAttacking(false);
	this->setJumpStage(0);
}

void BaseSprite::walk(float v)
{
	if (this->isLive())
	{
		bool isWalking = this->getIsWalk();
		this->runWalkAction(!this->isInAir() && !isWalking);
		if (!this->getIsAttacking() && v != 0)
			this->setFlippedX(v < 0);

		Vec2 velocity = this->getPhysicsBody()->getVelocity();
		velocity.x = v;
		this->getPhysicsBody()->setVelocity(velocity);
	}
}

void BaseSprite::jump(float v)
{

	if (this->isLive() && this->getJumpStage() < 2)
	{
		this->runJumpAction(true);
		//m_pHero->setJumpStage(m_pHero->getJumpStage() + 1);

		Vec2 velocity = this->getPhysicsBody()->getVelocity();
		velocity.y = v;
		if (!this->getIsAttacking() && velocity.x != 0)
		{
			this->setFlippedX(velocity.x < 0);
		}
		this->getPhysicsBody()->setVelocity(velocity);
		this->setPrePosition(this->getPosition());
	}
}

void BaseSprite::stop()
{
	if (this->isLive())
	{
		this->runIdleAction();
		this->getPhysicsBody()->setVelocity(Vec2(0.f, 0.f));
	}
}

void BaseSprite::attack()
{
	if (this->isLive())
	{
		this->runAttackAction();
	}
}

void BaseSprite::hurt(int damage)
{

	if (this->isLive())
	{
		this->runHurtAction();
		int hp = this->getHP() - damage;
		this->setHP(hp);
		ProgressTimer *blood = static_cast<ProgressTimer *>(this->getChildByName("blood"));
		if (hp < 0)
		{
			reset();
			blood->setPercentage(100.f);
		}
		else
		{
			blood->setPercentage(this->getHP() * 100.f / this->getMaxHP());
		}
	}
}



void BaseSprite::runIdleAction()
{
	if(changeState(ACTION_STATE_IDLE))
	{
		this->runAction(m_pIdleAction);
        this->m_currMoveState = 0;

		if (SocketManager::getInstance()->getNetworkType() == NT_Server)
		{
			SocketManager::getInstance()->sendData(NDT_HeroStop, getPosition(), getPhysicsBody()->getVelocity());
		}
	}
}
	
void BaseSprite::runWalkAction(bool isPlayAnim)
{
	changeState(ACTION_STATE_MOVE);
	m_currMoveState |= MOVE_STATE_WALK;

	if (isPlayAnim)
	{
		this->runAction(m_pWalkAction);
		m_isWalking = true;
	}
		
	if (SocketManager::getInstance()->getNetworkType() == NT_Server)
	{
		SocketManager::getInstance()->sendData(NDT_HeroWalk, getPosition(), getPhysicsBody()->getVelocity());
	}
}

void BaseSprite::runJumpAction(bool isUp)
{
	changeState(ACTION_STATE_MOVE);
	if (isUp)
	{
		stopMoveAction(MOVE_STATE_UP, true);
		stopMoveAction(MOVE_STATE_WALK, false);
		stopMoveAction(MOVE_STATE_DOWN, true);
		if (m_JumpStage == 1)
		{
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("jump.mp3");
			this->runAction(m_pJump2Action);
		}
			
		else
		{
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("jump.mp3");
			this->runAction(m_pJumpAction);
		}
			
		++m_JumpStage;
		m_currMoveState |= MOVE_STATE_UP;		
	}
	else
	{
		stopMoveAction(MOVE_STATE_DOWN, true);
		stopMoveAction(MOVE_STATE_WALK, false);
		stopMoveAction(MOVE_STATE_UP, true);
		m_currMoveState |= MOVE_STATE_DOWN;
		this->runAction(m_pDownAction);
	}

	if (SocketManager::getInstance()->getNetworkType() == NT_Server)
	{
		if(isUp)
			SocketManager::getInstance()->sendData(NDT_HeroJumpUp, getPosition(), getPhysicsBody()->getVelocity());
		else
			SocketManager::getInstance()->sendData(NDT_HeroJumpDown, getPosition(), getPhysicsBody()->getVelocity());
	}
}

void BaseSprite::runHurtAction()
{
	Blink* blinkAction = Blink::create(0.1f, 1);
	this->setVisible(true);
	this->runAction(blinkAction);
}

void BaseSprite::runDeadAction()
{
	if(changeState(ACTION_STATE_DEAD))
	{
		this->m_hp = 0;
		this->runAction(m_pDeadAction);
        this->m_currMoveState = 0;
	}
}

void BaseSprite::runAttackAction()
{
	if (this->getIsAttacking() == false)
	{
		this->setIsAttacking(true);
		if (this->getCurrActionState() == ACTION_STATE_IDLE)
		{
			this->stopAction(m_pIdleAction);
			this->runAction(m_pIdleFireAction);
		}
		else if (this->getCurrActionState() == ACTION_STATE_MOVE && this->getCurrMoveState() == MOVE_STATE_WALK)
		{
			this->stopAction(m_pWalkAction);
			this->m_isWalking = false;
			this->runAction(m_pWalkFireAction);
		}
	}
}

void BaseSprite::stopAttackAction()
{
	if (this->getIsAttacking() == true)
	{
		this->setIsAttacking(false);
		if (this->getCurrActionState() == ACTION_STATE_IDLE)
		{
			this->stopAction(m_pIdleAction);
			this->stopAction(m_pIdleFireAction);
			this->runAction(m_pIdleAction);
			
		}
		else if (this->getCurrActionState() == ACTION_STATE_MOVE && this->getCurrMoveState() == MOVE_STATE_WALK)
		{
			this->stopAction(m_pWalkFireAction);
			if (this->m_isWalking == false)
			{
				this->runAction(m_pWalkAction);
				this->m_isWalking = true;
			}		
		}
	}
}

void BaseSprite::removeSprite()
{
	changeState(ACTION_STATE_REMOVE);
    this->m_currMoveState = 0;
}

Animation* BaseSprite::createAnimation(const char* formatStr, int frameCount, int fps)
{
	Vector<SpriteFrame*>  vec(frameCount);
	for(int i = 1; i <= frameCount; ++ i) 
	{
		const char* imgName = String::createWithFormat(formatStr, i)->getCString();
		SpriteFrame *pFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(imgName);
		vec.pushBack(pFrame);
	}
	return Animation::createWithSpriteFrames(vec, 1.0f / fps);
}

CallFunc* BaseSprite::createDeadCallbackFunc()
{
	return CallFunc::create( CC_CALLBACK_0(BaseSprite::onDead, this));
}

CallFunc* BaseSprite::createIdleCallbackFunc()
{
	return  CallFunc::create(CC_CALLBACK_0(BaseSprite::runIdleAction, this));
}

void BaseSprite::onDead()
{

}

bool BaseSprite::isLive()
{
	if(this->m_currActionState >= ACTION_STATE_DEAD)
	{
		return false;
	}else {
		return true;
	}
}

bool BaseSprite::isInAir()
{
    return (this->m_currActionState == ACTION_STATE_MOVE) && this->isInMoveAction(MOVE_STATE_UP | MOVE_STATE_DOWN);
}

bool BaseSprite::changeState(ActionState actionState)
{
	if((m_currActionState == ACTION_STATE_DEAD && actionState != ACTION_STATE_REMOVE) || m_currActionState == actionState)
	{
		return false;
	}

	this->stopAllActions();
	this->m_currActionState = actionState;
	if(actionState == ACTION_STATE_REMOVE)
		return false;
	else
		return true;
}

int BaseSprite::stopMoveAction(int moveAction, bool clear)
{
	if(clear)
		m_currMoveState &= ~moveAction;

	if (moveAction == MOVE_STATE_WALK)
	{
		this->m_isWalking = false;
		this->stopAction(m_pWalkAction);
	}		
	else if (moveAction == MOVE_STATE_DOWN)
		this->stopAction(m_pDownAction);
	else if (moveAction == MOVE_STATE_UP)
	{
		this->stopAction(m_pJumpAction);
		this->stopAction(m_pJump2Action);
	}
		
    return m_currMoveState;
}

bool BaseSprite::isInMoveAction(int moveAction)
{
    return ((m_currMoveState & moveAction) > 0);
}

cocos2d::Point BaseSprite::getShootPosition()
{
	if (this->isFlippedX())
		return getPosition() + Vec2(-15.f, -20.f);
	else
		return getPosition() + Vec2(15.f, -20.f);

}
