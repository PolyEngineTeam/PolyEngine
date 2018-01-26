#pragma once

#include <BaseObject.hpp>

class btCollisionObject;

namespace Poly
{
	/// This struct is needed because we don't want to declare btCollisionObject in any header
	struct Trigger3DImpl : BaseObject<>
	{
		btCollisionObject* BulletTrigger = nullptr;
	};
} // namespace Poly