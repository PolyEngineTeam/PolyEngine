#include "EnginePCH.hpp"

#include "BulletIncludes.hpp"

#include "Collider3DImpl.hpp"

//------------------------------------------------------------------------------
Poly::Collider3DComponent::Collider3DComponent(World* world, const Collider3DComponentTemplate& tmp)
	: BodyWorld(world)
{
	Template.Shape = std::make_unique<Physics3DShape>(*tmp.Shape);
	Template.Registered = tmp.Registered;
	Template.CollisionGroup = tmp.CollisionGroup;
	Template.CollisionMask = tmp.CollisionMask;
}
//------------------------------------------------------------------------------
Poly::Collider3DComponent::~Collider3DComponent()
{
	Rigidbody3DComponent* rigidbody = GetSibling<Rigidbody3DComponent>();

	if ((rigidbody && !rigidbody->IsRegistered() && Template.Registered)
		|| (!rigidbody && Template.Registered))
	{
		Physics3DSystem::UnregisterComponent(BodyWorld, GetOwner());
	}
}

//------------------------------------------------------------------------------
void Poly::Collider3DComponent::SetShape(const Physics3DShape& shape)
{
	ImplData->BulletTrigger->setCollisionShape(shape.BulletShape);
	Template.Shape.release();
	Template.Shape = std::make_unique<Physics3DShape>(shape);
}

//------------------------------------------------------------------------------
void Poly::Collider3DComponent::UpdatePosition()
{
	const EntityTransform& transform = GetTransform();
		// TODO: parent can't be nullptr
	//ASSERTE(transCmp->GetParent() == nullptr, "Physics cannot be applied to child entity");

	Vector localTrans = transform.GetLocalTranslation();
	Quaternion localRot = transform.GetLocalRotation();

	btVector3 position(localTrans.X, localTrans.Y, localTrans.Z);
	btQuaternion orientation(localRot.X, localRot.Y, localRot.Z, localRot.W);

	btTransform initialTransform;
	initialTransform.setOrigin(position);
	initialTransform.setRotation(orientation);

	ImplData->BulletTrigger->setWorldTransform(initialTransform);
}
