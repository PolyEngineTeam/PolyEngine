#include "EnginePCH.hpp"

#include "Physics2DSystem.hpp"

#include "TimeSystem.hpp"
#include "RigidBody2DImpl.hpp"

#include <Box2D/Box2D.h>

void Poly::Physics2DSystem::Physics2DUpdatePhase(World* world)
{
	float deltaTime = (float)TimeSystem::GetTimerDeltaTime(world, eEngineTimer::GAMEPLAY);
	Physics2DWorldComponent* physicsCmp = world->GetWorldComponent<Physics2DWorldComponent>();
	physicsCmp->LastDeltaOverflow += deltaTime;

	for (auto& tuple : world->IterateComponents<RigidBody2DComponent, TransformComponent>())
	{
		RigidBody2DComponent* rigidBody = std::get<RigidBody2DComponent*>(tuple);
		rigidBody->UpdatePosition();
		rigidBody->EnsureInit();
	}

	// maintain fixed step
	while (physicsCmp->LastDeltaOverflow >= physicsCmp->Config.TimeStep)
	{
		physicsCmp->LastDeltaOverflow -= physicsCmp->Config.TimeStep;
		physicsCmp->World->Step(physicsCmp->Config.TimeStep, physicsCmp->Config.VelocityIterations, physicsCmp->Config.PositionIterations);
	}

	for (auto& tuple : world->IterateComponents<RigidBody2DComponent, TransformComponent>())
	{
		RigidBody2DComponent* rigidBody = std::get<RigidBody2DComponent*>(tuple);
		TransformComponent* transform = std::get<TransformComponent*>(tuple);

		b2Vec2 pos = rigidBody->ImplData->Body->GetPosition();
		float rot = rigidBody->ImplData->Body->GetAngle();
		Vector localTrans = transform->GetLocalTranslation();
		EulerAngles localrot = transform->GetLocalRotation().ToEulerAngles();

		localTrans.X = pos.x;
		localTrans.Y = pos.y;
		localrot.Z = Angle::FromRadians(rot);

		transform->SetLocalTranslation(localTrans);
		transform->SetLocalRotation(Quaternion(localrot));
	}
}
