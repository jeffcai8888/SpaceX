#ifndef __SPACEX_CLASSES_GAMEDATA_H__
#define __SPACEX_CLASSES_GAMEDATA_H__

#include <cocos2d.h>

class BaseSprite;
class GameData
{
public:
	static GameData* getInstance();

	int			m_playerTypes[4];
	BaseSprite* m_pPlayers[4];

	BaseSprite* getMySelf();

	CC_SYNTHESIZE(int, m_RoleType, RoleType);
	CC_SYNTHESIZE(int, m_RoleIndex, RoleIndex);

private:
	GameData();
	virtual ~GameData();

	static GameData* instance;
};

#endif // __SPACEX_GAMEDATA_H__