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
		m_pJoystickBg1 = Sprite::createWithSpriteFrameName("joystick_bg1.png");
		this->addChild(m_pJoystick, 0);
		this->addChild(m_pJoystickBg1, 1);
		this->addChild(m_pJoystickBg, 2);

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
		
		m_KeyPressedValue = 0;
		
		ret = true;

	} while(false);

	return ret;
}

void OperateLayer::onEnter()
{
	Layer::onEnter();
	m_pHero = static_cast<GameLayer *>(this->getScene()->getChildByTag(LT_Game))->getHero();
    m_pForesight = static_cast<GameLayer *>(this->getScene()->getChildByTag(LT_Game))->getForesight();
    m_pRange = static_cast<GameLayer *>(this->getScene()->getChildByTag(LT_Game))->getRange();
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
    showJoystick(Point(joystickPosX, joystickPosY));
	
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = [this](const vector<Touch*>& touches, Event *event)
	{
		Size winSize = Director::getInstance()->getWinSize();
		vector<Touch*>::const_iterator touchIter = touches.begin();
		while (touchIter != touches.end())
		{
			Touch *pTouch = (Touch*)(*touchIter);
			Point p = pTouch->getLocation();
			if(isInRange(m_pJoystickBg->getPosition(), m_pJoystickBg->getContentSize().width * 1.5, p))
			{
				m_mapPressType[pTouch->getID()] = Value(BT_Joystick);
                m_pHero->attack(true);
                SocketManager::getInstance()->sendData(NDT_HeroAttack, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getShootDirection());
                m_pHero->setIsLocked(true);
                m_pForesight->setVisible(true);
                m_pRange->setVisible(true);
				switchButtonStatus(BT_Joystick, true);
			}
			else if (p.x <= 200.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
			{
				m_pHero->walk(-m_pHero->getWalkVelocity());
				SocketManager::getInstance()->sendData(NDT_HeroWalk, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
				switchButtonStatus(BT_Left, true);
				m_mapPressType[pTouch->getID()] = Value(BT_Left);
			}
			else if (p.x > 200.f && p.x <= 400.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
			{
				m_pHero->walk(m_pHero->getWalkVelocity());
				SocketManager::getInstance()->sendData(NDT_HeroWalk, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
				switchButtonStatus(BT_Right, true);
				m_mapPressType[pTouch->getID()] = Value(BT_Right);
			}
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
        float diff = start.getDistanceSq(p);
        if(diff < 400.f)
            return;
		
        if(m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Joystick)
        {
			if (isInRange(m_pJoystickBg->getPosition(), m_pJoystickBg->getContentSize().width * 3, p))
				dealWithJoystick(m_pJoystickBg->getPosition(), p);
			else
			{
                showJoystick(m_pJoystickBg->getPosition());
				m_mapPressType.erase(pTouch->getID());
                m_pForesight->setVisible(false);
                m_pRange->setVisible(false);
                m_pHero->attack(false);
				switchButtonStatus(BT_Joystick, false);
                SocketManager::getInstance()->sendData(NDT_HeroStopAttack, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getShootDirection());
			}
            m_pHero->setIsLocked(false);
        }
		
		bool isChange = false;
		if (m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Left)
		{
			if (!(p.x <= winSize.width / 8 && p.y >= 0.f && p.y <= winSize.height * 3 / 4))
			{
				switchButtonStatus(BT_Left, false);
				m_mapPressType.erase(pTouch->getID());
				isChange = true;
			}
		}
		else if (p.x <= winSize.width / 8 && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
		{
			m_mapPressType[pTouch->getID()] = Value(BT_Left);
			switchButtonStatus(BT_Left, true);
			isChange = true;
		}

		if (m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Right)
		{
			if (!(p.x > winSize.width / 8 && p.x <= winSize.width / 4 && p.y >= 0.f && p.y <= winSize.height * 3 / 4))
			{
				switchButtonStatus(BT_Right, false);
				m_mapPressType.erase(pTouch->getID());
				isChange = true;
			}
		}
		else if (p.x > winSize.width / 8 && p.x <= winSize.width / 4 && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
		{
			m_mapPressType[pTouch->getID()] = Value(BT_Right);
			switchButtonStatus(BT_Right, true);
			isChange = true;
		}

		if (isChange)
		{
			if (m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Left)
			{
				m_pHero->walk(-m_pHero->getWalkVelocity());
				SocketManager::getInstance()->sendData(NDT_HeroWalk, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
			}
			else if (m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Right)
			{
				m_pHero->walk(m_pHero->getWalkVelocity());
				SocketManager::getInstance()->sendData(NDT_HeroWalk, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getPhysicsBody()->getVelocity());
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

		if (m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Jump)
		{
			if (!(p.x > winSize.width * 7 / 8 && p.x <= winSize.width && p.y >= 0.f && p.y <= winSize.height * 3 / 4))
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

		if (m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Joystick)
        {
			m_mapPressType.erase(pTouch->getID());
            showJoystick(m_pJoystickBg->getPosition());
			m_pHero->attack(false);
            m_pHero->setIsLocked(false);
            m_pForesight->setVisible(false);
            m_pRange->setVisible(false);
			switchButtonStatus(BT_Joystick, false);
			SocketManager::getInstance()->sendData(NDT_HeroStopAttack, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getShootDirection());
        }
		else if (p.x <= winSize.width / 8 && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
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
	m_pJoystickBg1->setPosition(pos);

	m_pJoystick->setVisible(true);
	m_pJoystickBg->setVisible(true);
	m_pJoystickBg1->setVisible(true);
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
	m_pHero->attack(true);
	SocketManager::getInstance()->sendData(NDT_HeroAttack, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), m_pHero->getShootDirection());
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

void OperateLayer::dealWithKeyBoard()
{
	if (m_KeyPressedValue & KB_Up)
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
	if (type == BT_Joystick)
	{
		if (isPressed)
		{
			m_pJoystick->setSpriteFrame("joystick_down.png");
		}
		else
		{
			m_pJoystick->setSpriteFrame("joystick.png");
		}
	}
}