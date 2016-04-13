#include "ConfigCenter.h"
#include "BulletConfigModel.h"
#include "BombConfigModel.h"

USING_NS_CC;

ConfigCenter *ConfigCenter::instance = nullptr;

ConfigCenter* ConfigCenter::getInstance()
{
	if (instance == nullptr)
	{
		instance = new ConfigCenter();
	}
	return instance;
}

ConfigCenter::ConfigCenter()
{
	m_pBulletConfigModel = new BulletConfigModel();
	m_pBombConfigModel = new BombConfigModel();
}

ConfigCenter::~ConfigCenter()
{
	if (m_pBulletConfigModel)
	{
		delete(m_pBulletConfigModel);
		m_pBulletConfigModel = nullptr;
	}
	if (m_pBombConfigModel)
	{
		delete(m_pBombConfigModel);
		m_pBombConfigModel = nullptr;
	}
}

