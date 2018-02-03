#pragma once

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace Poly
{
	struct Physics3DWorldComponentImplData
	{
		btDiscreteDynamicsWorld* DynamicsWorld;

		btDefaultCollisionConfiguration* CollisionConfiguration;
		btCollisionDispatcher* Dispatcher;
		btBroadphaseInterface* Broadphase;
		btSequentialImpulseConstraintSolver* Solver;
	};

} // namespace Poly