#include <math.h>
#include "Macro.h"
#include "OfflineGameLayer.h"
#include "Hero.h"
#include "Gunner.h"
#include "Princess.h"
#include "Bullet.h"
#include "Foresight.h"
#include "OperateLayer.h"
#include "JsonParser.h"

#include "SceneManager.h"
#include "SimpleAudioEngine.h"
#include "SocketManager.h"

USING_NS_CC;

OfflineGameLayer::OfflineGameLayer()
	:GameLayer()
{
	
}

OfflineGameLayer::~OfflineGameLayer()
{
	
}

void OfflineGameLayer::onExit()
{
	GameLayer::onExit();
}
