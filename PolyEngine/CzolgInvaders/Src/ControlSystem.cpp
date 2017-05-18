#include "ControlSystem.hpp"

using namespace Poly;

void ControlSystem::ControlSystemPhase(World* world)
{
	for (auto playerControllerTuple : world->IterateComponents<PlayerControllerComponent>())
	{
		PlayerControllerComponent* player = std::get<PlayerControllerComponent*>(playerControllerTuple);
		Vector move(0,0,0);
		if (world->GetInputWorldComponent().IsPressed(player->GetLeftKey()))
			move -= Vector::UNIT_X;
		else if (world->GetInputWorldComponent().IsPressed(player->GetRightKey()))
			move += Vector::UNIT_X;


		if (move.Length() > 0)
		{
			move.Normalize();
			move *= player->GetMovementSpeed();
			move *= 0.016f;
			TransformComponent* player_transform = player->GetSibling<TransformComponent>();
			Vector prev_location = player_transform->GetLocalTranslation();
			player_transform->SetLocalTranslation(prev_location + move);
		}

		if (world->GetInputWorldComponent().IsPressed(player->GetShootKey()))
			SpawnBullet(world, player->GetSibling<TransformComponent>()->GetLocalTranslation(), Vector(0.0f, 0.0f, -1.0f),player->GetBulletSpeed());
	}
	for (auto bulletMovementTuple : world->IterateComponents<BulletMovementComponent>())
	{
		BulletMovementComponent* bullet = std::get<BulletMovementComponent*>(bulletMovementTuple);
		TransformComponent* transform = bullet->GetSibling<TransformComponent>();
		Vector move = bullet->GetDirection();
		move *= bullet->GetMovementSpeed();
		transform->SetLocalTranslation(transform->GetLocalTranslation() + move);

		
	}
}

void ControlSystem::SpawnBullet(World* world, Vector pos, Vector direction, float speed)
{
	auto bullet = world->SpawnEntity();
	world->AddComponent<Poly::TransformComponent>(bullet);
	world->AddComponent<Poly::MeshRenderingComponent>(bullet, "model-tank/tank.fbx");
	if (direction.Length() > 0)
		direction.Normalize();
	world->AddComponent<BulletMovementComponent>(bullet, speed, direction);
	Poly::TransformComponent* transform = world->GetComponent<Poly::TransformComponent>(bullet);
	transform->SetLocalScale(Vector(0.25f, 0.25f, 0.25f));
	transform->SetLocalTranslation(pos);
	//transform->SetLocalRotation(Quaternion(direction));
}

