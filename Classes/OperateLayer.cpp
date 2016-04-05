#include "Macro.h"
#include "OperateLayer.h"
#include "GameLayer.h"
#include "Hero.h"
#include "SceneManager.h"
#include "Foresight.h"
#include "JsonParser.h"
#include "SocketManager.h"

USING_NS_CC;
using namespace std;

OperateLayer::OperateLayer()
	:m_pHero(nullptr),
    m_pJoystick(nullptr),
    m_pJoystickBg(nullptr),
    m_pShoot(nullptr),
    m_pUp(nullptr),
    m_firstTouchJoystickID(-1)
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

        m_pUp = Sprite::createWithSpriteFrameName("jump.png");     
        m_pUp->setPosition(m_origin + Point(1086, 220));
        this->addChild(m_pUp);
        
        m_pShoot = Sprite::createWithSpriteFrameName("1.PNG");
        m_pShoot->setPosition(Point(900.f, 100.f));
        this->addChild(m_pShoot);
        
		m_pJoystick = Sprite::createWithSpriteFrameName("joystick.png");
		m_pJoystickBg = Sprite::createWithSpriteFrameName("joystick_bg.png");

		calJoyStickPos(m_JoyStickInitPos);
		showJoystick(m_JoyStickInitPos);
		this->addChild(m_pJoystick, 0);
		this->addChild(m_pJoystickBg, 1);
		Menu* menu;
		auto debugItem = MenuItemImage::create("pause.png", "pause_down.png", CC_CALLBACK_1(OperateLayer::gotoDebug, this));
		debugItem->setPosition(m_origin + Point(visibleSize) - Point(debugItem->getContentSize() / 2));

		if (SocketManager::getInstance()->getNetworkType() == NT_Server)
		{
			std::string ipAddr = SocketManager::getInstance()->getIPAddress();
			auto lblIpAddr = Label::createWithSystemFont(ipAddr, "Arial", 24.f);
			auto menuIpAddr = MenuItemLabel::create(lblIpAddr, nullptr);
			menuIpAddr->setPosition(Vec2(75.f, 25.f));
			menu = Menu::create(debugItem, menuIpAddr, NULL);
		}
		else
		{
			menu = Menu::create(debugItem, NULL);
		}

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

		m_pArraw = Sprite::createWithSpriteFrameName("arrow.png");
		m_pArraw->setScale(0.5f);
		
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
	float joystickRange1X = 0.f;
	float joystickRange1Y = 0.f;
	float joystickRange2X = 0.f;
	float joystickRange2Y = 0.f;

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
			else if (pair.first.compare("JoystickRangeW1") == 0)
			{
				m_JoyStickRange1Width = pair.second.asFloat();
			}
			else if (pair.first.compare("JoystickRangeW2") == 0)
			{
				m_JoyStickRange2Width = pair.second.asFloat();
			}
			else if (pair.first.compare("JoystickRangeX1") == 0)
			{
				joystickRange1X = pair.second.asFloat();
			}
			else if (pair.first.compare("JoystickRangeY1") == 0)
			{
				joystickRange1Y = pair.second.asFloat();
			}
			else if (pair.first.compare("JoystickRangeX2") == 0)
			{
				joystickRange2X = pair.second.asFloat();
			}
			else if (pair.first.compare("JoystickRangeY2") == 0)
			{
				joystickRange2Y = pair.second.asFloat();
			}
		}
	}
	m_JoyStickInitPos.setPoint(joystickPosX, joystickPosY);
	m_JoyStickRange1Pos.setPoint(joystickRange1X, joystickRange1Y);
	m_JoyStickRange2Pos.setPoint(joystickRange2X, joystickRange2Y);
	
	//m_pJoystick->setScale(joystickScale, joystickScale);
	//m_pJoystickBg->setScale(joystickScale, joystickScale);
	calJoyStickPos(m_JoyStickInitPos);
	showJoystick(m_JoyStickInitPos);
	

	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = [this](const vector<Touch*>& touches, Event *event)
	{
		Size winSize = Director::getInstance()->getWinSize();
		vector<Touch*>::const_iterator touchIter = touches.begin();
		while (touchIter != touches.end())
		{
			Touch *pTouch = (Touch*)(*touchIter);
			Point p = pTouch->getLocation();
			if (isInRange(m_JoyStickRange2Pos, m_JoyStickRange2Width, p))
			{
				m_firstTouchJoystickID = pTouch->getID();
				Point touchPos = p;
				calJoyStickPos(p);
				showJoystick(p);
				if (!isInRange(m_JoyStickRange1Pos, m_JoyStickRange1Width, touchPos))
				{
					dealWithJoystick(p, touchPos);
				}
			}
            else if(this->isTap(m_pShoot, p))
            {
                m_pHero->attack(true);
                SocketManager::getInstance()->sendData(NDT_HeroAttack, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getShootDirection());
                m_mapPressType[pTouch->getID()] = Value(BT_Shoot);
            }
            /*else if( p.x <= 200.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4 )
            {
                m_pHero->walk(-m_pHero->getWalkVelocity());
				SocketManager::getInstance()->sendData(NDT_HeroWalk, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
				switchButtonStatus(BT_Left, true);
				switchButtonStatus(BT_Right, false);
				m_mapPressType[pTouch->getID()] = Value(BT_Left);
            }
            else if( p.x > 200.f && p.x <= 400.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
            {
                m_pHero->walk(m_pHero->getWalkVelocity());
				SocketManager::getInstance()->sendData(NDT_HeroWalk, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
				switchButtonStatus(BT_Left, false);
				switchButtonStatus(BT_Right, true);
				m_mapPressType[pTouch->getID()] = Value(BT_Right);
            }*/
            else if ( p.x > 1036 && p.x <= winSize.width && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
            {
                m_pHero->jump(m_pHero->getJumpVelocity());
				SocketManager::getInstance()->sendData(NDT_HeroJumpUp, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
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
		
        if(m_firstTouchJoystickID == pTouch->getID())
        {
			dealWithJoystick(m_pJoystickBg->getPosition(), p);
        }
        else if(this->isTap(m_pShoot, p))
        {
            m_pHero->attack(true);
        }
        /*else if( p.x <= winSize.width / 8 && p.y >= 0.f && p.y <= winSize.height * 3 / 4 )
        {
            m_pHero->walk(-m_pHero->getWalkVelocity());
			SocketManager::getInstance()->sendData(NDT_HeroWalk, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
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
			SocketManager::getInstance()->sendData(NDT_HeroWalk, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
			if (m_mapPressType[pTouch->getID()].asInt() != BT_Right)
			{
				m_mapPressType[pTouch->getID()] =  Value(BT_Right);
				switchButtonStatus(BT_Left, false);
				switchButtonStatus(BT_Right, true);
			}
			
        }*/
        else if ( p.x > winSize.width * 7 / 8 && p.x <= winSize.width && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
        {
            m_pHero->jump(m_pHero->getJumpVelocity());
			SocketManager::getInstance()->sendData(NDT_HeroJumpUp, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
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
			//Point pos = m_pJoystickBg->getPosition();
			//m_pJoystick->setPosition(pos);
			showJoystick(m_JoyStickInitPos);
			m_firstTouchJoystickID = -1;
            
            if (!m_pHero->isInAir())
            {
                m_pHero->stop();
                SocketManager::getInstance()->sendData(NDT_HeroStop, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), Vec2(0, 0));
            }
            m_pHero->stopMoveAction(MOVE_STATE_WALK, true);
        }
        else if(this->isTap(m_pShoot, p))
        {
            m_pHero->attack(false);
        }
        /*else if (p.x <= winSize.width / 8 && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
        { 
			if (!m_pHero->isInAir())
			{
				m_pHero->stop();
				SocketManager::getInstance()->sendData(NDT_HeroStop, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), Vec2(0, 0));
			}               
            m_pHero->stopMoveAction(MOVE_STATE_WALK, true);
			switchButtonStatus(BT_Left, false);
			m_mapPressType.erase(pTouch->getID());

        }
        else if (p.x > winSize.width / 8 && p.x <= winSize.width / 4 && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
        {
			if (!m_pHero->isInAir())
			{
				m_pHero->stop();
				SocketManager::getInstance()->sendData(NDT_HeroStop, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), Vec2(0, 0));
			}				
            m_pHero->stopMoveAction(MOVE_STATE_WALK, true);
			switchButtonStatus(BT_Right, false);
			m_mapPressType.erase(pTouch->getID());
        }*/
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
		/*if (keyCode == EventKeyboard::KeyCode::KEY_D)
		{
			m_KeyPressedValue |= KB_Front;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_A)
		{
			m_KeyPressedValue |= KB_Back;
		}*/
        if(keyCode == EventKeyboard::KeyCode::KEY_S)
        {
            m_KeyPressedValue |= KB_Shoot;
        }
		else if (keyCode == EventKeyboard::KeyCode::KEY_W)
		{
			m_KeyPressedValue |= KB_Up;
		}
		dealWithKeyBoard();
	};
	keyListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event *event)
	{
		/*if (keyCode == EventKeyboard::KeyCode::KEY_D)
		{
			m_KeyPressedValue &= ~KB_Front;
			m_pHero->stopMoveAction(MOVE_STATE_WALK, true);
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_A)
		{
			m_KeyPressedValue &= ~KB_Back;
			m_pHero->stopMoveAction(MOVE_STATE_WALK, true);
		}*/
        if (keyCode == EventKeyboard::KeyCode::KEY_S)
        {
            m_KeyPressedValue &= ~KB_Shoot;
        }
		else if (keyCode == EventKeyboard::KeyCode::KEY_W)
		{
			m_KeyPressedValue &= ~KB_Up;
		}
		dealWithKeyBoard();
	};
	m_pHero->addChild(m_pArraw);
	m_pArraw->setPosition(m_pHero->getShootPosition());
	m_pArraw->setVisible(true);

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

void OperateLayer::dealWithJoystick(cocos2d::Point centerPoint, cocos2d::Point p)
{
	float distance = centerPoint.getDistance(p);
	Vec2 direction = p - centerPoint;
	direction.normalize();
	this->updateJoystick(direction, distance);
	m_pHero->setShootDirection(direction);
	float angle = CC_RADIANS_TO_DEGREES(direction.getAngle());
	if ((angle > -75.f &&  angle < 75.f) || angle > 105.f || angle < -105.f)
	{
		if (direction.x < 0)
		{
			m_pHero->walk(-m_pHero->getWalkVelocity());
			SocketManager::getInstance()->sendData(NDT_HeroWalk, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
		}
		else
		{
			m_pHero->walk(m_pHero->getWalkVelocity());
			SocketManager::getInstance()->sendData(NDT_HeroWalk, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
		}
	}
	else
	{
		if (!m_pHero->isInAir())
		{
			m_pHero->stop();
			SocketManager::getInstance()->sendData(NDT_HeroStop, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), Vec2(0, 0));
		}
		m_pHero->stopMoveAction(MOVE_STATE_WALK, true);
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

bool OperateLayer::isInRange(cocos2d::Point p1, int w, cocos2d::Point p2)
{
	return (p2.x >= (p1.x - w /2) && p2.x <= (p1.x + w / 2) && p2.y >= (p1.y - w / 2) && p2.y <= (p1.y + w / 2));
}

void OperateLayer::calJoyStickPos(cocos2d::Point& pos)
{
	if (pos.x - m_pJoystickBg->getContentSize().width / 2 < m_JoyStickRange1Pos.x - m_JoyStickRange1Width / 2)
		pos.x = m_JoyStickRange1Pos.x - m_JoyStickRange1Width / 2 + m_pJoystickBg->getContentSize().width / 2;
	else if (pos.x + m_pJoystickBg->getContentSize().width / 2 > m_JoyStickRange1Pos.x + m_JoyStickRange1Width / 2)
		pos.x = m_JoyStickRange1Pos.x + m_JoyStickRange1Width / 2 - m_pJoystickBg->getContentSize().width / 2;

	if (pos.y - m_pJoystickBg->getContentSize().height / 2 < m_JoyStickRange1Pos.y - m_JoyStickRange1Width / 2)
		pos.y = m_JoyStickRange1Pos.y - m_JoyStickRange1Width / 2 + m_pJoystickBg->getContentSize().height / 2;
	else if (pos.y + m_pJoystickBg->getContentSize().height / 2 > m_JoyStickRange1Pos.y + m_JoyStickRange1Width / 2)
		pos.y = m_JoyStickRange1Pos.y + m_JoyStickRange1Width / 2 - m_pJoystickBg->getContentSize().height / 2;
}

void OperateLayer::dealWithKeyBoard()
{
	/*if (m_KeyPressedValue & KB_Up)
	{
		m_pHero->jump(m_pHero->getJumpVelocity());
		SocketManager::getInstance()->sendData(NDT_HeroJumpUp, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
	}
	else if (m_KeyPressedValue & KB_Front)
	{
		m_pHero->walk(m_pHero->getWalkVelocity());
		SocketManager::getInstance()->sendData(NDT_HeroWalk, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
	}
	else if (m_KeyPressedValue & KB_Back)
	{
		m_pHero->walk(-m_pHero->getWalkVelocity());
		SocketManager::getInstance()->sendData(NDT_HeroWalk, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
	}
	else
	{
		if (!m_pHero->isInAir())
		{
			m_pHero->stop();
			SocketManager::getInstance()->sendData(NDT_HeroStop, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), Vec2(0, 0));
		}
	}*/
    
    if(m_KeyPressedValue & KB_Shoot)
    {
        m_pHero->attack(true);
        SocketManager::getInstance()->sendData(NDT_HeroAttack, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getShootDirection());
    }
    else
    {
        m_pHero->attack(false);
        SocketManager::getInstance()->sendData(NDT_HeroStopAttack, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getShootDirection());
    }
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
	/*if (type == BT_Left)
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
	else*/
    if (type == BT_Jump)
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