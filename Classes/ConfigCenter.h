#ifndef _SPACEX_CLASSES_CONFIGCENTER_H_
#define _SPACEX_CLASSES_CONFIGCENTER_H_

#include <cocos2d.h>

class BulletConfigModel;
class BombConfigModel;
class RoleConfigModel;

class ConfigCenter
{
public:
	static ConfigCenter* getInstance();

	BulletConfigModel* getBulletConfigModel() { return m_pBulletConfigModel; }
	BombConfigModel* getBombConfigModel() { return m_pBombConfigModel;  }
	RoleConfigModel* getRoleConfigModel() { return m_pRoleConfigModel; }

	void reload();
private:
	ConfigCenter();
	virtual ~ConfigCenter();

	static ConfigCenter* instance;

	BulletConfigModel* m_pBulletConfigModel;
	BombConfigModel* m_pBombConfigModel;
	RoleConfigModel* m_pRoleConfigModel;
};

#endif
