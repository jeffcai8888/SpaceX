#include "Macro.h"
#include "Bullet.h"
#include "Hero.h"
#include "PhysicsWorldManager.h"

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
		CC_BREAK_IF(!PhysicsSprite::init());

		this->initWithSpriteFrameName("bullet.png");
		/*auto body = PhysicsBody::create();
		auto shape = PhysicsShapeBox::create(Size(this->getContentSize().width, this->getContentSize().height), PHYSICSSHAPE_MATERIAL_DEFAULT);
		body->addShape(shape);
		body->setCategoryBitmask(0x02);
        body->setContactTestBitmask(0x4);
		body->setCollisionBitmask(0x04);
		this->setPhysicsBody(body);*/
		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		auto body = PhysicsWorldManager::getInstance()->getWorld()->CreateBody(&bd);
		this->setB2Body(body);
		this->setPTMRatio(PTM_RADIO);

		b2PolygonShape polygon;
		polygon.SetAsBox(this->getContentSize().width / 2 / PTM_RADIO, this->getContentSize().height / 2 / PTM_RADIO);

		b2FixtureDef fd;
		fd.shape = &polygon;
		fd.density = 1.0f;
		fd.friction = 0.f;
		fd.restitution = 0.f;
		body->CreateFixture(&fd);

		ret = true;
	} while (false);
	return ret;
}

void Bullet::update(float dt)
{
	if (this->m_isActive)
	{
		Point curPos = this->getPosition();

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

void Bullet::launch(Hero* pHero)
{
	this->m_isActive = true;
	this->m_owner = pHero;
	Point pos = pHero->getPosition();
	if(pHero->isFlippedX())
		this->setPosition(pos + Vec2(-15.f, -20.f));
	else
		this->setPosition(pos + Vec2(15.f, -20.f));
	this->m_startPostion = pos;
	this->m_fVelocity = pHero->getBulletLaunchVelocity();
	this->m_fDisappearTime = pHero->getBulletDisappearTime();
	this->m_fDirection = pHero->getShootDirection().rotateByAngle(Vec2(0.f, 0.f), CC_DEGREES_TO_RADIANS((int)(-rand_0_1() * pHero->getBullletAngle())));
	this->m_power = pHero->getBullletPower();
	//this->getPhysicsBody()->setVelocity(Vec2(0.f, 0.f));
	//this->getPhysicsBody()->applyImpulse(m_fVelocity * m_fDirection + pHero->getPhysicsBody()->getVelocity());
	this->getB2Body()->SetLinearVelocity(b2Vec2(0.f,0.f));
	b2Vec2 p = this->getB2Body()->GetWorldCenter();
	b2Vec2 heroV = pHero->getB2Body()->GetLinearVelocity();
	b2Vec2 bulletV = b2Vec2((m_fVelocity * m_fDirection).x, (m_fVelocity * m_fDirection).y);
	this->getB2Body()->ApplyLinearImpulse(heroV + bulletV, p, true);

	m_launchTime = 0.f;
}
