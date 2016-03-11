#include "PhysicsWorldManager.h"
#include "Macro.h"

PhysicsWorldManager* PhysicsWorldManager::instance = nullptr;

PhysicsWorldManager::PhysicsWorldManager()
	:m_pWorld(nullptr)
{
}

PhysicsWorldManager::~PhysicsWorldManager()
{
}

PhysicsWorldManager* PhysicsWorldManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new PhysicsWorldManager();
	}
	return instance;
}

void PhysicsWorldManager::init()
{
	b2Vec2 gravity;
	gravity.Set(0.0f, -1.0f);
	m_pWorld = new b2World(gravity);

	m_pDebugDraw = new GLESDebugDraw(PTM_RADIO);
	m_pWorld->SetDebugDraw(m_pDebugDraw);
	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_centerOfMassBit;
	//flags += b2Draw::e_aabbBit;
	flags += b2Draw::e_centerOfMassBit;
	flags += b2Draw::e_jointBit;
	//flags += b2Draw::e_shapeBit;
	m_pDebugDraw->SetFlags(flags);
}

void PhysicsWorldManager::update(float dt)
{
	m_pWorld->Step(dt, 8, 1);
}

void PhysicsWorldManager::draw()
{
	m_pWorld->DrawDebugData();
}
