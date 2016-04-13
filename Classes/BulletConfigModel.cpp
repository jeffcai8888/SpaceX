#include "BulletConfigModel.h"
#include "JsonParser.h"

USING_NS_CC;

std::string bulletConfigName[1] =
{
	"Bullet1Config",
};


BulletConfigModel::BulletConfigModel()
{
	for (int i = 0; i < 1; ++i)
	{
        std::string configName = bulletConfigName[i];
		init(configName);
	}
}

BulletConfigModel::~BulletConfigModel()
{

}

void BulletConfigModel::init(const std::string& fileName)
{
	BulletConfig config;
	JsonParser* parser = JsonParser::createWithFile((fileName + ".json").c_str());
	parser->decodeDebugData();
	auto list = parser->getList();
	for (auto& v : list)
	{
		ValueMap row = v.asValueMap();
		for (auto& pair : row)
		{
			if (pair.first.compare("Power") == 0)
			{
				config.m_iPower = pair.second.asInt();
			}
			else if (pair.first.compare("Speed") == 0)
			{
				config.m_fLaunchVelocity = pair.second.asFloat();
			}
			else if (pair.first.compare("DisappearTime") == 0)
			{
				config.m_fDisappearTime = pair.second.asFloat();
			}
			else if (pair.first.compare("Angle") == 0)
			{
				config.m_iAngle = pair.second.asInt();
			}
			else if (pair.first.compare("Interval") == 0)
			{
				config.m_fInterval = pair.second.asFloat();
			}
			else if (pair.first.compare("Gravity") == 0)
			{
				config.m_fGravity = pair.second.asFloat();
			}
			else if (pair.first.compare("Capacity") == 0)
			{
				config.m_iCapacity = pair.second.asInt();
			}
		}
	}
	m_mapBulletConfig[fileName] = config;
}

