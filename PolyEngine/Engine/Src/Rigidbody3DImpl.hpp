#pragma once

#include "btBulletDynamicsCommon.h"

namespace Poly
{
	struct Trigger3DImpl;
	
	struct Rigidbody3DImpl
	{
		btDefaultMotionState* BulletMotionState = nullptr;
		btRigidBody* BulletRigidBody = nullptr;
	};
}