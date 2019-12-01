#include <pe/engine/EnginePCH.hpp>
#include <pe/engine/movement/MovementSystem.hpp>

#include <pe/engine/movement/FreeFloatMovementComponent.hpp>
#include <pe/api/ecs/EntityTransform.hpp>
#include <pe/api/ecs/Scene.hpp>
#include <pe/engine/time/TimeSystem.hpp>
#include <pe/engine/input/InputSystem.hpp>
#include <pe/engine/input/InputWorldComponent.hpp>


namespace pe::engine::movement {

void MovementSystem::MovementUpdatePhase(api::ecs::Scene* world)
{
	float deltaTime = (float)(time::TimeSystem::GetTimerDeltaTime(world, time::eEngineTimer::GAMEPLAY));
	input::InputWorldComponent* inputCmp = world->GetComponent<input::InputWorldComponent>();
	
	if (inputCmp->GetIsConsumed())
		return;

	for (auto freeFloatTuple : world->IterateComponents<FreeFloatMovementComponent>())
	{
		FreeFloatMovementComponent* freeFloatMovementCmp = std::get<FreeFloatMovementComponent*>(freeFloatTuple);
		api::ecs::EntityTransform& trans = freeFloatMovementCmp->GetOwner()->GetTransform();

		float wheelDelta = freeFloatMovementCmp->GetWheelSensitivity() * inputCmp->GetWheelPosDelta().Y;
		float currSpeed = freeFloatMovementCmp->GetMovementSpeed();
		float newSpeed = core::math::Clamp(currSpeed + wheelDelta, 0.001f, 10000.0f);
		freeFloatMovementCmp->SetMovementSpeed(newSpeed);
		
		core::math::Vector move;
		if (inputCmp->IsPressed(api::input::eKey::KEY_W))
			move -= core::math::Vector::UNIT_Z;
		else if (inputCmp->IsPressed(api::input::eKey::KEY_S))
			move += core::math::Vector::UNIT_Z;

		if (inputCmp->IsPressed(api::input::eKey::KEY_A))
			move -= core::math::Vector::UNIT_X;
		else if (inputCmp->IsPressed(api::input::eKey::KEY_D))
			move += core::math::Vector::UNIT_X;

		if (inputCmp->IsPressed(api::input::eKey::KEY_Q))
			move -= core::math::Vector::UNIT_Y;
		else if (inputCmp->IsPressed(api::input::eKey::KEY_E))
			move += core::math::Vector::UNIT_Y;

		if (move.LengthSquared() > 0)
			move.Normalize();

		move *= freeFloatMovementCmp->GetMovementSpeed() * deltaTime;

		trans.SetLocalTranslation(trans.GetLocalTranslation() + trans.GetLocalRotation() * move);
		
		if (inputCmp->IsPressed(api::input::eMouseButton::LEFT))
		{
			core::math::Vector2i delta = inputCmp->GetMousePosDelta();

			core::math::Quaternion rot = core::math::Quaternion(::pe::core::math::Vector::UNIT_Y, core::math::Angle::FromRadians(-delta.X * freeFloatMovementCmp->GetAngularVelocity()));
			rot *= trans.GetLocalRotation();
			rot *= core::math::Quaternion(::pe::core::math::Vector::UNIT_X, core::math::Angle::FromRadians(-delta.Y * freeFloatMovementCmp->GetAngularVelocity()));

			if (rot != core::math::Quaternion()) {
				rot.Normalize();
				trans.SetLocalRotation(rot);
			}
		}
	}
}

core::math::Vector MovementSystem::GetLocalForward(const api::ecs::EntityTransform& transform)
{
	return transform.GetLocalRotation() * -core::math::Vector::UNIT_Z;
}

core::math::Vector MovementSystem::GetLocalRight(const api::ecs::EntityTransform& transform)
{
	return transform.GetLocalRotation() * core::math::Vector::UNIT_X;
}

core::math::Vector MovementSystem::GetLocalUp(const api::ecs::EntityTransform& transform)
{
	return transform.GetLocalRotation() * core::math::Vector::UNIT_Y;
}

core::math::Vector MovementSystem::GetGlobalForward(const api::ecs::EntityTransform& transform)
{
	return transform.GetGlobalRotation() * -core::math::Vector::UNIT_Z;
}

core::math::Vector MovementSystem::GetGlobalRight(const api::ecs::EntityTransform& transform)
{
	return transform.GetGlobalRotation() * core::math::Vector::UNIT_X;
}

core::math::Vector MovementSystem::GetGlobalUp(const api::ecs::EntityTransform& transform)
{
	return transform.GetGlobalRotation() * core::math::Vector::UNIT_Y;
}
}