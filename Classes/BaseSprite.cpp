#include "BaseSprite.h"

USING_NS_CC;

BaseSprite::BaseSprite() :
	m_pIdleAction(NULL),
	m_pWalkAction(NULL),
	m_pAttackAction(NULL),
	m_pHurtAction(NULL),
	m_pDeadAction(NULL)
{

}
BaseSprite::~BaseSprite()
{
	CC_SAFE_RELEASE_NULL(m_pIdleAction);
	CC_SAFE_RELEASE_NULL(m_pWalkAction);
	CC_SAFE_RELEASE_NULL(m_pAttackAction);
	CC_SAFE_RELEASE_NULL(m_pHurtAction);
	CC_SAFE_RELEASE_NULL(m_pDeadAction);
}

void BaseSprite::runIdleAction()
{
	if(changeState(ACTION_STATE_IDLE))
	{
		this->runAction(m_pIdleAction);
        this->m_currMoveState = 0;
	}
}
	
void BaseSprite::runWalkAction()
{
	if(changeState(ACTION_STATE_MOVE))
	{
        m_currMoveState |= MOVE_STATE_WALK;
		this->runAction(m_pWalkAction);
	}
}

void BaseSprite::runJumpAction(bool isUp)
{
	if (changeState(ACTION_STATE_MOVE))
	{      
		this->runAction(m_pIdleAction);
	}
	if (isUp)
	{
		m_currMoveState &= ~MOVE_STATE_DOWN;
		m_currMoveState |= MOVE_STATE_UP;
	}
	else
	{
		m_currMoveState &= ~MOVE_STATE_UP;
		m_currMoveState |= MOVE_STATE_DOWN;
	}
}

void BaseSprite::runAttackAction()
{
	if (this->getIsAttacking() == false)
	{
		this->setIsAttacking(true);
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

void BaseSprite::removeSprite()
{
	changeState(ACTION_STATE_REMOVE);
    this->m_currMoveState = 0;
}

Animation* BaseSprite::createAnimation(const char* formatStr, int frameCount, int fps)
{
	Vector<SpriteFrame*>  vec(frameCount);
	for(int i = 0; i < frameCount; ++ i) 
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

int BaseSprite::stopMoveAction(int moveAction)
{
    m_currMoveState &= ~moveAction;
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
