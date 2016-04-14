#include "RoleConfigModel.h"
#include "JsonParser.h"

USING_NS_CC;

std::string roleConfigName[3] =
{
	"Hero",
	"Gun",
	"Pricess"
};


RoleConfigModel::RoleConfigModel()
{
	int size = roleConfigName->size();
	for (int i = 0; i < size; ++i)
	{
        std::string configName = roleConfigName[i];
		init(configName);
	}
}

RoleConfigModel::~RoleConfigModel()
{

}

void RoleConfigModel::init(const std::string& fileName)
{
	RoleConfig config;
	JsonParser* parser = JsonParser::createWithFile((fileName + ".json").c_str());
	parser->decodeDebugData();
	auto list = parser->getList();
	for (auto& v : list)
	{
		ValueMap row = v.asValueMap();
		for (auto& pair : row)
		{
			if (pair.first.compare("WalkSpeed") == 0)
			{
				config.m_fWalkVelocity = pair.second.asInt();
			}
			else if (pair.first.compare("JumpSpeed") == 0)
			{
				config.m_fJumpVelocity = pair.second.asFloat();
			}
			else if (pair.first.compare("Gravity") == 0)
			{
				config.m_fGravity = pair.second.asFloat();
			}
			else if (pair.first.compare("Skill") == 0)
			{
				config.m_iSkill = pair.second.asInt();
			}
		}
	}
	m_mapRoleConfig[fileName] = config;
}

