#include "EnginePCH.hpp"

#include "btBulletCollisionCommon.h"

Poly::Trigger3DComponent::Trigger3DComponent()
{
	BulletTrigger = new btCollisionObject();
}

Poly::Trigger3DComponent::~Trigger3DComponent()
{
}
