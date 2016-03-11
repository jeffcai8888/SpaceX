#ifndef __SPACEX_CLASSES_PHYSICSWORLDMANAGER_H__
#define __SPACEX_CLASSES_PHYSICSWORLDMANAGER_H__

#include <Box2D/Box2D.h>
#include <cocos2d.h>
#include "GLES-Render.h"

class PhysicsWorldManager
{
public:
	static PhysicsWorldManager* getInstance();

	void init();
	void update(float dt);
	void draw();

	CC_SYNTHESIZE_READONLY(b2World*, m_pWorld, World);
	CC_SYNTHESIZE_READONLY(GLESDebugDraw*, m_pDebugDraw, DebugDraw);

private:
	PhysicsWorldManager();
	virtual ~PhysicsWorldManager();

	static PhysicsWorldManager* instance;
};

#endif  //__SPACEX_CLASSES_PHYSICSWORLDMANAGER_H__