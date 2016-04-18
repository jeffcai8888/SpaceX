#include "GameData.h"
#include "Macro.h"
#include "BaseSprite.h"

GameData* GameData::instance = nullptr;

GameData::GameData()
{
	for (int i = 0; i < 4; ++i)
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
