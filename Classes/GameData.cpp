#include "GameData.h"
#include "Macro.h"
#include "BaseSprite.h"

GameData* GameData::instance = nullptr;

GameData::GameData()
	:m_PlayerNum(4)
{
	for (int i = 0; i < PLAYER_MAX_NUM; ++i)
	{
		m_pPlayers[i] = nullptr;
		m_playerTypes[i] = ROLE_NONE;
	}
}


GameData::~GameData()
{
}

GameData* GameData::getInstance()
{
	if (instance == nullptr)
	{
		instance = new GameData();
	}
	return instance;
}

BaseSprite* GameData::getMySelf()
{
	return m_pPlayers[m_RoleIndex];
}
