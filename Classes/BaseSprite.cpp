#include "BaseSprite.h"
#include "SocketManager.h"

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
	m_isWalking(false)
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
		if(m_JumpStage == 1)
			this->runAction(m_pJump2Action);
		else
			this->runAction(m_pJumpAction);
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
	if(changeState(ACTION_STATE_HURT))
	{
		this->runAction(m_pHurtAction);

	}
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
	this->onDeadCallback();
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

BoundingBox BaseSprite::createBoundingBox(cocos2d::Point origin, cocos2d::Size size)
{
	BoundingBox boundingBox;
	boundingBox.original.origin= origin;
	boundingBox.original.size= size;
	boundingBox.actual.origin = this->getPosition() + boundingBox.original.origin;
	boundingBox.actual.size= size;
	return boundingBox;
}
