#pragma once

#include <pe/Defines.hpp>

class btCollisionObject;

namespace Poly
{
	struct Trigger3DImpl : public ::pe::core::BaseObject<>
	{
		btCollisionObject* BulletTrigger = nullptr;
	};
} // namespace Poly