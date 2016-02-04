#include "OperateLayer.h"
#include "hero.h"

using namespace cocos2d;
using namespace std;

OperateLayer::OperateLayer()
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
		this->addChild(m_pJoystickBg[JT_Player], 0);
		this->addChild(m_pJoystick[JT_Player], 1);

		m_pJoystick[JT_Bullet] = Sprite::create("joystick.png");
		m_pJoystick[JT_Bullet]->setScale(0.75f, 0.75f);
		m_pJoystickBg[JT_Bullet] = Sprite::create("joystick_bg.png");
		m_pJoystickBg[JT_Bullet]->setScale(0.75f, 0.75f);
		this->addChild(m_pJoystick[JT_Bullet], 0);
		this->addChild(m_pJoystickBg[JT_Bullet], 1);
		m_pJoystick[JT_Bullet]->setPosition(Point(400.f, 50.f));
		m_pJoystickBg[JT_Bullet]->setPosition(Point(400.f, 50.f));


		this->hideJoystick(JT_Player);

		//setTouchEnabled(true);
		//setTouchMode(Touch::DispatchMode::ALL_AT_ONCE);

		auto listener = EventListenerTouchAllAtOnce::create();
		//listener->onTouchesBegan = CC_CALLBACK_2(OperateLayer::onTouchesBegan, this);
		//listener->onTouchesMoved = CC_CALLBACK_2(OperateLayer::onTouchesMoved, this);
		//listener->onTouchesEnded = CC_CALLBACK_2(OperateLayer::onTouchesEnded, this);
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
					CCLOG("onTouchesMoved direction(%f, %f)", direction.x, direction.y);
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

				CCLOG("onTouchesMoved %f", cos);

				if (direction.y > 0 && cos >= 0 && cos < 0.9)
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
				this->hideJoystick(JT_Player);
			else
			{
				Point pos = m_pJoystickBg[JT_Bullet]->getPosition();
				m_pJoystick[JT_Bullet]->setPosition(pos);
				m_pHero->setIsAttacking(false);
			}
			m_pHero->stop();
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
			DealWithKeyBoard();
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
			DealWithKeyBoard();
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

		m_KeyPressedValue = 0;
		
		ret = true;

	} while(false);

	return ret;
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

void OperateLayer::DealWithKeyBoard()
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