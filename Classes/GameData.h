#ifndef __SPACEX_CLASSES_GAMEDATA_H__
#define __SPACEX_CLASSES_GAMEDATA_H__

class GameData
{
public:
	static GameData* getInstance();

private:
	GameData();
	virtual ~GameData();

	static GameData* instance;
};

#endif // __SPACEX_GAMEDATA_H__