#include "Macro.h"
#include "Bullet.h"
#include "BaseSprite.h"
#include "ConfigCenter.h"
#include "BulletConfigModel.h"

USING_NS_CC;

Bullet::Bullet()
	:m_launchTime(0.f)
{
}


Bullet::~Bullet()
{
}

bool Bullet::init()
{
	bool ret = false;
	do 
	{
		this->initWithSpriteFrameName("bullet.png");
		auto body = PhysicsBody::create();
		auto shape = PhysicsShapeBox::create(Size(this->getContentSize().width, this->getContentSize().height), PHYSICSSHAPE_MATERIAL_DEFAULT);
		body->addShape(shape);
		body->setCategoryBitmask(PC_Bullet);
        body->setContactTestBitmask(PC_Ground | PC_Box | PC_Slope | PC_Damage);
		body->setCollisionBitmask(PC_Ground | PC_Box | PC_Slope);
		body->setRotationEnable(false);
		this->setPhysicsBody(body);
		ret = true;
	} while (false);
	return ret;
}

void Bullet::update(float dt)
{
	if (this->m_isActive)
	{
		float x = this->getPhysicsBody()->getVelocity().x;
		float y = this->getPhysicsBody()->getVelocity().y;
		y += m_gravity * dt;
		this->getPhysicsBody()->setVelocity(Vec2(x, y));

		m_launchTime += dt;
		if (m_launchTime >= m_fDisappearTime)
		{
			m_launchTime = 0.f;
			this->m_isActive = false;
			EventCustom event("bullet_disappear");
			event.setUserData(this);
			_eventDispatcher->dispatchEvent(&event);
		}
	}
}

void Bullet::launch(BaseSprite* pHero)
{
	BulletConfigMap bulletConfigMap = ConfigCenter::getInstance()->getBulletConfigModel()->GetBulletConfigMap();
	BulletConfig config = bulletConfigMap[pHero->getBulletType()];
	this->m_isActive = true;
	Point pos = pHero->getShootPosition();
	this->setPosition(pos);	
	this->m_fVelocity = config.m_fLaunchVelocity;
	this->m_fDisappearTime = config.m_fDisappearTime;
	float rotation = CC_DEGREES_TO_RADIANS(-rand_0_1() *config.m_iAngle);
	if (pHero->getIsShootInit())
	{
		this->m_fDirection = pHero->getShootDirection().rotateByAngle(Vec2(0.f, 0.f), rotation);
	}
	else
	{
		if (pHero->isFlippedX())
		{
			this->m_fDirection = Vec2(-1.f,0.f).rotateByAngle(Vec2(0.f, 0.f), rotation);
		}
		else
		{
			this->m_fDirection = Vec2(1.f, 0.f).rotateByAngle(Vec2(0.f, 0.f), rotation);
		}
	}
	
	this->m_power = config.m_iPower;
	this->m_gravity = config.m_fGravity;
	this->m_ownerTag = pHero->getTag();
	this->getPhysicsBody()->setVelocity(m_fVelocity * m_fDirection);
	m_launchTime = 0.f;
}
