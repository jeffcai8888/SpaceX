#include "Macro.h"
#include "OperateLayer.h"
#include "GameLayer.h"
#include "Hero.h"
#include "SceneManager.h"
#include "Foresight.h"
#include "JsonParser.h"

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
    m_pUp(nullptr),
    m_firstTouchJoystickLocation(Point(0.f, 0.f)),
    m_firstTouchJoystickID(-1),
	m_preTouchJoystickLocation(Point(0.f, 0.f))
{
	m_vecEventListener.clear();
	m_mapPressType.clear();
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

		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ui.plist");

        m_pFront = Sprite::createWithSpriteFrameName("right.png");
        m_pBack = Sprite::createWithSpriteFrameName("left.png");
        m_pUp = Sprite::createWithSpriteFrameName("jump.png");
		auto centerSprite = Sprite::createWithSpriteFrameName("button_bg.png");
		centerSprite->setPosition(m_origin + Point(200.f, 100.f));
        
        m_pFront->setPosition(m_origin + Point(250, 100));
        m_pBack->setPosition(m_origin + Point(150, 100));
        m_pUp->setPosition(m_origin + Point(1086, 220));
        this->addChild(m_pFront);
        this->addChild(m_pBack);
        this->addChild(m_pUp);
		this->addChild(centerSprite);
		m_pJoystick = Sprite::createWithSpriteFrameName("joystick.png");
		m_pJoystickBg = Sprite::createWithSpriteFrameName("joystick_bg.png");
		this->addChild(m_pJoystick, 0);
		this->addChild(m_pJoystickBg, 1);

		m_pCloseItem = MenuItemImage::create("pause.png", "pause_down.png", CC_CALLBACK_1(OperateLayer::gotoDebug, this));
		m_pCloseItem->setPosition(m_origin + Point(visibleSize) - Point(m_pCloseItem->getContentSize() / 2));
		auto menu = Menu::create(m_pCloseItem, NULL);
		menu->setPosition(Point::ZERO);
		this->addChild(menu, 1);

		auto bulletSprite = Sprite::createWithSpriteFrameName("1.PNG");
		bulletSprite->setPosition(50.f, 600.f);
		this -> addChild(bulletSprite);

		auto sprite1 = Sprite::createWithSpriteFrameName("2.png");
		sprite1->setPosition(500.f, 600.f);
		//this->addChild(sprite1);

		auto sprite2 = Sprite::createWithSpriteFrameName("3.png");
		sprite2->setPosition(636.f, 600.f);
		//this->addChild(sprite2);

		m_KeyPressedValue = 0;
		
		ret = true;

	} while(false);

	return ret;
}

void OperateLayer::onEnter()
{
	Layer::onEnter();
	m_pHero = static_cast<GameLayer *>(this->getScene()->getChildByTag(LT_Game))->getHero();
	m_pTarget = static_cast<GameLayer *>(this->getScene()->getChildByTag(LT_Game))->getForesight();
	float joystickScale = 1.0f;
	float joystickPosX = 0.f;
	float joystickPosY = 0.f;

	JsonParser* parser = JsonParser::createWithFile("Debug.json");
	parser->decodeDebugData();
	auto list = parser->getList();
	for (auto& v : list)
	{
		ValueMap row = v.asValueMap();

		for (auto& pair : row)
		{
			if (pair.first.compare("JoystickScale") == 0)
			{
				joystickScale = pair.second.asFloat();
			}
			else if (pair.first.compare("JoystickX") == 0)
			{
				joystickPosX = pair.second.asFloat();
			}
			else if (pair.first.compare("JoystickY") == 0)
			{
				joystickPosY = pair.second.asFloat();
			}
		}
	}

	
	//m_pJoystick->setScale(joystickScale, joystickScale);
	//m_pJoystickBg->setScale(joystickScale, joystickScale);
	m_pJoystick->setPosition(Point(joystickPosX, joystickPosY));
	m_pJoystickBg->setPosition(Point(joystickPosX, joystickPosY));
	

	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = [this](const vector<Touch*>& touches, Event *event)
	{
		Size winSize = Director::getInstance()->getWinSize();
		vector<Touch*>::const_iterator touchIter = touches.begin();
		while (touchIter != touches.end())
		{
			Touch *pTouch = (Touch*)(*touchIter);
			Point p = pTouch->getLocation();
            if (this->isTap(m_pJoystickBg, p))
            {
                m_pHero->attack();
				Vec2 direction = m_pTarget->getPosition() - m_pHero->getShootPosition();
				direction.normalize();
				m_pHero->setShootDirection(direction);
				m_firstTouchJoystickLocation = p;
				m_preTouchJoystickLocation = p;
				m_firstTouchJoystickID = pTouch->getID();
            }
            else if( p.x <= 200.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4 )
            {
                m_pHero->walk(-m_pHero->getWalkVelocity());
				switchButtonStatus(BT_Left, true);
				switchButtonStatus(BT_Right, false);
				m_mapPressType[pTouch->getID()] = Value(BT_Left);
            }
            else if( p.x > 200.f && p.x <= 400.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
            {
                m_pHero->walk(m_pHero->getWalkVelocity());
				switchButtonStatus(BT_Left, false);
				switchButtonStatus(BT_Right, true);
				m_mapPressType[pTouch->getID()] = Value(BT_Right);
            }
            else if ( p.x > 1036 && p.x <= winSize.width && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
            {
                m_pHero->jump(m_pHero->getJumpVelocity());
				switchButtonStatus(BT_Jump, true);
				m_mapPressType[pTouch->getID()] = Value(BT_Jump);
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
		if (m_firstTouchJoystickID == -1)
		{
			if (this->isTap(m_pJoystickBg, start))
			{
				m_firstTouchJoystickLocation = start;
				m_preTouchJoystickLocation = start;
				m_firstTouchJoystickID = pTouch->getID();
			}
			else if (this->isTap(m_pJoystickBg, p))
			{
				m_firstTouchJoystickLocation = p;
				m_preTouchJoystickLocation = p;
				m_firstTouchJoystickID = pTouch->getID();
			}
		}
        
        
        if(m_firstTouchJoystickID == pTouch->getID())
        {
            m_pHero->attack();
            float distance = m_firstTouchJoystickLocation.getDistance(p);
            Vec2 direction = p - m_firstTouchJoystickLocation;
            direction.normalize();
            this->updateJoystick(direction, distance);


			distance = m_preTouchJoystickLocation.getDistance(p);
			direction = p - m_preTouchJoystickLocation;
			direction.normalize();
			Vec2 v = m_pHero->getPhysicsBody()->getVelocity();
			m_pTarget->setDirection(direction * distance * m_pTarget->getVelocity());
			//float durateTime = distance * 200 / 400;
			//MoveBy* moveBy = MoveBy::create(durateTime, direction * 200);
			//m_pTarget->runAction(moveBy);
			m_pTarget->setIsStatic(false);


			direction = m_pTarget->getPosition() - m_pHero->getShootPosition();
			direction.normalize();
			m_pHero->setShootDirection(direction);
			m_preTouchJoystickLocation = p;
        }
        else if( p.x <= winSize.width / 8 && p.y >= 0.f && p.y <= winSize.height * 3 / 4 )
        {
            m_pHero->walk(-m_pHero->getWalkVelocity());
			if (m_mapPressType[pTouch->getID()].asInt() != BT_Left)
			{
				m_mapPressType[pTouch->getID()] =  Value(BT_Left);
				switchButtonStatus(BT_Left, true);
				switchButtonStatus(BT_Right, false);
			}		
        }
        else if( p.x > winSize.width / 8 && p.x <= winSize.width / 4 && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
        {
            m_pHero->walk(m_pHero->getWalkVelocity());
			if (m_mapPressType[pTouch->getID()].asInt() != BT_Right)
			{
				m_mapPressType[pTouch->getID()] =  Value(BT_Right);
				switchButtonStatus(BT_Left, false);
				switchButtonStatus(BT_Right, true);
			}
			
        }
        else if ( p.x > winSize.width * 7 / 8 && p.x <= winSize.width && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
        {
            m_pHero->jump(m_pHero->getJumpVelocity());
			switchButtonStatus(BT_Jump, true);
        }
		else
		{
			if (m_mapPressType[pTouch->getID()].asInt() == BT_Jump)
			{
				m_mapPressType.erase(pTouch->getID());
				switchButtonStatus(BT_Jump, false);
			}
		}
	};
	listener->onTouchesEnded = [this](const vector<Touch*>& touches, Event *event)
	{
		Size winSize = Director::getInstance()->getWinSize();
		std::vector<Touch*>::const_iterator touchIter = touches.begin();
		Touch *pTouch = (Touch*)(*touchIter);
		Point start = pTouch->getStartLocation();
		Point p = pTouch->getLocation();

		if (m_firstTouchJoystickID == pTouch->getID())
		{
			Point pos = m_pJoystickBg->getPosition();
			m_pJoystick->setPosition(pos);
			m_pHero->stopAttackAction();
			m_firstTouchJoystickID = -1;
		}
        else if (p.x <= winSize.width / 8 && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
        { 
            if(!m_pHero->isInAir())
                m_pHero->stop();
            m_pHero->stopMoveAction(MOVE_STATE_WALK, true);
			switchButtonStatus(BT_Left, false);
			m_mapPressType.erase(pTouch->getID());

        }
        else if (p.x > winSize.width / 8 && p.x <= winSize.width / 4 && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
        {
			if (!m_pHero->isInAir())
				m_pHero->stop();
            m_pHero->stopMoveAction(MOVE_STATE_WALK, true);
			switchButtonStatus(BT_Right, false);
			m_mapPressType.erase(pTouch->getID());
        }
        else if (p.x > winSize.width * 7 / 8 && p.x <= winSize.width && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
        {
			switchButtonStatus(BT_Jump, false);
			m_mapPressType.erase(pTouch->getID());
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
			m_pHero->stopMoveAction(MOVE_STATE_WALK, true);
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_A)
		{
			m_KeyPressedValue &= ~KB_Back;
			m_pHero->stopMoveAction(MOVE_STATE_WALK, true);
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
		m_pJoystick->setPosition(start + (direction * 65)); 
		 
	}
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
		m_pHero->jump(m_pHero->getJumpVelocity());
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

void OperateLayer::switchButtonStatus(int type, bool isPressed)
{
	if (type == BT_Left)
	{
		if (isPressed)
		{
			m_pBack->setSpriteFrame("left_down.png");
		}
		else
		{
			m_pBack->setSpriteFrame("left.png");
		}
	}
	else if (type == BT_Right)
	{
		if (isPressed)
		{
			m_pFront->setSpriteFrame("right_down.png");
		}
		else
		{
			m_pFront->setSpriteFrame("right.png");
		}
	}
	else if (type == BT_Jump)
	{
		if (isPressed)
		{
			m_pUp->setSpriteFrame("jump_down.png");
		}
		else
		{
			m_pUp->setSpriteFrame("jump.png");
		}
	}
}