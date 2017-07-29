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
}

Poly::RigidBody2DComponent::~RigidBody2DComponent()
{
}

void Poly::RigidBody2DComponent::FinishInit()
{
	if (!ImplData->Fixture)
	{
		TransformComponent* transform = GetSibling<TransformComponent>();
		ASSERTE(transform, "No transform on physics object!");
		ASSERTE(transform->GetParent() == nullptr, "Physics cannot be applied to child entity");

		Vector localTrans = transform->GetLocalTranslation();
		EulerAngles localRot = transform->GetLocalRotation().ToEulerAngles();

		// Set correct starting pos!
		ImplData->Body->SetTransform(b2Vec2(localTrans.X, localTrans.Y), localRot.Z.AsRadians());

		if (const Box2DColliderComponent* boxCmp = GetSibling<Box2DColliderComponent>())
		{
			ImplData->FixtureDef.shape = boxCmp->GetShape();

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
