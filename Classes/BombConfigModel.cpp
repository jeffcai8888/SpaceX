#include "BombConfigModel.h"
#include "JsonParser.h"

USING_NS_CC;

std::string bombConfigName[1] =
{
	"Bomb1Config",
};


BombConfigModel::BombConfigModel()
{
	load();
}

BombConfigModel::~BombConfigModel()
{

}

void BombConfigModel::load()
{
	for (unsigned int i = 0; i < 1; ++i)
	{
		std::string configName = bombConfigName[i];
		load(configName);
	}
}

void BombConfigModel::load(const std::string& fileName)
{
	BombConfig config;
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
				config.m_fVelocity = pair.second.asFloat();
			}
			else if (pair.first.compare("Range") == 0)
			{
				config.m_fRange = pair.second.asInt();
			}
			else if (pair.first.compare("StartTime") == 0)
			{
				config.m_fStartTime = pair.second.asFloat();
			}
			else if (pair.first.compare("CDTime") == 0)
			{
				config.m_fCDTime = pair.second.asInt();
			}
			else if (pair.first.compare("Gravity") == 0)
			{
				config.m_fGravity = pair.second.asFloat();
			}
		}
	}
	m_mapBombConfig[fileName] = config;
}

