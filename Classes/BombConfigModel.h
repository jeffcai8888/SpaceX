#ifndef _SPACEX_CLASSES_BOMBCONFIG_H_
#define _SPACEX_CLASSES_BOMBCONFIG_H_

#include <cocos2d.h>

class BombConfig
{
public:
	float m_fVelocity;
	float m_fRange;
	float m_fStartTime;
	float m_fCDTime;
	int	  m_iPower;
	float m_fGravity;
};

typedef std::map<std::string, BombConfig>						BombConfigMap;
typedef std::map<std::string, BombConfig>::iterator				BombConfigIter;
typedef	std::map<std::string, BombConfig>::const_iterator		BombConfigCIter;
class BombConfigModel
{
public:
	BombConfigModel();
	virtual ~BombConfigModel();


	const BombConfigMap&			GetBombConfigMap(void) { return m_mapBombConfig; }
private:
	BombConfigMap m_mapBombConfig;

	void init(const std::string& fileName);

};

#endif
