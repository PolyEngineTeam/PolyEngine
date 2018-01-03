#include "EnginePCH.hpp"

#include "btBulletDynamicsCommon.h"
#include "Rigidbody3DComponent.hpp"

Poly::Rigidbody3DComponent::Rigidbody3DComponent(World* world, eRigidBody3DType type, Physics3DShape* shape, float mass)
	: BodyType(type), 
	BodyWorld(world)
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
	btRigidBody::btRigidBodyConstructionInfo CI(mass, MotionState, Shape, inertia);
	CI.m_restitution = 0.7;
	RigidBody = new btRigidBody(CI);
	//Physics3DSystem::RegisterRigidbody(world, GetOwnerID());
}

Poly::Rigidbody3DComponent::~Rigidbody3DComponent()
{
	Physics3DSystem::UnregisterRigidBody(BodyWorld, GetOwnerID());

	delete RigidBody;
	delete MotionState;
	delete Shape;
}

void Poly::Rigidbody3DComponent::ApplyForceToCenter(const Vector& force)
{
	RigidBody->applyCentralForce(btVector3(force.X, force.Y, force.Z));
}

void Poly::Rigidbody3DComponent::ApplyImpulseToCenter(const Vector& impulse)
{
	RigidBody->applyCentralImpulse(btVector3(impulse.X, impulse.Y, impulse.Z));
}

void Poly::Rigidbody3DComponent::ApplyDamping(float timestep)
{
	RigidBody->applyDamping(timestep);
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

void Poly::Rigidbody3DComponent::SetLinearFactor(const Vector& factor)
{
	RigidBody->setLinearFactor(btVector3(factor.X, factor.Y, factor.Z));
}

void Poly::Rigidbody3DComponent::ApplyGravity()
{
	RigidBody->applyGravity();
}

void Poly::Rigidbody3DComponent::SetDamping(float linearDamping, float angularDamping)
{
	RigidBody->setDamping(linearDamping, angularDamping);
}
