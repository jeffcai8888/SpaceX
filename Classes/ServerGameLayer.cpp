#include <math.h>
#include "Macro.h"
#include "ServerGameLayer.h"
#include "Hero.h"
#include "Gunner.h"
#include "Princess.h"
#include "Bullet.h"
#include "Foresight.h"
#include "OperateLayer.h"
#include "JsonParser.h"

#include "SceneManager.h"
#include "SimpleAudioEngine.h"
#include "SocketManager.h"

USING_NS_CC;

ServerGameLayer::ServerGameLayer()
	:GameLayer()
{

}

ServerGameLayer::~ServerGameLayer()
{
	m_vecBullets.clear();
}

void ServerGameLayer::onEnter()
{
	GameLayer::onEnter();
	SocketManager::getInstance()->getSocketServer()->onRecv = CC_CALLBACK_3(ServerGameLayer::onRecv, this);
	SocketManager::getInstance()->getSocketServer()->onDisconnect = CC_CALLBACK_1(ServerGameLayer::onDisconnect, this);
}

void ServerGameLayer::onRecv(HSocket socket, const char* data, int count)
{
	NetworkData* networkData = (NetworkData*)data;
	if (networkData->dataSize == sizeof(NetworkData))
	{
		switch (networkData->dataType)
		{
		case NDT_HeroWalk:
			//m_pHero->runWalkAction(!m_pHero->isInAir());
			m_pEnemy[0]->setPosition(networkData->position);
			//m_pHero->getPhysicsBody()->setVelocity(networkData->velocity);
			m_pEnemy[0]->walk(networkData->vec.x);
			break;
		case NDT_HeroJumpUp:
			//m_pHero->runJumpAction(true);
			m_pEnemy[0]->setPosition(networkData->position);
			//m_pHero->getPhysicsBody()->setVelocity(networkData->velocity);
			m_pEnemy[0]->jump(m_pEnemy[0]->getJumpVelocity());
			break;
		case NDT_HeroJumpDown:
			m_pEnemy[0]->runJumpAction(false);
			m_pEnemy[0]->setPosition(networkData->position);
			m_pEnemy[0]->getPhysicsBody()->setVelocity(networkData->vec);
			break;
		case NDT_HeroStop:
			//m_pHero->runIdleAction();
			m_pEnemy[0]->setPosition(networkData->position);
			//m_pHero->getPhysicsBody()->setVelocity(networkData->velocity);
			m_pEnemy[0]->stop();
			m_pEnemy[0]->stopMoveAction(MOVE_STATE_WALK, true);
			break;
		case NDT_HeroPos:
			m_dequeShadow.push_back(networkData->vec);
			break;
		case NDT_HeroAttack:
			m_pEnemy[0]->setPosition(networkData->position);
			m_pEnemy[0]->setShootDirection(networkData->vec);
			m_pEnemy[0]->attack(true);
			break;
		case NDT_HeroStopAttack:
			m_pEnemy[0]->setPosition(networkData->position);
			m_pEnemy[0]->attack(false);
			break;
		default:
			break;
		}
	}
}

void ServerGameLayer::onDisconnect(HSocket socket)
{
	CCLOG("Client disconnect");
}
