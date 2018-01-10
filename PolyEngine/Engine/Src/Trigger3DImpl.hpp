#pragma once

#include "btBulletCollisionCommon.h"

namespace Poly
{
	class Physics3DShape;

	struct Trigger3DImpl
	{
		Physics3DShape* Shape = nullptr;
		btCollisionObject* BulletTrigger = nullptr;
	};
}