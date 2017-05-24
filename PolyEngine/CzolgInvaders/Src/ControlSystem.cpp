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
			if (Abs(prev_location.X + move.X) <= player->GetMaxAbsXPosition())
				player_transform->SetLocalTranslation(prev_location + move);
			/*if (prev_location.X > player->GetMaxAbsXPosition())
				player_transform->SetLocalTranslation(Vector(player->GetMaxAbsXPosition(), prev_location.Y, prev_location.Z));
			else if (prev_location.X < -(player->GetMaxAbsXPosition()))
				player_transform->SetLocalTranslation(Vector(-(player->GetMaxAbsXPosition()), prev_location.Y, prev_location.Z));*/
		}

		if (world->GetInputWorldComponent().IsPressed(player->GetShootKey()) && (world->GetTimeWorldComponent().GetGameplayTime() - player->GetLastShoot() >= player->GetShootInterval()))
		{
			SpawnBullet(world, player->GetSibling<TransformComponent>()->GetLocalTranslation(), Vector(0.0f, 0.0f, -1.0f), player->GetBulletSpeed());
			player->SetLastShoot(world->GetTimeWorldComponent().GetGameplayTime());
		}
			
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

