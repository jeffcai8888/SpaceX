#include "Macro.h"
#include "OperateLayer.h"
#include "GameLayer.h"
#include "Hero.h"
#include "SceneManager.h"

USING_NS_CC;
using namespace std;

OperateLayer::OperateLayer()
	:m_pCloseItem(nullptr),
	m_pBlood(nullptr),
	m_pBloodBg(nullptr),
	m_pDebugItem(nullptr),
    m_pHero(nullptr),
    m_pJoystick(nullptr),
    m_pJoystickBg(nullptr),
    m_pFront(nullptr),
    m_pBack(nullptr),
    m_pUp(nullptr)
{
	m_vecEventListener.clear();
}

OperateLayer::~OperateLayer()
{

}

bool OperateLayer::init()
{
	bool ret = false;
	do {
		CC_BREAK_IF( !Layer::init() );

		auto visibleSize = Director::getInstance()->getVisibleSize();
		auto m_origin = Director::getInstance()->getVisibleOrigin();

        
        m_pFront = Sprite::create("front.jpg");
        m_pBack = Sprite::create("back.jpg");
        m_pUp = Sprite::create("up.jpg");
        
        m_pFront->setPosition(m_origin + Point(visibleSize.width * 3 / 16, 50));
        m_pBack->setPosition(m_origin + Point(visibleSize.width / 16, 100));
        m_pUp->setPosition(m_origin + Point(visibleSize.width * 15 / 16, 120));
        this->addChild(m_pFront);
        this->addChild(m_pBack);
        this->addChild(m_pUp);

		m_pJoystick = Sprite::create("joystick.png");
		m_pJoystickBg = Sprite::create("joystick_bg.png");
		this->addChild(m_pJoystick, 0);
		this->addChild(m_pJoystickBg, 1);
        resetJoystick();
		
		m_pTarget = Sprite::create("target.jpg");
		this->addChild(m_pTarget, 2);

		m_pCloseItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(OperateLayer::exitApp, this));
		m_pCloseItem->setPosition(m_origin + Point(visibleSize) - Point(m_pCloseItem->getContentSize() / 2));
		auto menu = Menu::create(m_pCloseItem, NULL);
		menu->setPosition(Point::ZERO);
		this->addChild(menu, 1);

		m_pDebugItem = MenuItemImage::create("DebugNormal.png", "DebugSelected.png", CC_CALLBACK_1(OperateLayer::gotoDebug, this));
		m_pDebugItem->setPosition(m_origin + Point(visibleSize) - Point(m_pDebugItem->getContentSize().width * 2, m_pDebugItem->getContentSize().height / 2));
		auto menu_debug = Menu::create(m_pDebugItem, NULL);
		menu_debug->setPosition(Point::ZERO);
		this->addChild(menu_debug, 1);

		Sprite *pBloodSprite = Sprite::create("blood.png");
		this->m_pBlood = ProgressTimer::create(pBloodSprite);
		this->m_pBlood->setType(ProgressTimer::Type::BAR);
		this->m_pBlood->setMidpoint(Point(0, 0));
		this->m_pBlood->setBarChangeRate(Point(1, 0));
		this->m_pBlood->setAnchorPoint(Point(0, 1));
		this->m_pBlood->setPosition(m_origin + Point(2, visibleSize.height - 10));
		this->m_pBlood->setPercentage(100);

		this->m_pBloodBg = ProgressTimer::create(Sprite::create("bloodBg.png"));
		this->m_pBloodBg->setType(ProgressTimer::Type::BAR);
		this->m_pBloodBg->setMidpoint(Point(0, 0));
		this->m_pBloodBg->setBarChangeRate(Point(1, 0));
		this->m_pBloodBg->setAnchorPoint(Point(0, 1));
		this->m_pBloodBg->setPosition(this->m_pBlood->getPosition());
		this->m_pBloodBg->setPercentage(100);

		this->addChild(m_pBloodBg, 100);
		this->addChild(m_pBlood, 100);

		

		m_KeyPressedValue = 0;
		
		ret = true;

	} while(false);

	return ret;
}

void OperateLayer::onEnter()
{
	Layer::onEnter();
	m_pHero = static_cast<GameLayer *>(this->getScene()->getChildByTag(LT_Game))->getHero();
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = [this](const vector<Touch*>& touches, Event *event)
	{
		Size winSize = Director::getInstance()->getWinSize();
		vector<Touch*>::const_iterator touchIter = touches.begin();
		while (touchIter != touches.end())
		{
			Touch *pTouch = (Touch*)(*touchIter);
			Point p = pTouch->getLocation();
            
            if( p.x <= winSize.width / 8 && p.y >= 0.f && p.y <= winSize.height * 3 / 4 )
            {
                m_pHero->walk(-m_pHero->getWalkVelocity());
            }
            else if( p.x > winSize.width / 8 && p.x <= winSize.width / 4 && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
            {
                m_pHero->walk(m_pHero->getWalkVelocity());
            }
            else if ( p.x > winSize.width * 7 / 8 && p.x <= winSize.width && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
            {
                m_pHero->jump(m_pHero->getJumpVelocity());
            }
			++touchIter;
		}
	};
	listener->onTouchesMoved = [this](const vector<Touch*>& touches, Event *event)
	{
		Size winSize = Director::getInstance()->getWinSize();
		std::vector<Touch*>::const_iterator touchIter = touches.begin();
		Touch *pTouch = (Touch*)(*touchIter);
		Point start = pTouch->getStartLocation();
        Point p = pTouch->getLocation();
        if (this->isTap(m_pJoystickBg, start))
        {
            m_pHero->attack();
            Point dest = pTouch->getLocation();
            float distance = start.getDistance(dest);
            Vec2 direction = dest - start;
            direction.normalize();
            this->updateJoystick(direction, distance);
            m_pHero->setShootDirection(direction);
            this->updateTarget(direction);
        }
        else if( p.x <= winSize.width / 8 && p.y >= 0.f && p.y <= winSize.height * 3 / 4 )
        {
            m_pHero->walk(-m_pHero->getWalkVelocity());
        }
        else if( p.x > winSize.width / 8 && p.x <= winSize.width / 4 && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
        {
            m_pHero->walk(m_pHero->getWalkVelocity());
        }
        else if ( p.x > winSize.width * 7 / 8 && p.x <= winSize.width && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
        {
            m_pHero->jump(m_pHero->getJumpVelocity());
        }
	};
	listener->onTouchesEnded = [this](const vector<Touch*>& touches, Event *event)
	{
		Size winSize = Director::getInstance()->getWinSize();
		std::vector<Touch*>::const_iterator touchIter = touches.begin();
		Touch *pTouch = (Touch*)(*touchIter);
		Point start = pTouch->getStartLocation();
        if (start.x <= winSize.width / 8 && start.y >= 0.f && start.y <= winSize.height * 3 / 4)
        { 
            if(!m_pHero->isInAir())
                m_pHero->stop();
            m_pHero->stopMoveAction(MOVE_STATE_WALK);
        }
        else if (start.x > winSize.width / 8 && start.x <= winSize.width / 4 && start.y >= 0.f && start.y <= winSize.height * 3 / 4)
        {
			if (!m_pHero->isInAir())
			{
				m_pHero->stop();
			}              
            m_pHero->stopMoveAction(MOVE_STATE_WALK);
        }
        else if (start.x > winSize.width * 7 / 8 && start.x <= winSize.width && start.y >= 0.f && start.y <= winSize.height * 3 / 4)
        {
            
        }
		else
		{
			Point pos = m_pJoystickBg->getPosition();
			m_pJoystick->setPosition(pos);
			m_pHero->setIsAttacking(false);
		}

	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	m_vecEventListener.pushBack(listener);
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_pJoystick[JT_Bullet]);
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_D)
		{
			m_KeyPressedValue |= KB_Front;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_A)
		{
			m_KeyPressedValue |= KB_Back;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_W)
		{
			m_KeyPressedValue |= KB_Up;
		}
		dealWithKeyBoard();
	};
	keyListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event *event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_D)
		{
			m_KeyPressedValue &= ~KB_Front;
			m_pHero->stopMoveAction(MOVE_STATE_WALK);
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_A)
		{
			m_KeyPressedValue &= ~KB_Back;
			m_pHero->stopMoveAction(MOVE_STATE_WALK);
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_W)
		{
			m_KeyPressedValue &= ~KB_Up;
		}
		dealWithKeyBoard();
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
	m_vecEventListener.pushBack(keyListener);
}

void OperateLayer::onExit()
{
	Layer::onExit();
	removeAllEventListener();
}

void OperateLayer::showJoystick(Point pos)
{
	m_pJoystick->setPosition(pos);
	m_pJoystickBg->setPosition(pos);

	m_pJoystick->setVisible(true);
	m_pJoystickBg->setVisible(true);
}

void OperateLayer::updateJoystick(Point direction, float distance)
{
	Point start = m_pJoystickBg->getPosition();

	if(distance < 33)
	{
		m_pJoystick->setPosition(start + (direction * distance));
	//}else if(distance > 78) {
	//	m_pJoystick[type]->setPosition(start + (direction * 45));
	}else {
		m_pJoystick->setPosition(start + (direction * 33));
	}
}

void OperateLayer::resetJoystick()
{
    m_pJoystick->setPosition(Point(750, 100.f));
    m_pJoystickBg->setPosition(Point(750.f, 100.f));
}

bool OperateLayer::isTap(cocos2d::Node* pNode, cocos2d::Point point)
{
	Point pos = pNode->getPosition();
	float distance = pos.distanceSquared(point);
	Size size = pNode->getContentSize();
	if (distance < (size.width / 2) * (size.width / 2))
		return true;
	else
		return false;
}

void OperateLayer::dealWithKeyBoard()
{
	if (m_KeyPressedValue & KB_Up)
	{
		m_pHero->jump(m_pHero->getWalkVelocity());
	}
	else if (m_KeyPressedValue & KB_Front)
	{
		m_pHero->walk(m_pHero->getWalkVelocity());
	}
	else if (m_KeyPressedValue & KB_Back)
	{
		m_pHero->walk(-m_pHero->getWalkVelocity());
	}
	else
	{
		if(!m_pHero->isInAir())
			m_pHero->stop();
	}
}

void OperateLayer::updateTarget(Point pos)
{
	auto gameLayer = this->getScene()->getChildByTag(LT_Game);
	m_pTarget->setPosition(m_pHero->getPosition() + gameLayer->getPosition() + pos * 200);
	m_pTarget->setVisible(true);
}

void OperateLayer::resetTarget()
{
	auto gameLayer = this->getScene()->getChildByTag(LT_Game);
    if(m_pHero->isFlippedX())
    {
        m_pTarget->setPosition(m_pHero->getPosition() + gameLayer->getPosition() + Point(-1.f, 0.f) * 200);
    }
    else
    {
        m_pTarget->setPosition(m_pHero->getPosition() + gameLayer->getPosition() + Point(1.f, 0.f) * 200);
    }
}

void OperateLayer::exitApp(Ref* pSender)
{
	Director::getInstance()->end();
}

void OperateLayer::gotoDebug(Ref* pSender)
{
	SceneManager::getInstance()->showScene(DEBUG_SCENE, false);
}

void OperateLayer::removeAllEventListener()
{
	for (auto sp_obj : m_vecEventListener)
	{
		_eventDispatcher->removeEventListener(sp_obj);
	}
	m_vecEventListener.clear();
}