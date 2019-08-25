#include <EnginePCH.hpp>
#include <Movement/MovementSystem.hpp>

#include <Movement/FreeFloatMovementComponent.hpp>
#include <ECS/EntityTransform.hpp>
#include <ECS/Scene.hpp>
#include <Time/TimeSystem.hpp>
#include <Input/InputSystem.hpp>
#include <Input/InputWorldComponent.hpp>


using namespace Poly;

void MovementSystem::MovementUpdatePhase(Scene* world)
{
	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY));
	InputWorldComponent* inputCmp = world->GetWorldComponent<InputWorldComponent>();
	
	if (inputCmp->GetIsConsumed())
		return;

	for (auto freeFloatTuple : world->IterateComponents<FreeFloatMovementComponent>())
	{
		FreeFloatMovementComponent* freeFloatMovementCmp = std::get<FreeFloatMovementComponent*>(freeFloatTuple);
		EntityTransform& trans = freeFloatMovementCmp->GetTransform();

		float wheelDelta = freeFloatMovementCmp->GetWheelSensitivity() * inputCmp->GetWheelPosDelta().Y;
		float currSpeed = freeFloatMovementCmp->GetMovementSpeed();
		float newSpeed = core::math::Clamp(currSpeed + wheelDelta, 0.001f, 10000.0f);
		freeFloatMovementCmp->SetMovementSpeed(newSpeed);
		
		core::math::Vector move;
		if (inputCmp->IsPressed(eKey::KEY_W))
			move -= core::math::Vector::UNIT_Z;
		else if (inputCmp->IsPressed(eKey::KEY_S))
			move += core::math::Vector::UNIT_Z;

		if (inputCmp->IsPressed(eKey::KEY_A))
			move -= core::math::Vector::UNIT_X;
		else if (inputCmp->IsPressed(eKey::KEY_D))
			move += core::math::Vector::UNIT_X;

		if (inputCmp->IsPressed(eKey::KEY_Q))
			move -= core::math::Vector::UNIT_Y;
		else if (inputCmp->IsPressed(eKey::KEY_E))
			move += core::math::Vector::UNIT_Y;

		if (move.LengthSquared() > 0)
			move.Normalize();

		move *= freeFloatMovementCmp->GetMovementSpeed() * deltaTime;

		trans.SetLocalTranslation(trans.GetLocalTranslation() + trans.GetLocalRotation() * move);
		
		if (inputCmp->IsPressed(eMouseButton::LEFT))
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

core::math::Vector MovementSystem::GetLocalForward(const EntityTransform& transform)
{
	return transform.GetLocalRotation() * -core::math::Vector::UNIT_Z;
}

core::math::Vector MovementSystem::GetLocalRight(const EntityTransform& transform)
{
	return transform.GetLocalRotation() * core::math::Vector::UNIT_X;
}

core::math::Vector MovementSystem::GetLocalUp(const EntityTransform& transform)
{
	return transform.GetLocalRotation() * core::math::Vector::UNIT_Y;
}

core::math::Vector MovementSystem::GetGlobalForward(const EntityTransform& transform)
{
	return transform.GetGlobalRotation() * -core::math::Vector::UNIT_Z;
}

core::math::Vector MovementSystem::GetGlobalRight(const EntityTransform& transform)
{
	return transform.GetGlobalRotation() * core::math::Vector::UNIT_X;
}

core::math::Vector MovementSystem::GetGlobalUp(const EntityTransform& transform)
{
	return transform.GetGlobalRotation() * core::math::Vector::UNIT_Y;
}