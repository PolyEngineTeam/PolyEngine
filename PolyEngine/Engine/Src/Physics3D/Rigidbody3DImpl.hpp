#pragma once

#include <pe/Defines.hpp>

struct btDefaultMotionState;
class btRigidBody;

namespace Poly
{
	struct Rigidbody3DImpl : BaseObject<>
	{
		btDefaultMotionState* BulletMotionState = nullptr;
		btRigidBody* BulletRigidBody = nullptr;
	};
} // namespace Poly