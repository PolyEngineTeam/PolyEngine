#pragma once

struct btDefaultMotionState;
class btRigidBody;

namespace Poly
{
	/// This struct is needed because we don't want to declare btDefaultMotionState in any header
	struct Rigidbody3DImpl
	{
		btDefaultMotionState* BulletMotionState = nullptr;
		btRigidBody* BulletRigidBody = nullptr;
	};
} // namespace Poly