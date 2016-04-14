#ifndef _SPACEX_CLASSES_ROLECONFIG_H_
#define _SPACEX_CLASSES_ROLECONFIG_H_

#include <cocos2d.h>

class RoleConfig
{
public:
	float m_fWalkVelocity;
	float m_fJumpVelocity;
	float m_fGravity;
	int	  m_iSkill;
};

typedef std::map<std::string, RoleConfig>						RoleConfigMap;
typedef std::map<std::string, RoleConfig>::iterator				RoleConfigIter;
typedef	std::map<std::string, RoleConfig>::const_iterator		RoleConfigCIter;
class RoleConfigModel
{
public:
	RoleConfigModel();
	virtual ~RoleConfigModel();


	const RoleConfigMap&			GetRoleConfigMap(void) { return m_mapRoleConfig; }
private:
	RoleConfigMap m_mapRoleConfig;

	void init(const std::string& fileName);
};

#endif
