#include "EnginePCH.hpp"

#include "btBulletDynamicsCommon.h"
#include "Rigidbody3DComponent.hpp"

#include "Rigidbody3DImpl.hpp"
#include "Trigger3DImpl.hpp"

//********************************************************************************************************************************************
Poly::Rigidbody3DComponent::Rigidbody3DComponent(World* world, eRigidBody3DType type, float mass)
	: BodyWorld(world),
	BodyType(type),
	Mass(mass)
{
}

//********************************************************************************************************************************************
Poly::Rigidbody3DComponent::~Rigidbody3DComponent()
{
	Physics3DSystem::UnregisterRigidBody(BodyWorld, GetOwnerID());
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyForceToCenter(const Vector& force)
{
	ImplData.get()->BulletRigidBody->applyCentralForce(btVector3(force.X, force.Y, force.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyImpulseToCenter(const Vector& impulse)
{
	ImplData.get()->BulletRigidBody->applyCentralImpulse(btVector3(impulse.X, impulse.Y, impulse.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyDamping(float timestep)
{
	ImplData.get()->BulletRigidBody->applyDamping(timestep);
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyForce(const Vector& force, const Vector& relPos)
{
	ImplData.get()->BulletRigidBody->applyForce(btVector3(force.X, force.Y, force.Z), btVector3(relPos.X, relPos.Y, relPos.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyGravity()
{
	ImplData.get()->BulletRigidBody->applyGravity();
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyImpulse(const Vector& impulse, const Vector& relPos)
{
	ImplData.get()->BulletRigidBody->applyImpulse(btVector3(impulse.X, impulse.Y, impulse.Z), btVector3(relPos.X, relPos.Y, relPos.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyTorque(const Vector& torque)
{
	ImplData.get()->BulletRigidBody->applyTorque(btVector3(torque.X, torque.Y, torque.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ApplyTorqueImpulse(const Vector& torque)
{
	ImplData.get()->BulletRigidBody->applyTorqueImpulse(btVector3(torque.X, torque.Y, torque.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::ClearForces()
{
	ImplData.get()->BulletRigidBody->clearForces();
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
	
	ImplData.get()->BulletRigidBody->setWorldTransform(initialTransform);
	ImplData.get()->BulletMotionState->setWorldTransform(initialTransform);
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetLinearFactor(const Vector& factor)
{
	ImplData.get()->BulletRigidBody->setLinearFactor(btVector3(factor.X, factor.Y, factor.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetLinearVelocity(const Vector& velocity)
{
	ImplData.get()->BulletRigidBody->setLinearVelocity(btVector3(velocity.X, velocity.Y, velocity.Z));
}

//********************************************************************************************************************************************
const Poly::Vector& Poly::Rigidbody3DComponent::GetLinearFactor()
{
	const btVector3& v = ImplData.get()->BulletRigidBody->getLinearFactor();
	return Vector(v.x(), v.y(), v.z());
}

//********************************************************************************************************************************************
const Poly::Vector& Poly::Rigidbody3DComponent::GetLinearVelocity()
{
	const btVector3& v = ImplData.get()->BulletRigidBody->getLinearVelocity();
	return Vector(v.x(), v.y(), v.z());
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetAngularFactor(float factor)
{
	ImplData.get()->BulletRigidBody->setAngularFactor(factor);
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetAngularFactor(const Vector& factor)
{
	ImplData.get()->BulletRigidBody->setAngularFactor(btVector3(factor.X, factor.Y, factor.Z));
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetAngularVelocity(const Vector& velocity)
{
	ImplData.get()->BulletRigidBody->setAngularVelocity(btVector3(velocity.X, velocity.Y, velocity.Z));
}

//********************************************************************************************************************************************
const Poly::Vector& Poly::Rigidbody3DComponent::GetAngularFactor()
{
	const btVector3& v = ImplData.get()->BulletRigidBody->getAngularFactor();
	return Vector(v.x(), v.y(), v.z());
}

//********************************************************************************************************************************************
const Poly::Vector& Poly::Rigidbody3DComponent::GetAngularVelocity()
{
	const btVector3& v = ImplData.get()->BulletRigidBody->getAngularVelocity();
	return Vector(v.x(), v.y(), v.z());
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetRestitution(float restitution)
{
	ImplData.get()->BulletRigidBody->setRestitution(restitution);
}

//********************************************************************************************************************************************
float Poly::Rigidbody3DComponent::GetRestitution()
{
	return ImplData.get()->BulletRigidBody->getRestitution();
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetFriction(float friction)
{
	ImplData.get()->BulletRigidBody->setFriction(friction);
}

//********************************************************************************************************************************************
float Poly::Rigidbody3DComponent::GetFriction()
{
	return ImplData.get()->BulletRigidBody->getFriction();
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetRollingFriction(float friction)
{
	ImplData.get()->BulletRigidBody->setRollingFriction(friction);
}

//********************************************************************************************************************************************
float Poly::Rigidbody3DComponent::GetRollingFriction()
{
	return ImplData.get()->BulletRigidBody->getRollingFriction();
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetDamping(float linearDamping, float angularDamping)
{
	ImplData.get()->BulletRigidBody->setDamping(linearDamping, angularDamping);
}

//********************************************************************************************************************************************
float Poly::Rigidbody3DComponent::GetLinearDamping()
{
	return ImplData.get()->BulletRigidBody->getLinearDamping();
}

//********************************************************************************************************************************************
float Poly::Rigidbody3DComponent::GetAngularDamping()
{
	return ImplData.get()->BulletRigidBody->getAngularDamping();
}

//********************************************************************************************************************************************
void Poly::Rigidbody3DComponent::SetGravity(const Vector&  gravity)
{
	ImplData.get()->BulletRigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
}

//********************************************************************************************************************************************
const Poly::Vector& Poly::Rigidbody3DComponent::GetGravity()
{
	const btVector3& g = ImplData.get()->BulletRigidBody->getGravity();
	return Vector(g.x(), g.y(), g.z());
}

void Poly::Rigidbody3DComponent::EnsureInit()
{
	if (ImplData.get())
		return;
	
	ImplData = std::make_unique<Rigidbody3DImpl>();
	ImplData.get()->BulletMotionState = new btDefaultMotionState();
	
	btCollisionShape* shape = GetSibling<Trigger3DComponent>()->GetShape().BulletShape;
	
	btVector3 inertia(0, 0, 0);
	
	switch (BodyType)
	{
	case eRigidBody3DType::STATIC:
		break;
	
	case eRigidBody3DType::DYNAMIC:
		ASSERTE(BodyType != eRigidBody3DType::STATIC && Mass > 0, "Can't create dynamic body with 0 mass.");
		shape->calculateLocalInertia(Mass, inertia);
		break;
	}
	
	btRigidBody::btRigidBodyConstructionInfo CI(Mass, ImplData.get()->BulletMotionState, shape, inertia);
	
	ImplData.get()->BulletRigidBody = new btRigidBody(CI);
	GetSibling<Trigger3DComponent>()->ImplData->BulletTrigger = ImplData.get()->BulletRigidBody;
	
	Physics3DSystem::RegisterRigidbody(BodyWorld, GetOwnerID());
	Registered = true;
}
