#include "EnginePCH.hpp"

#include "btBulletCollisionCommon.h"

#include "Trigger3DImpl.hpp"

//********************************************************************************************************************************************
Poly::Trigger3DComponent::Trigger3DComponent(World* world, Physics3DShape* shape)
	: BodyWorld(world)
{
	ImplData = std::make_unique<Trigger3DImpl>();
	ImplData.get()->Shape = shape;

	ImplData.get()->BulletTrigger->setCollisionShape(ImplData.get()->Shape->BulletShape);
}

//********************************************************************************************************************************************
Poly::Trigger3DComponent::~Trigger3DComponent()
{
	Physics3DSystem::UnregisterTriger(BodyWorld, GetOwnerID());
}

const Poly::Physics3DShape& Poly::Trigger3DComponent::GetShape()
{
	return *ImplData.get()->Shape;
}

void Poly::Trigger3DComponent::EnsureInit()
{
	if (ImplData.get()->BulletTrigger || GetSibling<Rigidbody3DComponent>())
		return;

	ImplData.get()->BulletTrigger = new btCollisionObject();
	ImplData.get()->BulletTrigger->setCollisionFlags(ImplData.get()->BulletTrigger->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	Physics3DSystem::RegisterTriger(BodyWorld, GetOwnerID());
	Registered = true;
}
