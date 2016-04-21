#ifndef __SPACEX_CLASSES_GAMEDATA_H__
#define __SPACEX_CLASSES_GAMEDATA_H__

#include <cocos2d.h>

class BaseSprite;

#define PLAYER_MAX_NUM 4
class GameData
{
public:
	static GameData* getInstance();

	int			m_playerTypes[PLAYER_MAX_NUM];
	BaseSprite* m_pPlayers[PLAYER_MAX_NUM];

	BaseSprite* getMySelf();

	CC_SYNTHESIZE(int, m_RoleType, RoleType);
	CC_SYNTHESIZE(int, m_RoleIndex, RoleIndex);
	CC_SYNTHESIZE(int, m_PlayerNum, PlayerNum);

private:
	GameData();
	virtual ~GameData();

	static GameData* instance;
};

#endif // __SPACEX_GAMEDATA_H__