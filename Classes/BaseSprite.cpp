#include "BaseSprite.h"
#include "SocketManager.h"
#include "SimpleAudioEngine.h"
#include "ConfigCenter.h"
#include "BulletConfigModel.h"
#include "Foresight.h"

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
	m_maxHp(100),
	m_fShootDirection(Vec2(1.f, 0.f)),
    m_isLocked(false),
	m_isAutoShoot(false),
	m_isMe(false)
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
	ProgressTimer *blood = static_cast<ProgressTimer *>(this->getChildByName("blood"));
	if(blood)
		blood->setPercentage(100.f);
}

void BaseSprite::walk(float v)
{
	if (this->isLive())
	{
		if(!this->isInAir())
			this->runWalkAction();
		if (!this->getIsAttacking() && v != 0)
			this->setFlippedX(v < 0);

		Vec2 velocity = this->getPhysicsBody()->getVelocity();
		velocity.x = v;
		this->getPhysicsBody()->setVelocity(velocity);
		CCLOG("walk %f,%f", this->getPhysicsBody()->getVelocity().x, this->getPhysicsBody()->getVelocity().y);
	}
}

void BaseSprite::jump(float v)
{

	if (this->isLive() && this->getJumpStage() < 2)
	{
		this->runJumpUpAction();
		Vec2 velocity = this->getPhysicsBody()->getVelocity();
		velocity.y = v;
		if (!this->getIsAttacking() && velocity.x != 0)
		{
			this->setFlippedX(velocity.x < 0);
		}
		this->getPhysicsBody()->setVelocity(velocity);
		CCLOG("jump Stage %d", this->getJumpStage());
		this->setPrePosition(this->getPosition());
	}
}

void BaseSprite::stop()
{
	if (this->isLive())
	{
		this->runIdleAction();
		this->getPhysicsBody()->setVelocity(Vec2(0.f, 0.f));
		CCLOG("stop %f,%f", this->getPhysicsBody()->getVelocity().x, this->getPhysicsBody()->getVelocity().y);
	}
}

void BaseSprite::attack(bool isStart)
{
	if (this->isLive())
	{
		if (isStart)
		{
			this->runAttackAction();
		}
		else
		{
			this->stopAttackAction();
		}
        
        if(m_isMe)
        {
            m_pForesight->setVisible(isStart);
            m_pRange->setVisible(isStart);
        }
        else{
            m_pForesight->setVisible(false);
            m_pRange->setVisible(false);
        }
	}
}

void BaseSprite::hurt(int damage)
{

	if (this->isLive())
	{
		this->runHurtAction();
		int hp = this->getHP() - damage;
		this->setHP(hp);
		CCLOG("HP = %d", hp);
		ProgressTimer *blood = static_cast<ProgressTimer *>(this->getChildByName("blood"));
		if (blood)
		{
			if (hp > 0)
			{
				blood->setPercentage(this->getHP() * 100.f / this->getMaxHP());
			}
		}	
	}
}



void BaseSprite::runIdleAction()
{
	if(changeState(ACTION_STATE_IDLE))
	{
		this->runAction(m_pIdleAction);
	}
}
	
void BaseSprite::runWalkAction()
{
	if (changeState(ACTION_STATE_WALK))
	{
		this->runAction(m_pWalkAction);
		m_isWalking = true;
	}
}

void BaseSprite::runJumpUpAction()
{
	changeState(ACTION_STATE_JUMP_UP);
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
}

void BaseSprite::runJumpDownAction()
{
	if (changeState(ACTION_STATE_JUMP_DOWN))
	{
		this->runAction(m_pDownAction);
	}	
}

void BaseSprite::runHurtAction()
{
	//Blink* blinkAction = Blink::create(0.1f, 1);
	//this->setVisible(true);
	//this->runAction(blinkAction);
}

void BaseSprite::runDeadAction()
{
	if(changeState(ACTION_STATE_DEAD))
	{
		this->m_hp = 0;
		this->runAction(m_pDeadAction);
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
		else if (this->getCurrActionState() == ACTION_STATE_WALK)
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
		else if (this->getCurrActionState() == ACTION_STATE_WALK)
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
    return (m_currActionState == ACTION_STATE_JUMP_UP || m_currActionState == ACTION_STATE_JUMP_DOWN);
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

cocos2d::Point BaseSprite::getShootPosition()
{
	if (this->isFlippedX())
		return getPosition() + Vec2(-15.f, -20.f);
	else
		return getPosition() + Vec2(15.f, -20.f);

}

void BaseSprite::update(float dt)
{
	float x = getPhysicsBody()->getVelocity().x;
	float y = getPhysicsBody()->getVelocity().y;
	if (!getIsOnRotateGround() && !isInSplash())
	{
		y += getGravity() * dt;
	}	
	getPhysicsBody()->setVelocity(Vec2(x, y));
	//CCLOG("update %f,%f", this->getPhysicsBody()->getVelocity().x, this->getPhysicsBody()->getVelocity().y);

	if (getCurrActionState() == ACTION_STATE_JUMP_UP && getPosition().y < getPrePosition().y)
	{
		runJumpDownAction();
	}


	if (getIsAttacking())
	{
		m_shootTime -= dt;
		if (m_shootTime < 0)
		{
			EventCustom event("shoot_bullet");
			event.setUserData(this);
			_eventDispatcher->dispatchEvent(&event);

			BulletConfigMap bulletConfigMap = ConfigCenter::getInstance()->getBulletConfigModel()->GetBulletConfigMap();
			BulletConfig config = bulletConfigMap[getBulletType()];
			m_shootTime = config.m_fInterval;
			if (getShootDirection().x != 0)
				setFlippedX(getShootDirection().x < 0);
		}
	}
	
	if (m_isMe)
	{
		m_pForesight->setPosition(getPosition() + Point(0.f, -20.f));
		float angle = CC_RADIANS_TO_DEGREES(getShootDirection().getAngle());
		m_pForesight->setRotation(-angle);
	}
}
