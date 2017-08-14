#include "EnginePCH.hpp"

#include "MovementSystem.hpp"

using namespace Poly;

void MovementSystem::MovementUpdatePhase(World* world)
{
	for (auto freeFloatTuple : world->IterateComponents<FreeFloatMovementComponent, TransformComponent>())
	{
		TransformComponent* transCmp = std::get<TransformComponent*>(freeFloatTuple);
		FreeFloatMovementComponent* freeFloatMovementCmp = std::get<FreeFloatMovementComponent*>(freeFloatTuple);

		Vector move;
		if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_W))
			move -= Vector::UNIT_Z;
		else if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_S))
			move += Vector::UNIT_Z;

		if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_A))
			move -= Vector::UNIT_X;
		else if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_D))
			move += Vector::UNIT_X;

		if (move.LengthSquared() > 0)
			move.Normalize();
		move *= freeFloatMovementCmp->GetMovementSpeed() * 0.016f; //TMP dt

		transCmp->SetLocalTranslation(transCmp->GetLocalTranslation() + transCmp->GetLocalRotation() * move);
		
		if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::MLBUTTON))
		{
			Vector delta = world->GetWorldComponent<InputWorldComponent>()->GetMousePosDelta();

			Quaternion rot = Quaternion(Vector::UNIT_Y, Angle::FromRadians(-delta.X * freeFloatMovementCmp->GetAngularVelocity()));
			rot *= transCmp->GetLocalRotation();
			rot *= Quaternion(Vector::UNIT_X, Angle::FromRadians(-delta.Y * freeFloatMovementCmp->GetAngularVelocity()));

			if (rot != Quaternion()) {
				rot.Normalize();
				transCmp->SetLocalRotation(rot);
			}
		}
	}
}

Vector MovementSystem::GetLocalForward(const TransformComponent* transform)
{
	return transform->GetLocalRotation() * -Vector::UNIT_Z;
}

Vector MovementSystem::GetLocalRight(const TransformComponent* transform)
{
	return transform->GetLocalRotation() * Vector::UNIT_X;
}

Vector MovementSystem::GetLocalUp(const TransformComponent* transform)
{
	return transform->GetLocalRotation() * Vector::UNIT_Y;
}

Vector MovementSystem::GetGlobalForward(const TransformComponent* transform)
{
	return transform->GetGlobalRotation() * -Vector::UNIT_Z;
}

Vector MovementSystem::GetGlobalRight(const TransformComponent* transform)
{
	return transform->GetGlobalRotation() * Vector::UNIT_X;
}

Vector MovementSystem::GetGlobalUp(const TransformComponent* transform)
{
	return transform->GetGlobalRotation() * Vector::UNIT_Y;
}
