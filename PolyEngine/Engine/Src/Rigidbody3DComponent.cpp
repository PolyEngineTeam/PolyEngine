#include "EnginePCH.hpp"

Poly::Rigidbody3DComponent::Rigidbody3DComponent(World* world, eRigidBody3DType type, btCollisionShape* shape, float mass)
{
	//Shape = std::move(shape);
	//
	//btVector3 inertia(0, 0, 0);
	//if (mass > 0) Shape->calculateLocalInertia(mass, inertia);
	//
	//TransformComponent* trans = world->GetComponent<TransformComponent>(GetOwnerID());
	//Vector translation = trans->GetGlobalTranslation();
	//Quaternion rotation = trans->GetGlobalRotation();

	//btVector3 v(translation.X, translation.Y, translation.Z);
	//btQuaternion q(rotation.X, rotation.Y, rotation.Z, rotation.W);
	//btTransform t(q, v);
	
	//btDefaultMotionState m(t);
	//MotionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(t));
	//
	//RigidBody = std::unique_ptr<btRigidBody>(new btRigidBody({ mass, MotionState.get(), Shape.get(), inertia }));
}
