#include <cocos2d.h>
#include <Box2D/Box2D.h>
#include "Hero.h"
#include "Bullet.h"

using namespace cocos2d;

#define PTM_RATIO 32

Hero::Hero()
{}

Hero::~Hero()
{}

bool Hero::init()
{
	bool ret = false;
	do {
		this->initWithSpriteFrameName("hero_idle_00.png");

		Animation *pIdleAnim = this->createAnimation("hero_idle_%02d.png", 6, 12);
		this->setIdleAction(RepeatForever::create(Animate::create(pIdleAnim)));

		Animation *pWalkAnim = this->createAnimation("hero_walk_%02d.png", 7, 14);
		this->setWalkAction(RepeatForever::create(Animate::create(pWalkAnim)));
		
		Animation *pAttackAnim = this->createAnimation("hero_attack_00_%02d.png", 3, 20);
		this->setAttackAction(Sequence::create(Animate::create(pAttackAnim), BaseSprite::createIdleCallbackFunc(), NULL));

		Animation *pHurtAnim = this->createAnimation("hero_hurt_%02d.png", 3, 12);
		this->setHurtAction(Sequence::create(Animate::create(pHurtAnim), BaseSprite::createIdleCallbackFunc(), NULL));

		Animation *pDeadAnim = this->createAnimation("hero_knockout_%02d.png", 5, 12);
		this->setDeadAction(Sequence::create(Animate::create(pDeadAnim), Blink::create(3, 9), BaseSprite::createDeadCallbackFunc(), NULL));

		Size heroShowSize = this->getSpriteFrame()->getRect().size;
		this->m_bodyBox = this->createBoundingBox( Point(-heroShowSize.width / 2, -heroShowSize.height / 2), heroShowSize);
		this->m_hitBox = this->createBoundingBox( Point(heroShowSize.width / 2, -10), Size(20, 20));

		/*PhysicsMaterial materail(1.0f, 0.0f, 0.5f);
		auto body = PhysicsBody::create();
		auto shape = PhysicsShapeBox::create(Size(this->getContentSize().width / 8, this->getContentSize().height / 3), PHYSICSSHAPE_MATERIAL_DEFAULT, Vec2(0.f, -15.f));
		body->addShape(shape);
		body->setCategoryBitmask(0x01);
		body->setCollisionBitmask(0x04);
		this->setPhysicsBody(body);*/
		ret = true;
	} while(0);

	return ret;
}


bool Hero::initPhysics(b2World* world)
{
	bool ret = false;
	do {
		m_pWorld = world;
		b2BodyDef heroBodyDef;
		heroBodyDef.type = b2_kinematicBody;
		heroBodyDef.position.Set(this->getPosition().x / PTM_RATIO, this->getPosition().y / PTM_RATIO);
		b2Body* heroBody = m_pWorld->CreateBody(&heroBodyDef);

		b2PolygonShape heroShape;
		heroShape.SetAsBox(this->getContentSize().width / 8, this->getContentSize().height / 3);
		b2FixtureDef bodyFixture;
		bodyFixture.shape = &heroShape;
		bodyFixture.friction = 0.5f;
		bodyFixture.density = 1;
		bodyFixture.restitution = 0.5f;

		heroBody->CreateFixture(&bodyFixture);
		heroBody->SetUserData(this);
		ret = true;
	} while (0);

	return ret;
}
