#include <EnginePCH.hpp>

#include <Physics2D/Rigidbody2DComponent.hpp>
#include <Physics2D/Physics2DWorldComponent.hpp>
#include <Physics2D/Physics2DColliders.hpp>
#include <Physics2D/RigidBody2DImpl.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::RigidBody2DComponent)

Poly::RigidBody2DComponent::RigidBody2DComponent(Scene* world, eRigidBody2DType type, float density, float friction)
	: BodyWorld(world), BodyType(type)
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

	ImplData->Body = world->GetWorldComponent<Physics2DWorldComponent>()->Scene->CreateBody(&ImplData->BodyDef);
	ASSERTE(ImplData->Body, "Body failed to create!");

	ImplData->FixtureDef.density = density;
	ImplData->FixtureDef.friction = friction;
	ImplData->FixtureDef.userData = this;
}

Poly::RigidBody2DComponent::RigidBody2DComponent(Scene* world, eRigidBody2DType type, RigidBody2DSensorTag /*sensorTag*/)
	: BodyWorld(world), BodyType(type)
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

	ImplData->Body = world->GetWorldComponent<Physics2DWorldComponent>()->Scene->CreateBody(&ImplData->BodyDef);
	ASSERTE(ImplData->Body, "Body failed to create!");

	ImplData->FixtureDef.isSensor = true;
	ImplData->FixtureDef.userData = this;
}

Poly::RigidBody2DComponent::~RigidBody2DComponent()
{
	BodyWorld->GetWorldComponent<Physics2DWorldComponent>()->Scene->DestroyBody(ImplData->Body);
}

void Poly::RigidBody2DComponent::EnsureInit()
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
	const b2Vec2& center = ImplData->Body->GetWorldCenter();
	ImplData->Body->ApplyForce(b2Vec2(force.X, force.Y),center, true);
}

void Poly::RigidBody2DComponent::ApplyImpulseToCenter(const Vector& impulse)
{
	const b2Vec2& center = ImplData->Body->GetWorldCenter();
	ImplData->Body->ApplyLinearImpulse(b2Vec2(impulse.X, impulse.Y), center, true);
}

void Poly::RigidBody2DComponent::SetLinearDamping(float dampfactor)
{
	ImplData->Body->SetLinearDamping(dampfactor);
}

void Poly::RigidBody2DComponent::SetAngularDamping(float dampfactor)
{
	ImplData->Body->SetAngularDamping(dampfactor);
}

void Poly::RigidBody2DComponent::SetFixedRotation(bool fixed)
{
	ImplData->Body->SetFixedRotation(fixed);
}

float Poly::RigidBody2DComponent::GetDensity() const
{
	return ImplData->Fixture->GetDensity();
}

Vector Poly::RigidBody2DComponent::GetLinearVelocity() const
{
	b2Vec2 v = ImplData->Body->GetLinearVelocity();
	return Vector(v.x, v.y, 0);
}

void Poly::RigidBody2DComponent::SetLinearVelocity(const Vector& speed)
{
	ImplData->Body->SetLinearVelocity(b2Vec2(speed.X, speed.Y));
}

float Poly::RigidBody2DComponent::GetAngularVelocity() const
{
	return ImplData->Body->GetAngularVelocity();
}

void Poly::RigidBody2DComponent::SetAngularVelocity(float speed)
{
	ImplData->Body->SetAngularVelocity(speed);
}

void Poly::RigidBody2DComponent::UpdatePosition()
{
	EntityTransform& transform = GetTransform();
	ASSERTE(GetOwner()->IsChildOfRoot(), "Physics cannot be applied to child entity");
	Vector localTrans = transform.GetGlobalTranslation();
	EulerAngles localRot = transform.GetGlobalRotation().ToEulerAngles();

	// Set correct starting pos!
	ImplData->Body->SetTransform(b2Vec2(localTrans.X, localTrans.Y), localRot.Z.AsRadians());
}

void Poly::RigidBody2DComponent::SetDensity(float density)
{
	ImplData->Fixture->SetDensity(density);
}
