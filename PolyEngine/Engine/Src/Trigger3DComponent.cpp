#include "EnginePCH.hpp"

#include "btBulletCollisionCommon.h"

//********************************************************************************************************************************************
Poly::Trigger3DComponent::Trigger3DComponent(Physics3DShape* shape)
	: Shape(shape)
{
	BulletTrigger = new btCollisionObject();
	BulletTrigger->setCollisionShape(shape->BulletShape);
	BulletTrigger->setCollisionFlags(BulletTrigger->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

//********************************************************************************************************************************************
Poly::Trigger3DComponent::~Trigger3DComponent()
{
	Physics3DSystem::UnregisterTriger(BodyWorld, GetOwnerID());

	delete BulletTrigger;
}
