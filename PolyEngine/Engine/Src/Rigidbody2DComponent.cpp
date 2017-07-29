#include "EnginePCH.hpp"

#include "Rigidbody2DComponent.hpp"

#include "Physics2DWorldComponent.hpp"
#include "Physics2DColliders.hpp"
#include "World.hpp"
#include "RigidBody2DImpl.hpp"

#include <Box2D/Box2D.h>

using namespace Poly;

Poly::RigidBody2DComponent::RigidBody2DComponent(World* world, eRigidBody2DType type, float density, float friction)
	: BodyType(type)
{
	ImplData = std::make_unique<RigidBody2DData>();

	switch (BodyType)
	{
	case eRigidBody2DType::STATIC:
		ImplData->BodyDef.type = b2_staticBody;
		break;
	case eRigidBody2DType::KINEMATIC:
		ImplData->BodyDef.type = b2_kinematicBody;
		break;
	case eRigidBody2DType::DYNAMIC:
		ImplData->BodyDef.type = b2_dynamicBody;
		break;
	default:
		ASSERTE(false, "Invalid body type");
	}

	ImplData->Body = world->GetWorldComponent<Physics2DWorldComponent>()->GetPhysicsWorld()->CreateBody(&ImplData->BodyDef);
	ASSERTE(ImplData->Body, "Body failed to create!");

	ImplData->FixtureDef.density = density;
	ImplData->FixtureDef.friction = friction;
	ImplData->FixtureDef.userData = this;
}

Poly::RigidBody2DComponent::RigidBody2DComponent(World* world, eRigidBody2DType type, RigidBody2DSensorTag sensorTag)
{
	ImplData = std::make_unique<RigidBody2DData>();

	switch (BodyType)
	{
	case eRigidBody2DType::STATIC:
		ImplData->BodyDef.type = b2_staticBody;
		break;
	case eRigidBody2DType::KINEMATIC:
		ImplData->BodyDef.type = b2_kinematicBody;
		break;
	case eRigidBody2DType::DYNAMIC:
		ImplData->BodyDef.type = b2_dynamicBody;
		break;
	default:
		ASSERTE(false, "Invalid body type");
	}

	ImplData->Body = world->GetWorldComponent<Physics2DWorldComponent>()->GetPhysicsWorld()->CreateBody(&ImplData->BodyDef);
	ASSERTE(ImplData->Body, "Body failed to create!");

	ImplData->FixtureDef.isSensor = true;
	ImplData->FixtureDef.userData = this;
}

Poly::RigidBody2DComponent::~RigidBody2DComponent()
{
}

void Poly::RigidBody2DComponent::FinishInit()
{
	if (!ImplData->Fixture)
	{
		if (const Box2DColliderComponent* boxCmp = GetSibling<Box2DColliderComponent>())
		{
			ImplData->FixtureDef.shape = boxCmp->GetShape();

			ImplData->Fixture = ImplData->Body->CreateFixture(&ImplData->FixtureDef);
			ASSERTE(ImplData->Fixture, "Fixture failed to create!");
		}
		else if(const Circle2DColliderComponent* circleCmp = GetSibling<Circle2DColliderComponent>())
		{
			ImplData->FixtureDef.shape = circleCmp->GetShape();

			ImplData->Fixture = ImplData->Body->CreateFixture(&ImplData->FixtureDef);
			ASSERTE(ImplData->Fixture, "Fixture failed to create!");
		}
		else
			gConsole.LogError("There is no collider on rigidbody!");
	}
}

void Poly::RigidBody2DComponent::DebugPrintInfo() const
{
	b2Vec2 position = ImplData->Body->GetPosition();
	float32 angle = ImplData->Body->GetAngle();
	gConsole.LogInfo("{} {} {}", position.x, position.y, angle);
}

void Poly::RigidBody2DComponent::ApplyForceToCenter(const Vector& force)
{
	ImplData->Body->ApplyForceToCenter(b2Vec2(force.X, force.Y), true);
}

void Poly::RigidBody2DComponent::ApplyImpulseToCenter(const Vector& impulse)
{
	ImplData->Body->ApplyLinearImpulseToCenter(b2Vec2(impulse.X, impulse.Y), true);
}

void Poly::RigidBody2DComponent::SetDamping(float dampfactor)
{
	ImplData->Body->SetLinearDamping(dampfactor);
}

void Poly::RigidBody2DComponent::SetFixedRotation(bool fixed)
{
	ImplData->Body->SetFixedRotation(fixed);
}

float Poly::RigidBody2DComponent::GetDensity() const
{
	return ImplData->Fixture->GetDensity();
}

Vector Poly::RigidBody2DComponent::GetLinearSpeed() const
{
	b2Vec2 v = ImplData->Body->GetLinearVelocity();
	return Vector(v.x, v.y, 0);
}

void Poly::RigidBody2DComponent::UpdatePosition()
{
	TransformComponent* transform = GetSibling<TransformComponent>();
	ASSERTE(transform, "No transform on physics object!");
	ASSERTE(transform->GetParent() == nullptr, "Physics cannot be applied to child entity");

	Vector localTrans = transform->GetLocalTranslation();
	EulerAngles localRot = transform->GetLocalRotation().ToEulerAngles();

	// Set correct starting pos!
	ImplData->Body->SetTransform(b2Vec2(localTrans.X, localTrans.Y), localRot.Z.AsRadians());
}

void Poly::RigidBody2DComponent::SetDensity(float density)
{
	ImplData->Fixture->SetDensity(density);
}