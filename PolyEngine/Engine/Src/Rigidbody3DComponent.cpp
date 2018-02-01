#include "EnginePCH.hpp"

#include "BulletIncludes.hpp"

#include "Rigidbody3DImpl.hpp"

//------------------------------------------------------------------------------
Poly::Rigidbody3DComponent::Rigidbody3DComponent(World* world, const Rigidbody3DComponentTemplate& tmp)
	: BodyWorld(world)
{
	Template = tmp;
}

//------------------------------------------------------------------------------
Poly::Rigidbody3DComponent::~Rigidbody3DComponent()
{
	if (Template.Registered)
		Physics3DSystem::UnregisterComponent(BodyWorld, GetOwner());
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetMassProps(float mass, const Vector& intertia)
{
	Template.Mass = mass;
	Template.Intertia = intertia;

	ImplData->BulletRigidBody->setMassProps(mass, btVector3(intertia.X, intertia.Y, intertia.Z));
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetRestitution(float restitution)
{
	Template.Restitution = restitution;

	ImplData->BulletRigidBody->setRestitution(restitution);
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetFriction(float friction)
{
	Template.Friction = friction;

	ImplData->BulletRigidBody->setFriction(friction);
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetRollingFriction(float friction)
{
	Template.RollingFriction;

	ImplData->BulletRigidBody->setRollingFriction(friction);
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetDamping(float linearDamping, float angularDamping)
{
	Template.LinearDamping = linearDamping;
	Template.AngularDamping = angularDamping;

	ImplData->BulletRigidBody->setDamping(linearDamping, angularDamping);
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetLinearFactor(const Vector& linearFactor)
{
	Template.LinearFactor = linearFactor;

	ImplData->BulletRigidBody->setLinearFactor(btVector3(linearFactor.X, linearFactor.Y, linearFactor.Z));
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetAngularFactor(const Vector& angularFactor)
{
	Template.AngularFactor = angularFactor;

	ImplData->BulletRigidBody->setAngularFactor(btVector3(angularFactor.X, angularFactor.Y, angularFactor.Z));
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetLinearVelocity(const Vector& velocity)
{
	ImplData->BulletRigidBody->setLinearVelocity(btVector3(velocity.X, velocity.Y, velocity.Z));
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetAngularVelocity(const Vector& velocity)
{
	ImplData->BulletRigidBody->setAngularVelocity(btVector3(velocity.X, velocity.Y, velocity.Z));
}

//------------------------------------------------------------------------------
Poly::Vector Poly::Rigidbody3DComponent::GetLinearVelocity()
{
	const btVector3& v = ImplData->BulletRigidBody->getLinearVelocity();
	return Vector(v.x(), v.y(), v.z());
}

//------------------------------------------------------------------------------
Poly::Vector Poly::Rigidbody3DComponent::GetAngularVelocity()
{
	const btVector3& v = ImplData->BulletRigidBody->getAngularVelocity();
	return Vector(v.x(), v.y(), v.z());
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::UpdatePosition()
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

	ImplData->BulletRigidBody->setWorldTransform(initialTransform);
	ImplData->BulletMotionState->setWorldTransform(initialTransform);
}

	//TODO(squares): test these

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::ApplyForceToCenter(const Vector& force)
{
	ImplData->BulletRigidBody->applyCentralForce(btVector3(force.X, force.Y, force.Z));
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::ApplyImpulseToCenter(const Vector& impulse)
{
	ImplData->BulletRigidBody->applyCentralImpulse(btVector3(impulse.X, impulse.Y, impulse.Z));
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::ApplyDamping(float timestep)
{
	ImplData->BulletRigidBody->applyDamping(timestep);
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::ApplyForce(const Vector& force, const Vector& relPos)
{
	ImplData->BulletRigidBody->applyForce(btVector3(force.X, force.Y, force.Z), btVector3(relPos.X, relPos.Y, relPos.Z));
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::ApplyGravity()
{
	ImplData->BulletRigidBody->applyGravity();
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::ApplyImpulse(const Vector& impulse, const Vector& relPos)
{
	ImplData->BulletRigidBody->applyImpulse(btVector3(impulse.X, impulse.Y, impulse.Z), btVector3(relPos.X, relPos.Y, relPos.Z));
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::ApplyTorque(const Vector& torque)
{
	ImplData->BulletRigidBody->applyTorque(btVector3(torque.X, torque.Y, torque.Z));
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::ApplyTorqueImpulse(const Vector& torque)
{
	ImplData->BulletRigidBody->applyTorqueImpulse(btVector3(torque.X, torque.Y, torque.Z));
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::ClearForces()
{
	ImplData->BulletRigidBody->clearForces();
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetGravity(const Vector&  gravity)
{
	ImplData->BulletRigidBody->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
}

//------------------------------------------------------------------------------
Poly::Vector Poly::Rigidbody3DComponent::GetGravity() const
{
	const btVector3& g = ImplData->BulletRigidBody->getGravity();
	return Vector(g.x(), g.y(), g.z());
}
