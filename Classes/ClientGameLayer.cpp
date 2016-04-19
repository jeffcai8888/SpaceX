#include <math.h>
#include "Macro.h"
#include "ClientGameLayer.h"
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
#include "GameData.h"

USING_NS_CC;

ClientGameLayer::ClientGameLayer()
	:GameLayer()
{
	m_dequeShadow.clear();
}

ClientGameLayer::~ClientGameLayer()
{
	m_vecBullets.clear();
	m_dequeShadow.clear();
}

void ClientGameLayer::onEnter()
{
	GameLayer::onEnter();
	SocketManager::getInstance()->getSocketClient()->onRecv = CC_CALLBACK_2(ClientGameLayer::onRecv, this);
	SocketManager::getInstance()->getSocketClient()->onDisconnect = CC_CALLBACK_0(ClientGameLayer::onDisconnect, this);
}

void ClientGameLayer::onRecv(const char* data, int count)
{
	NetworkData* networkData = (NetworkData*)data;
	if (networkData->dataSize == sizeof(NetworkData))
	{
		switch (networkData->dataType)
		{
		case NDT_HeroWalk:
		{
			BaseSprite* player = GameData::getInstance()->m_pPlayers[networkData->index];
			player->setPosition(networkData->position);
			player->walk(networkData->vec.x);
		}
			break;
		case NDT_HeroJumpUp:
		{
			BaseSprite* player = GameData::getInstance()->m_pPlayers[networkData->index];
			player->setPosition(networkData->position);
			player->jump(player->getJumpVelocity());
		}			
			break;
		case NDT_HeroJumpDown:
		{
			BaseSprite* player = GameData::getInstance()->m_pPlayers[networkData->index];
			player->runJumpDownAction();
			player->setPosition(networkData->position);
			player->getPhysicsBody()->setVelocity(networkData->vec);
		}		
			break;
		case NDT_HeroStop:
		{
			BaseSprite* player = GameData::getInstance()->m_pPlayers[networkData->index];
			player->setPosition(networkData->position);
			player->stop();
		}	
			break;
		case NDT_HeroPos:
			m_dequeShadow.push_back(networkData->vec);
			break;
		case NDT_HeroAttack:
		{
			BaseSprite* player = GameData::getInstance()->m_pPlayers[networkData->index];
			player->setPosition(networkData->position);
			player->setShootDirection(networkData->vec);
			player->attack(true);
		}			
			break;
		case NDT_HeroStopAttack:
		{
			BaseSprite* player = GameData::getInstance()->m_pPlayers[networkData->index];
			player->attack(false);
			player->setPosition(networkData->position);
		}		
			break;
		default:
			break;
		}
	}
}

void ClientGameLayer::onDisconnect()
{
	CCLOG("Client disconnect");
}
