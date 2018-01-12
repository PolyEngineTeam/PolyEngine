#pragma once

#include "btBulletDynamicsCommon.h"

namespace Poly
{
	struct Rigidbody3DImpl
	{
		btDefaultMotionState* BulletMotionState = nullptr;
		btRigidBody* BulletRigidBody = nullptr;
	};
}