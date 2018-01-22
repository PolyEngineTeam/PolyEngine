#include "EnginePCH.hpp"

#include "btBulletCollisionCommon.h"

#include "Collider3DImpl.hpp"

//********************************************************************************************************************************************
Poly::Collider3DComponent::Collider3DComponent(World* world, const Collider3DComponentTemplate& tmp)
	: BodyWorld(world)
{
	Template = std::make_unique<Collider3DComponentTemplate>();

	Template->Shape = tmp.Shape;
	Template->Registered = tmp.Registered;
	Template->CollisionGroup = tmp.CollisionGroup;
	Template->CollisionMask = tmp.CollisionMask;
}

//********************************************************************************************************************************************
Poly::Collider3DComponent::Collider3DComponent(World* world, Collider3DComponentTemplate&& tmp)
	: BodyWorld(world)
{
	*Template = std::move(tmp);
}

//********************************************************************************************************************************************
Poly::Collider3DComponent::~Collider3DComponent()
{
	if (Template->Registered)
		Physics3DSystem::UnregisterCollider(BodyWorld, GetOwnerID());
}

//********************************************************************************************************************************************
void Poly::Collider3DComponent::SetShape(const std::shared_ptr<Physics3DShape>& shape)
{
	Template->Shape = shape;
	ImplData->BulletTrigger->setCollisionShape(shape->BulletShape);
}
