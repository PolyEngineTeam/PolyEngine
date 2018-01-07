#include "EnginePCH.hpp"

#include "btBulletDynamicsCommon.h"
#include "Rigidbody3DComponent.hpp"

//********************************************************************************************************************************************
Poly::Rigidbody3DComponent::Rigidbody3DComponent(World* world, eRigidBody3DType type, Physics3DShape* shape,
	float restitution, float friction, float rollingFriction, float spinningFriction, float mass)
	: BodyWorld(world),
	Mass(mass),
	Restitution(restitution),
	Friction(friction),
	RollingFriction(rollingFriction),
	SpinningFriction(spinningFriction),
	Shape(shape),
	BodyType(type)
{	
	BulletMotionState = new btDefaultMotionState();
	
	btVector3 inertia(0, 0, 0);

	switch (type)
	{
	case eRigidBody3DType::STATIC:
		break;

	case eRigidBody3DType::DYNAMIC:
		ASSERTE(type != eRigidBody3DType::STATIC && mass > 0, "Can't create dynamic body with 0 mass.");
		shape->BulletShape->calculateLocalInertia(mass, inertia);
		break;
	}

	btRigidBody::btRigidBodyConstructionInfo CI(mass, BulletMotionState, shape->BulletShape, inertia);
	CI.m_restitution = restitution;
	CI.m_friction = friction;
	CI.m_rollingFriction = rollingFriction;
	CI.m_spinningFriction = spinningFriction;

	BulletRigidBody = new btRigidBody(CI);
}

//********************************************************************************************************************************************
Poly::Rigidbody3DComponent::~Rigidbody3DComponent()
{
	Physics3DSystem::UnregisterRigidBody(BodyWorld, GetOwnerID());

	delete BulletRigidBody;
	delete BulletMotionState;
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyForceToCenter(const Vector& force)
{
	BulletRigidBody->applyCentralForce(btVector3(force.X, force.Y, force.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyImpulseToCenter(const Vector& impulse)
{
	BulletRigidBody->applyCentralImpulse(btVector3(impulse.X, impulse.Y, impulse.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyDamping(float timestep)
{
	BulletRigidBody->applyDamping(timestep);
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyForce(const Vector& force, const Vector& relPos)
{
	BulletRigidBody->applyForce(btVector3(force.X, force.Y, force.Z), btVector3(relPos.X, relPos.Y, relPos.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyGravity()
{
	BulletRigidBody->applyGravity();
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyImpulse(const Vector& impulse, const Vector& relPos)
{
	BulletRigidBody->applyImpulse(btVector3(impulse.X, impulse.Y, impulse.Z), btVector3(relPos.X, relPos.Y, relPos.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyTorque(const Vector& torque)
{
	BulletRigidBody->applyTorque(btVector3(torque.X, torque.Y, torque.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyTorqueImpulse(const Vector& torque)
{
	BulletRigidBody->applyTorqueImpulse(btVector3(torque.X, torque.Y, torque.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ClearForces()
{
	BulletRigidBody->clearForces();
}

//********************************************************************************************************************************************
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
	
	BulletRigidBody->setWorldTransform(initialTransform);
	BulletMotionState->setWorldTransform(initialTransform);
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetLinearFactor(const Vector& factor)
{
	BulletRigidBody->setLinearFactor(btVector3(factor.X, factor.Y, factor.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetLinearVelocity(const Vector& velocity)
{
	BulletRigidBody->setLinearVelocity(btVector3(velocity.X, velocity.Y, velocity.Z));
}

//********************************************************************************************************************************************
const Poly::Vector& Poly::Rigidbody3DComponent::GetLinearFactor()
{
	const btVector3& v = BulletRigidBody->getLinearFactor();
	return Vector(v.x(), v.y(), v.z());
}

//********************************************************************************************************************************************
const Poly::Vector& Poly::Rigidbody3DComponent::GetLinearVelocity()
{
	const btVector3& v = BulletRigidBody->getLinearVelocity();
	return Vector(v.x(), v.y(), v.z());
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetAngularFactor(float factor)
{
	BulletRigidBody->setAngularFactor(factor);
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetAngularFactor(const Vector& factor)
{
	BulletRigidBody->setAngularFactor(btVector3(factor.X, factor.Y, factor.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetAngularVelocity(const Vector& velocity)
{
	BulletRigidBody->setAngularVelocity(btVector3(velocity.X, velocity.Y, velocity.Z));
}

//********************************************************************************************************************************************
const Poly::Vector& Poly::Rigidbody3DComponent::GetAngularFactor()
{
	const btVector3& v = BulletRigidBody->getAngularFactor();
	return Vector(v.x(), v.y(), v.z());
}

//********************************************************************************************************************************************
const Poly::Vector& Poly::Rigidbody3DComponent::GetAngularVelocity()
{
	const btVector3& v = BulletRigidBody->getAngularVelocity();
	return Vector(v.x(), v.y(), v.z());
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetDamping(float linearDamping, float angularDamping)
{
	BulletRigidBody->setDamping(linearDamping, angularDamping);
}

//********************************************************************************************************************************************
float Poly::Rigidbody3DComponent::GetLinearDamping()
{
	return BulletRigidBody->getLinearDamping();
}

//********************************************************************************************************************************************
float Poly::Rigidbody3DComponent::GetAngularDamping()
{
	return BulletRigidBody->getAngularDamping();
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetGravity(const Vector&  gravity)
{
	BulletRigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
}

//********************************************************************************************************************************************
const Poly::Vector& Poly::Rigidbody3DComponent::GetGravity()
{
	const btVector3& g = BulletRigidBody->getGravity();
	return Vector(g.x(), g.y(), g.z());
}
