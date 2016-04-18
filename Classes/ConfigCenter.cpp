#include "ConfigCenter.h"
#include "BulletConfigModel.h"
#include "BombConfigModel.h"
#include "RoleConfigModel.h"

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
	//m_pRoleConfigModel = new RoleConfigModel();
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

void ConfigCenter::reload()
{
	if (m_pBulletConfigModel)
		m_pBulletConfigModel->load();

	if (m_pBombConfigModel)
		m_pBombConfigModel->load();
}

