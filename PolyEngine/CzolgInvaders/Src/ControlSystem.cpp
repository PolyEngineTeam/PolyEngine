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


	if (world->GetInputWorldComponent().IsReleased(gameManager->GetQuitKey()))
	{
		//quit game
	}

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
				bullet->GetCollisionBox().SetPosition(transform->GetLocalTranslation() + move);
			}
			else
			{
				if (!gameManager->GetDeadGameEntities()->Contains(ent))
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
		else if (world->GetComponent<EnemyMovementComponent>(ent) != nullptr)
		{
			EnemyMovementComponent* enemy = world->GetComponent<EnemyMovementComponent>(ent);
			Vector move(1.0f, 0, 0);


			if (move.Length() > 0)
			{
				move.Normalize();
				move *= enemy->GetMovementSpeed();
				move *= 0.016f;
				TransformComponent* enemy_transform = enemy->GetSibling<TransformComponent>();
				Vector prev_location = enemy_transform->GetLocalTranslation();
				enemy_transform->SetLocalTranslation(prev_location + move);
				enemy->GetCollisionBox().SetPosition(enemy_transform->GetLocalTranslation() + move);
			}
			CheckBulletCollisions(world, gameManager, ent);
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
	world->AddComponent<BulletComponent>(bullet, speed, direction, 
		AARect(world->GetComponent<Poly::TransformComponent>(bullet)->GetLocalTranslation(), Vector(2.0f,2.0f,2.0f)), world->GetTimeWorldComponent().GetGameplayTime());
	Poly::TransformComponent* transform = world->GetComponent<Poly::TransformComponent>(bullet);
	transform->SetLocalScale(Vector(0.25f, 0.25f, 0.25f));
	transform->SetLocalTranslation(pos);
	gConsole.LogInfo("Spawning Bullet!", std::to_string(bullet.GetHash()));
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

void ControlSystem::CheckBulletCollisions(World* world, GameManagerComponent* gameManager, const UniqueID other)
{
	//todo : Collsioncomponent
	if (world->GetComponent<EnemyMovementComponent>(other) != nullptr)
	{
		EnemyMovementComponent* tank = world->GetComponent<EnemyMovementComponent>(other);
		AARect& rect1 = tank->GetCollisionBox();
		bool to_delete = false;
		Poly::Dynarray<Poly::UniqueID>* gameEntities = gameManager->GetGameEntities();
		for (Poly::UniqueID ent : *gameEntities)
		{
			if (to_delete)
				break;
			if (world->GetComponent<BulletComponent>(ent) != nullptr)
			{
				BulletComponent* bullet = world->GetComponent<BulletComponent>(ent);
				to_delete = CheckCollision(rect1, bullet->GetCollisionBox());
				if (to_delete)
				{
					gConsole.LogInfo("Collision!");
					if (!gameManager->GetDeadGameEntities()->Contains(ent))
						gameManager->GetDeadGameEntities()->PushBack(ent); 
				}
			}
		}
		if (to_delete)
		{
			gameManager->SetKillCount(gameManager->GetKillCount() + 1);
			if (!gameManager->GetDeadGameEntities()->Contains(tank->GetTurret()))
				gameManager->GetDeadGameEntities()->PushBack(tank->GetTurret());
			if (!gameManager->GetDeadGameEntities()->Contains(other))
				gameManager->GetDeadGameEntities()->PushBack(other);
		}
	}
}

bool ControlSystem::CheckCollision(const AARect& rect1, const AARect& rect2)
{
	return(rect1.GetMax().X > rect2.GetMin().X &&
		rect1.GetMin().X < rect2.GetMax().X &&
		rect1.GetMax().Y > rect2.GetMin().Y &&
		rect1.GetMin().Y < rect2.GetMax().Y &&
		rect1.GetMax().Z > rect2.GetMin().Z &&
		rect1.GetMin().Z < rect2.GetMax().Z);
}