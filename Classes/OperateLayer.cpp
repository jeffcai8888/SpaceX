#include "Macro.h"
#include "OperateLayer.h"
#include "GameLayer.h"
#include "Hero.h"
#include "SceneManager.h"
#include "Foresight.h"
#include "JsonParser.h"
#include "SocketManager.h"
#include "GameData.h"

USING_NS_CC;
using namespace std;

OperateLayer::OperateLayer()
	:m_pJoystick(nullptr),
    m_pJoystickBg(nullptr),
	m_pFront(nullptr),
	m_pBack(nullptr),
    m_pUp(nullptr),
	m_pSkill(nullptr),
	m_pSkill1(nullptr),
	m_isAutoShootPressed(false)
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
		m_pUp->setPosition(m_origin + Point(1038, 264));
        m_pUp->setScale(1.25f);
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
		showJoystick(Point(1039, 103));

		m_pSkill = Sprite::createWithSpriteFrameName("skill_flash1.png");
		m_pSkill->setScale(0.5f);
		this->addChild(m_pSkill);
		m_pSkill->setPosition(m_origin + Point(918, 198));

		m_pSkill1 = Sprite::createWithSpriteFrameName("skill_bomb.png");
		m_pSkill1->setScale(0.5f);
		this->addChild(m_pSkill1);
		m_pSkill1->setPosition(m_origin + Point(880, 78));

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
	GameLayer *layer = static_cast<GameLayer *>(this->getScene()->getChildByTag(LT_Game));
    m_pForesight = layer->getForesight();
    m_pRange = layer->getRange();

	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = [this](const vector<Touch*>& touches, Event *event)
	{
		Size winSize = Director::getInstance()->getWinSize();
		vector<Touch*>::const_iterator touchIter = touches.begin();
		while (touchIter != touches.end())
		{
			Touch *pTouch = (Touch*)(*touchIter);
			Point p = pTouch->getLocation();
			if(isTap(m_pJoystickBg, p))
			{
				m_mapPressType[pTouch->getID()] = Value(BT_Joystick);
#if 0
				m_pHero->attack(true);
                m_pHero->setIsLocked(true);
                m_pForesight->setVisible(true);
                m_pRange->setVisible(true);
#else
				m_isAutoShootPressed = true;
#endif
				switchButtonStatus(BT_Joystick, true);
				BaseSprite* self = GameData::getInstance()->getMySelf();
				SocketManager::getInstance()->sendData(NDT_HeroAttack, self->getCurrActionState(), self->getPosition(), self->getShootDirection());
			}
			else if (p.x <= 200.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
			{
				switchButtonStatus(BT_Left, true);
				m_mapPressType[pTouch->getID()] = Value(BT_Left);
				m_KeyPressedValue |= KB_Back;
				dealWithKeyBoard();
			}
			else if (p.x > 200.f && p.x <= 400.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
			{
				switchButtonStatus(BT_Right, true);
				m_mapPressType[pTouch->getID()] = Value(BT_Right);
				m_KeyPressedValue |= KB_Front;
				dealWithKeyBoard();
			}
			else if (isTap(m_pUp, p))
            {
				BaseSprite* self = GameData::getInstance()->getMySelf();
				self->jump(self->getJumpVelocity());
				SocketManager::getInstance()->sendData(NDT_HeroJumpUp, self->getCurrActionState(), self->getPosition(), self->getPhysicsBody()->getVelocity());
				
				m_mapPressType[pTouch->getID()] = Value(BT_Jump);
				switchButtonStatus(BT_Jump, true);
			}
			else if (isTap(m_pSkill, p))
			{
				BaseSprite* self = GameData::getInstance()->getMySelf();
				Hero* hero = dynamic_cast<Hero*>(self);
				if (hero)
				{
					GameLayer *layer = static_cast<GameLayer *>(this->getScene()->getChildByTag(LT_Game));
					auto positionSprite = layer->getSkillStartPos();
					if (hero->getCurSkillState() == 0)
					{
						hero->setSkillActivePos(hero->getPosition());
						if (hero->isFlippedX())
						{
							hero->getPhysicsBody()->setVelocity(Vec2(-hero->getSkillState1Speed(), 0.f));
						}
						else
						{
							hero->getPhysicsBody()->setVelocity(Vec2(hero->getSkillState1Speed(), 0.f));
						}
						m_pSkill->setSpriteFrame("skill_flash2.png");
						hero->setCurSkillState(1);
						hero->setCurSkillCDTime(hero->getSkillState1CDTime());
						hero->setCurSkillLastTime(hero->getSkillState1LastTime());

						CCLOG("skill speed = %f, skill last time = %f", hero->getPhysicsBody()->getVelocity().x, hero->getCurSkillLastTime());
						hero->setIsInSplash(true);
						positionSprite->setVisible(true);
						positionSprite->setPosition(hero->getPosition() + Vec2(0, -10.f));
					}
					else if (hero->getCurSkillState() == 1)
					{
						if (hero->isFlippedX())
						{
							hero->getPhysicsBody()->setVelocity(Vec2(-hero->getSkillState1Speed(), 0.f));
						}
						else
						{
							hero->getPhysicsBody()->setVelocity(Vec2(hero->getSkillState1Speed(), 0.f));
						}
						m_pSkill->setSpriteFrame("skill_flash3.png");
						hero->setCurSkillState(2);
						hero->setIsInSplash(true);
						hero->setCurSkillCDTime(hero->getSkillState2CDTime());
						hero->setCurSkillLastTime(hero->getSkillState2LastTime());
					}
					else if (hero->getCurSkillState() == 2)
					{
						hero->setPosition(hero->getSkillActivePos());
						hero->setCurSkillState(0);
						m_pSkill->setSpriteFrame("skill_flash1.png");
						positionSprite->setVisible(false);
					}					
				}
			}
			else if (isTap(m_pSkill1, p))
			{
				BaseSprite* self = GameData::getInstance()->getMySelf();
				Hero* hero = dynamic_cast<Hero*>(self);
				if (hero)
				{
					hero->setIsThrowBomb(true);
				}
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
		if (diff < 400.f)
			return;
		BaseSprite* self = GameData::getInstance()->getMySelf();
		
        if(m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Joystick && !self->getIsAutoShoot())
        {
			if (isInRange(m_pJoystickBg->getPosition(), m_pJoystickBg->getContentSize().width * 3, m_pJoystickBg->getContentSize().height * 3, p))
				dealWithJoystick(m_pJoystickBg->getPosition(), p);
			else
			{
                showJoystick(m_pJoystickBg->getPosition());
				m_mapPressType.erase(pTouch->getID());
                m_pForesight->setVisible(false);
                m_pRange->setVisible(false);				
				self->attack(false);
				switchButtonStatus(BT_Joystick, false);
                SocketManager::getInstance()->sendData(NDT_HeroStopAttack, self->getCurrActionState(), self->getPosition(), self->getShootDirection());
			}
#if 0
            m_pHero->setIsLocked(false);
#else
			m_isAutoShootPressed = false;
#endif
        }

		
		if (m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Left)
		{
			if (!(p.x <= 200.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4))
			{
				switchButtonStatus(BT_Left, false);
				m_mapPressType.erase(pTouch->getID());
				m_KeyPressedValue &= ~KB_Back;
				dealWithKeyBoard();
			}
		}
		else if (p.x <= 200.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
		{
			if (m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Right)
			{
				m_KeyPressedValue &= ~KB_Front;
			}
			m_mapPressType[pTouch->getID()] = Value(BT_Left);
			switchButtonStatus(BT_Left, true);
			m_KeyPressedValue |= KB_Back;
			dealWithKeyBoard();
		}

		if (m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Right)
		{
			if (!(p.x > 200.f && p.x <= 400.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4))
			{
				switchButtonStatus(BT_Right, false);
				m_mapPressType.erase(pTouch->getID());
				m_KeyPressedValue &= ~KB_Front;
				dealWithKeyBoard();
			}
		}
		else if (p.x > 200.f && p.x <= 400.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
		{
			if (m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Left)
			{
				m_KeyPressedValue &= ~KB_Back;
			}
			m_mapPressType[pTouch->getID()] = Value(BT_Right);
			switchButtonStatus(BT_Right, true);
			m_KeyPressedValue |= KB_Front;
			dealWithKeyBoard();
		}

		if (m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Jump)
		{
			if (!(isTap(m_pUp, p)))
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
		BaseSprite* self = GameData::getInstance()->getMySelf();
		if (m_mapPressType.find(pTouch->getID()) != m_mapPressType.end() && m_mapPressType[pTouch->getID()].asInt() == BT_Joystick)
        {
			m_mapPressType.erase(pTouch->getID());
            showJoystick(m_pJoystickBg->getPosition());
			
#if 0
			m_pHero->attack(false);
            m_pHero->setIsLocked(false);
            m_pForesight->setVisible(false);
            m_pRange->setVisible(false);
#else
			if (!m_isAutoShootPressed)
			{
				self->attack(false);
				self->setIsLocked(false);
				m_pForesight->setVisible(false);
				m_pRange->setVisible(false);
			}
			else
			{
				m_isAutoShootPressed = false;
				self->setIsAutoShoot(!self->getIsAutoShoot());
				if (self->getIsAutoShoot())
				{
					m_pRange->setVisible(true);
				}
				else
				{
					m_pRange->setVisible(false);
				}
			}
#endif
			switchButtonStatus(BT_Joystick, false);
			SocketManager::getInstance()->sendData(NDT_HeroStopAttack, self->getCurrActionState(), self->getPosition(), self->getShootDirection());
        }
		else if (p.x <= 200.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
		{
			/*if (!m_pHero->isInAir())
			{
				m_pHero->stop();
				SocketManager::getInstance()->sendData(NDT_HeroStop, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), Vec2(0, 0));
			}
			m_pHero->stopMoveAction(MOVE_STATE_WALK, true);*/
			switchButtonStatus(BT_Left, false);
			m_mapPressType.erase(pTouch->getID());
			m_KeyPressedValue &= ~KB_Back;
			dealWithKeyBoard();

		}
		else if (p.x > 200.f && p.x <= 400.f && p.y >= 0.f && p.y <= winSize.height * 3 / 4)
		{
			/*if (!m_pHero->isInAir())
			{
				m_pHero->stop();
				SocketManager::getInstance()->sendData(NDT_HeroStop, m_pHero->getCurrActionState(), m_pHero->getCurrMoveState(), m_pHero->getPosition(), Vec2(0, 0));
			}
			m_pHero->stopMoveAction(MOVE_STATE_WALK, true);*/
			switchButtonStatus(BT_Right, false);
			m_mapPressType.erase(pTouch->getID());
			m_KeyPressedValue &= ~KB_Front;
			dealWithKeyBoard();
		}
		if (isTap(m_pUp, p))
		{
			m_mapPressType.erase(pTouch->getID());
			switchButtonStatus(BT_Jump, false);
		}
		dealWithKeyBoard();
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
			switchButtonStatus(BT_Jump, true);
			BaseSprite* self = GameData::getInstance()->getMySelf();
			self->jump(self->getJumpVelocity());
			SocketManager::getInstance()->sendData(NDT_HeroJumpUp, self->getCurrActionState(), self->getPosition(), self->getPhysicsBody()->getVelocity());
		}
		dealWithKeyBoard();
	};
	keyListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event *event)
	{
		if (keyCode == EventKeyboard::KeyCode::KEY_D)
		{
			m_KeyPressedValue &= ~KB_Front;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_A)
		{
			m_KeyPressedValue &= ~KB_Back;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_W)
		{
			switchButtonStatus(BT_Jump, false);
		}
		dealWithKeyBoard();
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
	m_vecEventListener.pushBack(keyListener);


	auto listener1 = EventListenerCustom::create("heroSkillTimeOut", [this](EventCustom* event) {
		BaseSprite* self = GameData::getInstance()->getMySelf();
		Hero* hero = dynamic_cast<Hero*>(self);
		if (hero)
		{
			GameLayer *layer = static_cast<GameLayer *>(this->getScene()->getChildByTag(LT_Game));
			auto positionSprite = layer->getSkillStartPos();
			positionSprite->setVisible(false);
			m_pSkill->setSpriteFrame("skill_flash1.png");
		}
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listener1, 1);
	m_vecEventListener.pushBack(listener1);

	auto listener2 = EventListenerCustom::create("auto_shoot", [this](EventCustom* event) {
		BaseSprite* self = GameData::getInstance()->getMySelf();
		self->attack(true);
		m_pForesight->setVisible(true);
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listener2, 1);
	m_vecEventListener.pushBack(listener2);

	auto listener3 = EventListenerCustom::create("auto_shoot_finish", [this](EventCustom* event) {
		BaseSprite* self = GameData::getInstance()->getMySelf();
		self->attack(false);
		m_pForesight->setVisible(false);
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listener3, 1);
	m_vecEventListener.pushBack(listener3);

	auto listener4 = EventListenerCustom::create("collision", [this](EventCustom* event) {
		BaseSprite* player = static_cast<BaseSprite *>(event->getUserData());
		BaseSprite* self = GameData::getInstance()->getMySelf();
		if (player == self)
		{
			dealWithKeyBoard();
		}
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listener4, 1);
	m_vecEventListener.pushBack(listener4);
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
	m_pJoystickBg->setVisible(false);
	m_pJoystickBg1->setVisible(false);
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
	BaseSprite* self = GameData::getInstance()->getMySelf();
	self->setShootDirection(direction);
	self->attack(true);
	SocketManager::getInstance()->sendData(NDT_HeroAttack, self->getCurrActionState(), self->getPosition(), self->getShootDirection());
}

bool OperateLayer::isTap(cocos2d::Node* pNode, cocos2d::Point point)
{
	Point pos = pNode->getPosition();
	Size size = pNode->getContentSize();
	return isInRange(pos, size.width / 2, point);
}

bool OperateLayer::isInRange(cocos2d::Point p1, float w, float h, cocos2d::Point p2)
{
	return (p2.x >= (p1.x - w /2) && p2.x <= (p1.x + w / 2) && p2.y >= (p1.y - h / 2) && p2.y <= (p1.y + h / 2));
}

bool OperateLayer::isInRange(cocos2d::Point p1, float r, cocos2d::Point p2)
{
	float distance = p1.distanceSquared(p2);
	if (distance < r * r)
		return true;
	else
		return false;
}

void OperateLayer::dealWithKeyBoard()
{
	CCLOG("m_KeyPressedValue = %d", m_KeyPressedValue);
	BaseSprite* self = GameData::getInstance()->getMySelf();
	if (m_KeyPressedValue & KB_Back)
	{
		self->walk(-self->getWalkVelocity());
		SocketManager::getInstance()->sendData(NDT_HeroWalk, self->getCurrActionState(), self->getPosition(), self->getPhysicsBody()->getVelocity());
	}
	else if (m_KeyPressedValue & KB_Front)
	{
		self->walk(self->getWalkVelocity());
		SocketManager::getInstance()->sendData(NDT_HeroWalk, self->getCurrActionState(), self->getPosition(), self->getPhysicsBody()->getVelocity());
	}
	
	else
	{

		if (!self->isInAir())
		{
			self->stop();
			SocketManager::getInstance()->sendData(NDT_HeroStop, self->getCurrActionState(), self->getPosition(), Vec2(0, 0));
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