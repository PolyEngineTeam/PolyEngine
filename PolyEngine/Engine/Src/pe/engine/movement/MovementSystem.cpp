#include <pe/engine/EnginePCH.hpp>
#include <pe/engine/movement/MovementSystem.hpp>

#include <pe/engine/movement/FreeFloatMovementComponent.hpp>
#include <pe/api/ecs/EntityTransform.hpp>
#include <pe/api/ecs/Scene.hpp>
#include <pe/engine/time/TimeSystem.hpp>
#include <pe/engine/input/InputSystem.hpp>
#include <pe/engine/input/InputWorldComponent.hpp>


namespace pe::engine::movement {

void MovementSystem::onUpdate(api::ecs::Scene* scene)
{
	float deltaTime = (float)(time::TimeSystem::GetTimerDeltaTime(scene, time::eEngineTimer::GAMEPLAY));
	input::InputWorldComponent* inputCmp = scene->getComponent<input::InputWorldComponent>();
	
	if (inputCmp->getIsConsumed())
		return;

	for (auto freeFloatTuple : scene->iterateComponents<FreeFloatMovementComponent>())
	{
		FreeFloatMovementComponent* freeFloatMovementCmp = std::get<FreeFloatMovementComponent*>(freeFloatTuple);
		api::ecs::EntityTransform& trans = freeFloatMovementCmp->getOwner()->getTransform();

		float wheelDelta = freeFloatMovementCmp->getWheelSensitivity() * inputCmp->getWheelPosDelta().Y;
		float currSpeed = freeFloatMovementCmp->getMovementSpeed();
		float newSpeed = core::math::Clamp(currSpeed + wheelDelta, 0.001f, 10000.0f);
		freeFloatMovementCmp->setMovementSpeed(newSpeed);
		
		core::math::Vector move;
		if (inputCmp->isPressed(api::input::eKey::KEY_W))
			move -= core::math::Vector::UNIT_Z;
		else if (inputCmp->isPressed(api::input::eKey::KEY_S))
			move += core::math::Vector::UNIT_Z;

		if (inputCmp->isPressed(api::input::eKey::KEY_A))
			move -= core::math::Vector::UNIT_X;
		else if (inputCmp->isPressed(api::input::eKey::KEY_D))
			move += core::math::Vector::UNIT_X;

		if (inputCmp->isPressed(api::input::eKey::KEY_Q))
			move -= core::math::Vector::UNIT_Y;
		else if (inputCmp->isPressed(api::input::eKey::KEY_E))
			move += core::math::Vector::UNIT_Y;

		if (move.LengthSquared() > 0)
			move.Normalize();

		move *= freeFloatMovementCmp->getMovementSpeed() * deltaTime;

		trans.setLocalTranslation(trans.getLocalTranslation() + trans.getLocalRotation() * move);
		
		if (inputCmp->isPressed(api::input::eMouseButton::LEFT))
		{
			core::math::Vector2i delta = inputCmp->getMousePosDelta();

			core::math::Quaternion rot = core::math::Quaternion(::pe::core::math::Vector::UNIT_Y, 
				core::math::Angle::FromRadians(-delta.X * freeFloatMovementCmp->getAngularVelocity()));
			rot *= trans.getLocalRotation();
			rot *= core::math::Quaternion(::pe::core::math::Vector::UNIT_X, 
				core::math::Angle::FromRadians(-delta.Y * freeFloatMovementCmp->getAngularVelocity()));

			if (rot != core::math::Quaternion()) {
				rot.Normalize();
				trans.setLocalRotation(rot);
			}
		}
	}
}

}