#include "ControlSystem.hpp"

using namespace Poly;

void ControlSystem::ControlSystemPhase(World* world)
{
	GameManagerComponent* gameManager = nullptr;
	for (auto gameManagerTuple : world->IterateComponents<GameManagerComponent>())
	{
		gameManager = std::get<GameManagerComponent*>(gameManagerTuple);
	}
	Poly::Dynarray<Poly::UniqueID>* gameEntities = gameManager->GetGameEntities();
	double time = world->GetTimeWorldComponent().GetGameplayTime();

	for (Poly::UniqueID ent : *gameEntities)
	{
		if (world->GetComponent<BulletComponent>(ent) != nullptr)
		{
			BulletComponent* bullet = world->GetComponent<BulletComponent>(ent);
			if (time < bullet->GetSpawnTme() + bullet->GetLifeTme())
			{
				TransformComponent* transform = bullet->GetSibling<TransformComponent>();
				Vector move = bullet->GetDirection();
				move *= bullet->GetMovementSpeed();
				transform->SetLocalTranslation(transform->GetLocalTranslation() + move);
			}
			else
			{
				gameManager->GetDeadGameEntities()->PushBack(ent);
			}
		}
		else if (world->GetComponent<PlayerControllerComponent>(ent) != nullptr)
		{
			PlayerControllerComponent* player = world->GetComponent<PlayerControllerComponent>(ent);
			Vector move(0, 0, 0);
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
			}
			if (world->GetInputWorldComponent().IsPressed(player->GetShootKey()) && (world->GetTimeWorldComponent().GetGameplayTime() - player->GetLastShoot() >= player->GetShootInterval()))
			{
				SpawnBullet(gameManager, world, player->GetSibling<TransformComponent>()->GetLocalTranslation(), Vector(0.0f, 0.0f, -1.0f), player->GetBulletSpeed());
				player->SetLastShoot(world->GetTimeWorldComponent().GetGameplayTime());
			}
		}			
	}
	for (auto enemyMovementTuple : world->IterateComponents<EnemyMovementComponent>())
	{
		EnemyMovementComponent* enemy = std::get<EnemyMovementComponent*>(enemyMovementTuple);
		Vector move(0, 0, 1.0f);


		if (move.Length() > 0)
		{
			move.Normalize();
			move *= enemy->GetMovementSpeed();
			move *= 0.016f;
			TransformComponent* enemy_transform = enemy->GetSibling<TransformComponent>();
			Vector prev_location = enemy_transform->GetLocalTranslation();
			enemy_transform->SetLocalTranslation(prev_location + move);
		}
	}
	CleanUpEnitites(gameManager, world);
	SpawnEnitites(gameManager, world);
}

void ControlSystem::SpawnBullet(GameManagerComponent* gameManager, World* world, Vector pos, Vector direction, float speed)
{
	auto bullet = world->SpawnEntity();
	world->AddComponent<Poly::TransformComponent>(bullet);
	world->AddComponent<Poly::MeshRenderingComponent>(bullet, "model-tank/tank.fbx");
	if (direction.Length() > 0)
		direction.Normalize();
	world->AddComponent<BulletComponent>(bullet, speed, direction, world->GetTimeWorldComponent().GetGameplayTime());
	Poly::TransformComponent* transform = world->GetComponent<Poly::TransformComponent>(bullet);
	transform->SetLocalScale(Vector(0.25f, 0.25f, 0.25f));
	transform->SetLocalTranslation(pos);
	gameManager->GetSpawnGameEntities()->PushBack(bullet);
}
void ControlSystem::CleanUpEnitites(GameManagerComponent* gameManager, World* world)
{
	for (Poly::UniqueID ent : *(gameManager->GetDeadGameEntities()))
	{
		world->DestroyEntity(ent);
		gameManager->GetGameEntities()->Remove(ent);
	}
	gameManager->GetDeadGameEntities()->Clear();	
}
void ControlSystem::SpawnEnitites(GameManagerComponent* gameManager, World* world)
{
	for (Poly::UniqueID ent : *(gameManager->GetSpawnGameEntities()))
	{
		gameManager->GetGameEntities()->PushBack(ent);
	}
	gameManager->GetSpawnGameEntities()->Clear();

}
