#include "EnginePCH.hpp"

#include "btBulletDynamicsCommon.h"
#include "Rigidbody3DComponent.hpp"

Poly::Rigidbody3DComponent::Rigidbody3DComponent(World* world, eRigidBody3DType type, btCollisionShape* shape, float mass)
	: BodyType(type)
{
	Shape = shape;
	
	TransformComponent* trans = world->GetComponent<TransformComponent>(GetOwnerID());
	Vector translation = trans->GetGlobalTranslation();
	Quaternion rotation = trans->GetGlobalRotation();
	
	MotionState = new btDefaultMotionState(btTransform({ rotation.X, rotation.Y, rotation.Z, rotation.W }, { translation.X, translation.Y, translation.Z }));
	
	btVector3 inertia(0, 0, 0);

	switch (type)
	{
	case eRigidBody3DType::STATIC:
		break;

	case eRigidBody3DType::DYNAMIC:
		ASSERTE(type != eRigidBody3DType::STATIC && mass > 0, "Can't create dynamic body with 0 mass.");
		Shape->calculateLocalInertia(mass, inertia);
		break;
	}

	RigidBody = new btRigidBody({ mass, MotionState, Shape, inertia });
}

Poly::Rigidbody3DComponent::~Rigidbody3DComponent()
{
}

void Poly::Rigidbody3DComponent::UpdatePosition()
{
	TransformComponent* transCmp = GetSibling<TransformComponent>();
	ASSERTE(transCmp, "No transform on physics object!");
	ASSERTE(transCmp->GetParent() == nullptr, "Physics cannot be applied to child entity");

	Vector localTrans = transCmp->GetLocalTranslation();
	Quaternion localRot = transCmp->GetLocalRotation();

	btVector3 position(localTrans.X, localTrans.Y, localTrans.Z);
	btQuaternion orientation(localRot.X, localRot.Y, localRot.Z, localRot.W);

	btTransform initialTransform;
	initialTransform.setOrigin(position);
	initialTransform.setRotation(orientation);

	RigidBody->setWorldTransform(initialTransform);
	MotionState->setWorldTransform(initialTransform);
}
