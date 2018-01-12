#include "EnginePCH.hpp"

#include "btBulletCollisionCommon.h"

#include "Trigger3DImpl.hpp"

//********************************************************************************************************************************************
Poly::Trigger3DComponent::Trigger3DComponent(World* world, Physics3DShape* shape)
	: BodyWorld(world)
{
	ImplData = new Trigger3DImpl();
	ImplData->Shape = shape;
}

//********************************************************************************************************************************************
Poly::Trigger3DComponent::~Trigger3DComponent()
{
	Physics3DSystem::UnregisterTriger(BodyWorld, GetOwnerID());
}

//********************************************************************************************************************************************
const Poly::Physics3DShape& Poly::Trigger3DComponent::GetShape()
{
	return *ImplData->Shape;
}

//********************************************************************************************************************************************
void Poly::Trigger3DComponent::EnsureInit()
{
	if (ImplData->BulletTrigger || GetSibling<Rigidbody3DComponent>())
		return;
	
	ImplData->BulletTrigger = new btCollisionObject();
	ImplData->BulletTrigger->setCollisionShape(ImplData->Shape->BulletShape);
	ImplData->BulletTrigger->setCollisionFlags(ImplData->BulletTrigger->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	
	Physics3DSystem::RegisterTriger(BodyWorld, GetOwnerID());
	Registered = true;
}
