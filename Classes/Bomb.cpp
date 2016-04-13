#include "Macro.h"
#include "Bomb.h"
#include "Hero.h"
#include "ConfigCenter.h"
#include "BombConfigModel.h"

USING_NS_CC;

Bomb::Bomb()
:m_gravity(0.f)
,m_isActive(false)
,m_isStart(false)
{
}


Bomb::~Bomb()
{
}

bool Bomb::init()
{
	bool ret = false;
	do 
	{
		this->initWithSpriteFrameName("bullet.png");
		auto body = PhysicsBody::create();
		auto shape = PhysicsShapeBox::create(Size(this->getContentSize().width, this->getContentSize().height), PHYSICSSHAPE_MATERIAL_DEFAULT);
		body->addShape(shape);
		body->setCategoryBitmask(PC_Bomb);
        body->setContactTestBitmask(PC_Ground | PC_Box | PC_Slope);
		body->setCollisionBitmask(PC_Ground | PC_Box | PC_Slope);
		body->setRotationEnable(false);
		this->setPhysicsBody(body);
		ret = true;
	} while (false);
	return ret;
}

void Bomb::reset()
{
	m_isActive = false;
	m_isStart = false;
}

void Bomb::update(float dt)
{
	if (m_isActive)
	{
		float x = this->getPhysicsBody()->getVelocity().x;
		float y = this->getPhysicsBody()->getVelocity().y;
		y += m_gravity * dt;
		this->getPhysicsBody()->setVelocity(Vec2(x, y));
		if (m_isStart)
		{
			m_startTime -= dt;
			if (m_startTime < 0)
			{
				m_isStart = false;
				explode();
			}
		}
	}
}

void Bomb::launch(Hero* pHero)
{
	BombConfigMap bombConfigMap = ConfigCenter::getInstance()->getBombConfigModel()->GetBombConfigMap();
	BombConfig config = bombConfigMap[pHero->getBombType()];
	this->m_isActive = true;
	this->m_isStart = false;
	Point pos = pHero->getShootPosition();
	this->setPosition(pos);	
	this->m_fVelocity = config.m_fVelocity;
	if (pHero->isFlippedX())
		this->m_fDirection = Vec2(-1.f, 0.f);
	else
		this->m_fDirection = Vec2(1.f, 0.f);
	this->m_power = config.m_iPower;
	this->m_gravity = config.m_fGravity;
	this->m_owner = pHero;
	this->getPhysicsBody()->setVelocity(m_fVelocity * m_fDirection);
}

void Bomb::start()
{
	BombConfigMap bombConfigMap = ConfigCenter::getInstance()->getBombConfigModel()->GetBombConfigMap();
	BombConfig config = bombConfigMap[this->m_owner->getBombType()];
	m_startTime = config.m_fStartTime;
	Blink* blinkAction = Blink::create(m_startTime, 10);
	this->runAction(blinkAction);
	m_isStart = true;
	getPhysicsBody()->setVelocity(Vec2(0.f, 0.f));
}

void Bomb::explode()
{
	m_owner->setIsBombExplore(true);
	reset();
}
