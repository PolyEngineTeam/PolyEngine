#include "MovementSystem.hpp"

#include <Vector.hpp>
#include <Quaternion.hpp>
#include <UniqueID.hpp>
#include <World.hpp>
#include <TransformComponent.hpp>
#include <TimeSystem.hpp>

#include "MovementComponent.hpp"

void Invaders::MovementSystem::MovementUpdatePhase(Poly::World* world)
{
	float dt = Poly::TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);
	MovementComponent* movement;
	Poly::TransformComponent* transform;

	for (auto tuple : world->IterateComponents<MovementComponent, Poly::TransformComponent>())
	{
		movement = std::get<MovementComponent*>(tuple);
		transform = std::get<Poly::TransformComponent*>(tuple);

		movement->LinearVelocity += movement->LinearAcceleration;
		movement->AngularVelocity *= movement->AngularAcceleration;

		transform->SetLocalTranslation(transform->GetLocalTranslation() + movement->LinearVelocity * dt);
		// TODO: movement->AngularVelocity * dt
		transform->SetLocalRotation(transform->GetLocalRotation() * movement->AngularVelocity);
	}
}

void Invaders::MovementSystem::SetLinearVelocity(Poly::World* world, const Poly::UniqueID& id, const Poly::Vector& vel)
{
	world->GetComponent<MovementComponent>(id)->LinearVelocity = vel;
}

const Poly::Vector& Invaders::MovementSystem::GetLinearVelocity(Poly::World* world, Poly::UniqueID id)
{
	return world->GetComponent<MovementComponent>(id)->LinearVelocity;
}

void Invaders::MovementSystem::SetLinearAcceleration(Poly::World* world, const Poly::UniqueID& id, const Poly::Vector& acc)
{
	world->GetComponent<MovementComponent>(id)->LinearAcceleration = acc;
}

const Poly::Vector& Invaders::MovementSystem::GetLinearAcceleration(Poly::World* world, Poly::UniqueID id)
{
	return world->GetComponent<MovementComponent>(id)->LinearAcceleration;
}

void Invaders::MovementSystem::SetAngularVelocity(Poly::World* world, const Poly::UniqueID& id, const Poly::Quaternion& rot)
{
	world->GetComponent<MovementComponent>(id)->AngularVelocity = rot;
}

const Poly::Quaternion& Invaders::MovementSystem::GetAngularVelocity(Poly::World* world, Poly::UniqueID id)
{
	return world->GetComponent<MovementComponent>(id)->AngularVelocity;
}

void Invaders::MovementSystem::SetAngularAcceleration(Poly::World* world, const Poly::UniqueID& id, const Poly::Quaternion& acc)
{
	world->GetComponent<MovementComponent>(id)->AngularAcceleration = acc;
}

const Poly::Quaternion& Invaders::MovementSystem::GetAngularAcceleration(Poly::World* world, Poly::UniqueID id)
{
	return world->GetComponent<MovementComponent>(id)->AngularAcceleration;
}
