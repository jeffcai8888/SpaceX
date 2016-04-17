#ifndef __SPACEX_CLASSES_GAMEDATA_H__
#define __SPACEX_CLASSES_GAMEDATA_H__

#include <cocos2d.h>

class GameData
{
public:
	static GameData* getInstance();

	CC_SYNTHESIZE(int, m_Role, Role);

private:
	GameData();
	virtual ~GameData();

	static GameData* instance;

	
};

#endif // __SPACEX_GAMEDATA_H__