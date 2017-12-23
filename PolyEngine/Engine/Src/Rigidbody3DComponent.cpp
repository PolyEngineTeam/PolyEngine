#include "EnginePCH.hpp"

#include "btBulletDynamicsCommon.h"

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

	case eRigidBody3DType::KINEMATIC:
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
