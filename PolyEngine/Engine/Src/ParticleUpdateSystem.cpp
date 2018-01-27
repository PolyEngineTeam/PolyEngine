#include "EnginePCH.hpp"

#include "ParticleUpdateSystem.hpp"
#include "ParticleComponent.hpp"

using namespace Poly;

void ParticleUpdateSystem::ParticleUpdatePhase(World* world)
{
	gConsole.LogInfo("ParticleUpdateSystem::ParticleUpdatePhasee");
	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY));

	// Render meshes
	for (auto compTuple : world->IterateComponents<ParticleComponent>())
	{
		ParticleComponent* particle = std::get<ParticleComponent*>(compTuple);
		particle->emitter->Update();
		particle->emitter->UpdateDeviceProxy();
	}

	// for (auto freeFloatTuple : world->IterateComponents<FreeFloatMovementComponent>())
	// {
	// 	FreeFloatMovementComponent* freeFloatMovementCmp = std::get<FreeFloatMovementComponent*>(freeFloatTuple);
	// 	EntityTransform& trans = freeFloatMovementCmp->GetTransform();
	// 
	// 	int wheelDelta = inputCmp->GetWheelPosDelta().Y;
	// 	float speed = freeFloatMovementCmp->GetMovementSpeed();
	// 	speed = Clamp(speed + wheelDelta, 0.001f, 10000.0f);
	// 	freeFloatMovementCmp->SetMovementSpeed(speed);
	// 
	// 	Vector move;
	// 	if (inputCmp->IsPressed(eKey::KEY_W))
	// 		move -= Vector::UNIT_Z;
	// 	else if (inputCmp->IsPressed(eKey::KEY_S))
	// 		move += Vector::UNIT_Z;
	// 
	// 	if (inputCmp->IsPressed(eKey::KEY_A))
	// 		move -= Vector::UNIT_X;
	// 	else if (inputCmp->IsPressed(eKey::KEY_D))
	// 		move += Vector::UNIT_X;
	// 
	// 	if (inputCmp->IsPressed(eKey::KEY_Q))
	// 		move -= Vector::UNIT_Y;
	// 	else if (inputCmp->IsPressed(eKey::KEY_E))
	// 		move += Vector::UNIT_Y;
	// 
	// 	if (move.LengthSquared() > 0)
	// 		move.Normalize();
	// 
	// 	move *= speed * deltaTime;
	// 
	// 	trans.SetLocalTranslation(trans.GetLocalTranslation() + trans.GetLocalRotation() * move);
	// 	
	// 	if (inputCmp->IsPressed(eMouseButton::LEFT))
	// 	{
	// 		Vector2i delta = inputCmp->GetMousePosDelta();
	// 
	// 		Quaternion rot = Quaternion(Vector::UNIT_Y, Angle::FromRadians(-delta.X * freeFloatMovementCmp->GetAngularVelocity()));
	// 		rot *= trans.GetLocalRotation();
	// 		rot *= Quaternion(Vector::UNIT_X, Angle::FromRadians(-delta.Y * freeFloatMovementCmp->GetAngularVelocity()));
	// 
	// 		if (rot != Quaternion()) {
	// 			rot.Normalize();
	// 			trans.SetLocalRotation(rot);
	// 		}
	// 	}
	// }
}