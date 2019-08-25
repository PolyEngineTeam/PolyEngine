#include <EnginePCH.hpp>

#include <Physics3D/Rigidbody3DComponent.hpp>
#include <Physics3D/Rigidbody3DImpl.hpp>

RTTI_DEFINE_COMPONENT(::Poly::Rigidbody3DComponent)

//------------------------------------------------------------------------------
Poly::Rigidbody3DComponent::Rigidbody3DComponent(Scene* world, const Rigidbody3DComponentTemplate& tmp)
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
void Poly::Rigidbody3DComponent::SetMassProps(float mass, const core::math::Vector& inertia)
{
	Template.Mass = mass;
	Template.Inertia = inertia;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetRestitution(float restitution)
{
	Template.Restitution = restitution;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetFriction(float friction)
{
	Template.Friction = friction;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetRollingFriction(float friction)
{
	Template.RollingFriction = friction;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetDamping(float linearDamping, float angularDamping)
{
	Template.LinearDamping = linearDamping;
	Template.AngularDamping = angularDamping;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetLinearFactor(const core::math::Vector& linearFactor)
{
	Template.LinearFactor = linearFactor;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetAngularFactor(const core::math::Vector& angularFactor)
{
	Template.AngularFactor = angularFactor;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetLinearVelocity(const core::math::Vector& velocity)
{
	Template.LinearVelocity = velocity;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetAngularVelocity(const core::math::Vector& velocity)
{
	Template.AngularVelocity = velocity;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
core::math::Vector Poly::Rigidbody3DComponent::GetLinearVelocity()
{
	const btVector3& v = ImplData->BulletRigidBody->getLinearVelocity();
	return core::math::Vector(v.x(), v.y(), v.z());
}

//------------------------------------------------------------------------------
core::math::Vector Poly::Rigidbody3DComponent::GetAngularVelocity()
{
	const btVector3& v = ImplData->BulletRigidBody->getAngularVelocity();
	return core::math::Vector(v.x(), v.y(), v.z());
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::UpdatePosition()
{
	const EntityTransform& transform = GetTransform();

	core::math::Vector localTrans = transform.GetGlobalTranslation();
	core::math::Quaternion localRot = transform.GetGlobalRotation();

	btVector3 position(localTrans.X, localTrans.Y, localTrans.Z);
	btQuaternion orientation(localRot.X, localRot.Y, localRot.Z, localRot.W);

	btTransform initialTransform;
	initialTransform.setOrigin(position);
	initialTransform.setRotation(orientation);

	ImplData->BulletRigidBody->setWorldTransform(initialTransform);
	ImplData->BulletMotionState->setWorldTransform(initialTransform);
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::ApplyImpulseToCenter(const core::math::Vector& impulse)
{
	ImpulseToCenter = impulse;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::ApplyImpulse(const core::math::Vector& impulse, const core::math::Vector& relPos)
{
	Impulse = impulse;
	ImpulsePos = relPos;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::ApplyTorqueImpulse(const core::math::Vector& torque)
{
	TorqueImpulse = torque;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
void Poly::Rigidbody3DComponent::SetGravity(const core::math::Vector& gravity)
{
	Template.Gravity = gravity;

	TemplateChanged = true;
}

//------------------------------------------------------------------------------
core::math::Vector Poly::Rigidbody3DComponent::GetGravity() const
{
	const btVector3& g = ImplData->BulletRigidBody->getGravity();
	return core::math::Vector(g.x(), g.y(), g.z());
}
