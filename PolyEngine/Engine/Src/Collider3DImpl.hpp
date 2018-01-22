#pragma once

class btCollisionObject;

namespace Poly
{
	/// This struct is needed because we don't want to declare btCollisionObject in any header
	struct Trigger3DImpl
	{
		btCollisionObject* BulletTrigger = nullptr;
	};
} // namespace Poly