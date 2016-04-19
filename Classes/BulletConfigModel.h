#ifndef _SPACEX_CLASSES_BULLETCONFIG_H_
#define _SPACEX_CLASSES_BULLETCONFIG_H_

#include <cocos2d.h>

class BulletConfig
{
public:
	float	m_fDisappearTime;
	float	m_fLaunchVelocity;
	int		m_iAngle;
	int		m_iPower;
	float	m_fInterval;
	int		m_iCapacity;
	float	m_fGravity;
};

typedef std::map<std::string, BulletConfig>						BulletConfigMap;
typedef std::map<std::string, BulletConfig>::iterator			BulletConfigIter;
typedef	std::map<std::string, BulletConfig>::const_iterator		BulletConfigCIter;
class BulletConfigModel
{
public:
	BulletConfigModel();
	virtual ~BulletConfigModel();


	const BulletConfigMap&			GetBulletConfigMap(void) { return m_mapBulletConfig; }
	void load();
private:
	BulletConfigMap m_mapBulletConfig;
	void load(const std::string& fileName);
};

#endif
