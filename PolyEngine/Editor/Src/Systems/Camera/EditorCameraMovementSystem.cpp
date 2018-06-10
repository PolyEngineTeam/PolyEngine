#include "PolyEditorPCH.hpp"

#include "Time/TimeSystem.hpp"
#include "Input/InputWorldComponent.hpp"

//------------------------------------------------------------------------------
void EditorCameraMovementSystem::Update(World* world)
{
	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY));
	InputWorldComponent* inputCmp = world->GetWorldComponent<InputWorldComponent>();

	for (auto freeFloatTuple : world->IterateComponents<EditorCameraMovementComponent>())
	{
		EditorCameraMovementComponent* freeFloatMovementCmp = std::get<EditorCameraMovementComponent*>(freeFloatTuple);
		EntityTransform& trans = freeFloatMovementCmp->GetTransform();

		int wheelDelta = inputCmp->GetWheelPosDelta().Y;
		float speed = freeFloatMovementCmp->GetMovementSpeed();
		speed = Clamp(speed + wheelDelta, 0.001f, 10000.0f);
		freeFloatMovementCmp->SetMovementSpeed(speed);

		Vector move;
		if (inputCmp->IsPressed(eKey::KEY_W))
			move -= Vector::UNIT_Z;
		else if (inputCmp->IsPressed(eKey::KEY_S))
			move += Vector::UNIT_Z;

		if (inputCmp->IsPressed(eKey::KEY_A))
			move -= Vector::UNIT_X;
		else if (inputCmp->IsPressed(eKey::KEY_D))
			move += Vector::UNIT_X;

		if (inputCmp->IsPressed(eKey::KEY_Q))
			move -= Vector::UNIT_Y;
		else if (inputCmp->IsPressed(eKey::KEY_E))
			move += Vector::UNIT_Y;

		if (move.LengthSquared() > 0)
			move.Normalize();

		move *= speed * deltaTime;

		trans.SetLocalTranslation(trans.GetLocalTranslation() + trans.GetLocalRotation() * move);

		if (inputCmp->IsPressed(eMouseButton::LEFT))
		{
			Vector2i delta = inputCmp->GetMousePosDelta();

			Quaternion rot = Quaternion(Vector::UNIT_Y, Angle::FromRadians(-delta.X * freeFloatMovementCmp->GetAngularVelocity()));
			rot *= trans.GetLocalRotation();
			rot *= Quaternion(Vector::UNIT_X, Angle::FromRadians(-delta.Y * freeFloatMovementCmp->GetAngularVelocity()));

			if (rot != Quaternion()) {
				rot.Normalize();
				trans.SetLocalRotation(rot);
			}
		}
	}
}

//------------------------------------------------------------------------------
Vector EditorCameraMovementSystem::GetLocalForward(const EntityTransform& transform)
{
	return transform.GetLocalRotation() * -Vector::UNIT_Z;
}

//------------------------------------------------------------------------------
Vector EditorCameraMovementSystem::GetLocalRight(const EntityTransform& transform)
{
	return transform.GetLocalRotation() * Vector::UNIT_X;
}

//------------------------------------------------------------------------------
Vector EditorCameraMovementSystem::GetLocalUp(const EntityTransform& transform)
{
	return transform.GetLocalRotation() * Vector::UNIT_Y;
}

//------------------------------------------------------------------------------
Vector EditorCameraMovementSystem::GetGlobalForward(const EntityTransform& transform)
{
	return transform.GetGlobalRotation() * -Vector::UNIT_Z;
}

//------------------------------------------------------------------------------
Vector EditorCameraMovementSystem::GetGlobalRight(const EntityTransform& transform)
{
	return transform.GetGlobalRotation() * Vector::UNIT_X;
}

//------------------------------------------------------------------------------
Vector EditorCameraMovementSystem::GetGlobalUp(const EntityTransform& transform)
{
	return transform.GetGlobalRotation() * Vector::UNIT_Y;
}
