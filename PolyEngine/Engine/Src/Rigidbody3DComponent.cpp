#include "EnginePCH.hpp"

#include "btBulletDynamicsCommon.h"
#include "Rigidbody3DComponent.hpp"

Poly::Rigidbody3DComponent::Rigidbody3DComponent(World* world, eRigidBody3DType type, Physics3DShape* shape, float mass)
	: BodyType(type)
{
	switch (shape->ShapeType)
	{
	case ePhysics3DShapes::BOX:
	{
		Vector v = reinterpret_cast<Physics3DBoxShape*>(shape)->HalfExtents;
		Shape = new btBoxShape(btVector3(v.X, v.Y, v.Z));
	}
		break;

	case ePhysics3DShapes::SPHERE:
		Shape = new btSphereShape(reinterpret_cast<Physics3DSphereShape*>(shape)->Radius);
		break;
	}
	
	MotionState = new btDefaultMotionState();
	
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

	RigidBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, MotionState, Shape, inertia));
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
