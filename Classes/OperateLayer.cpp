#include "OperateLayer.h"
#include "Hero.h"
#include "GameLayer.h"

using namespace cocos2d;
using namespace std;

OperateLayer::OperateLayer()
	:m_pCloseItem(nullptr),
	m_pBlood(nullptr),
	m_pBloodBg(nullptr)
{
	for (int i = 0; i < 2; ++i)
	{
		m_pJoystick[i] = NULL;
		m_pJoystickBg[i] = NULL;
	}
}

OperateLayer::~OperateLayer()
{

}

bool OperateLayer::init()
{
	bool ret = false;
	do {
		CC_BREAK_IF( !Layer::init() );

		m_pJoystick[JT_Player] = Sprite::create("joystick.png");
		m_pJoystickBg[JT_Player] = Sprite::create("joystick_bg.png");
        m_pJoystick[JT_Player]->setScale(2.5f, 2.5f);
        m_pJoystickBg[JT_Player]->setScale(2.5f, 2.5f);
		this->addChild(m_pJoystickBg[JT_Player], 0);
		this->addChild(m_pJoystick[JT_Player], 1);
        resetJoystick(JT_Player);
        

		m_pJoystick[JT_Bullet] = Sprite::create("joystick.png");
		m_pJoystickBg[JT_Bullet] = Sprite::create("joystick_bg.png");
        m_pJoystick[JT_Bullet]->setScale(2.5f, 2.5f);
        m_pJoystickBg[JT_Bullet]->setScale(2.5f, 2.5f);
		this->addChild(m_pJoystick[JT_Bullet], 0);
		this->addChild(m_pJoystickBg[JT_Bullet], 1);
        resetJoystick(JT_Bullet);
		
		m_pTarget = Sprite::create("target.jpg");
		this->addChild(m_pTarget, 2);

		//this->hideJoystick(JT_Player);
		this->hideTarget();

		auto visibleSize = Director::getInstance()->getVisibleSize();
		auto m_origin = Director::getInstance()->getVisibleOrigin();

		m_pCloseItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(OperateLayer::exitApp, this));
		m_pCloseItem->setPosition(m_origin + Point(visibleSize) - Point(m_pCloseItem->getContentSize() / 2));
		auto menu = Menu::create(m_pCloseItem, NULL);
		menu->setPosition(Point::ZERO);
		this->addChild(menu, 1);

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
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = [this](const vector<Touch*>& touches, Event *event)
	{
		Size winSize = Director::getInstance()->getWinSize();
		vector<Touch*>::const_iterator touchIter = touches.begin();
		while (touchIter != touches.end())
		{
			Touch *pTouch = (Touch*)(*touchIter);
			Point p = pTouch->getLocation();
			if (p.x <= winSize.width / 2)
			{
				this->showJoystick(JT_Player, p);
			}
			else {
				if (this->isTap(m_pJoystickBg[JT_Bullet], pTouch->getLocation()))
					m_pHero->attack();
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
		if (start.x > winSize.width / 2)
		{
			if (this->isTap(m_pJoystickBg[JT_Bullet], start))
			{
				Point dest = pTouch->getLocation();
				float distance = start.getDistance(dest);
				Vec2 direction = dest - start;
				direction.normalize();
				this->updateJoystick(JT_Bullet, direction, distance);
				m_pHero->setShootDirection(direction);
				this->updateTarget(direction);
			}
		}
		else
		{
			Point dest = pTouch->getLocation();
			float distance = start.getDistance(dest);
			Vec2 direction = dest - start;
			direction.normalize();
			this->updateJoystick(JT_Player, direction, distance);

			const Vec2 v1(direction.x, direction.y);
			const Vec2 v2(1.f, 0.f);
			float cos = v1.dot(v2);

			//CCLOG("onTouchesMoved %f", cos);

			if (direction.y > 0 && cos > -0.9 && cos < 0.9)
				m_pHero->jump(direction, distance);
			else
				m_pHero->walk(direction, distance);
		}
	};
	listener->onTouchesEnded = [this](const vector<Touch*>& touches, Event *event)
	{
		Size winSize = Director::getInstance()->getWinSize();
		std::vector<Touch*>::const_iterator touchIter = touches.begin();
		Touch *pTouch = (Touch*)(*touchIter);
		Point start = pTouch->getStartLocation();
		if (start.x < winSize.width / 2)
		{
			this->resetJoystick(JT_Player);
			if (m_pHero->getCurrActionState() == ACTION_STATE_WALK)
				m_pHero->stop();
		}
		else
		{
			Point pos = m_pJoystickBg[JT_Bullet]->getPosition();
			m_pJoystick[JT_Bullet]->setPosition(pos);
			m_pHero->setIsAttacking(false);
			this->hideTarget();
		}

	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_pJoystick[JT_Bullet]);
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event)
	{
		CCLOG("KeyPressed %d", keyCode);
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
		CCLOG("KeyReleased %d", keyCode);
		if (keyCode == EventKeyboard::KeyCode::KEY_D)
		{
			m_KeyPressedValue ^= KB_Front;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_A)
		{
			m_KeyPressedValue ^= KB_Back;
		}
		else if (keyCode == EventKeyboard::KeyCode::KEY_W)
		{
			m_KeyPressedValue ^= KB_Up;
		}
		dealWithKeyBoard();
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
}

void OperateLayer::onExit()
{
	Layer::onExit();
	_eventDispatcher->removeAllEventListeners();
}

void OperateLayer::showJoystick(int type, Point pos)
{
	m_pJoystick[type]->setPosition(pos);
	m_pJoystickBg[type]->setPosition(pos);

	m_pJoystick[type]->setVisible(true);
	m_pJoystickBg[type]->setVisible(true);
}

void OperateLayer::hideJoystick(int type)
{
	m_pJoystick[type]->setPosition(m_pJoystickBg[type]->getPosition());
	m_pJoystick[type]->setVisible(false);
	m_pJoystickBg[type]->setVisible(false);
}

void OperateLayer::updateJoystick(int type, Point direction, float distance)
{
	Point start = m_pJoystickBg[type]->getPosition();

	if(distance < 33)
	{
		m_pJoystick[type]->setPosition(start + (direction * distance));
	//}else if(distance > 78) {
	//	m_pJoystick[type]->setPosition(start + (direction * 45));
	}else {
		m_pJoystick[type]->setPosition(start + (direction * 33));
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
	if (m_KeyPressedValue&KB_Up)
	{
		m_pHero->jump(Vec2(0.f, 1.f), 5.f);
	}
	else if (m_KeyPressedValue&KB_Front)
	{
		m_pHero->walk(Vec2(1.f, 0.f), 5.f);
	}
	else if (m_KeyPressedValue&KB_Back)
	{
		m_pHero->walk(Vec2(-1.f, 0.f), 5.f);
	}
	else
	{
		m_pHero->stop();
	}

}

void OperateLayer::updateTarget(Point pos)
{
	m_pTarget->setPosition(m_pHero->getPosition() + m_pGameLayer->getPosition() + pos * 200);
	CCLOG("Hero pos = %f, %f", m_pHero->getPosition().x, m_pHero->getPosition().y);
	CCLOG("GameLayer pos = %f, %f", m_pGameLayer->getPosition().x, m_pGameLayer->getPosition().y);
	m_pTarget->setVisible(true);
}

void OperateLayer::hideTarget()
{
	m_pTarget->setVisible(false);
}

void OperateLayer::exitApp(Ref* pSender)
{
	Director::getInstance()->end();
}