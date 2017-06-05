#include "ControlSystem.hpp"
#include <DeferredTaskSystem.hpp>

using namespace Poly;

void ControlSystem::ControlSystemPhase(World* world)
{

	GameManagerComponent* gameManager = nullptr;
	double time = world->GetTimeWorldComponent().GetGameplayTime();
	for (auto componentTuple : world->IterateComponents<GameManagerComponent>())
	{
		gameManager = std::get<GameManagerComponent*>(componentTuple);
	}
	for (auto componentTuple : world->IterateComponents<PlayerControllerComponent>())
	{
		PlayerControllerComponent* player = std::get<PlayerControllerComponent*>(componentTuple);
		Vector move(0, 0, 0);
		if (world->GetInputWorldComponent().IsPressed(player->GetLeftKey()))
			move -= Vector::UNIT_X;
		else if (world->GetInputWorldComponent().IsPressed(player->GetRightKey()))
			move += Vector::UNIT_X;


		if (move.Length() > 0)
		{
			move.Normalize();
			move *= player->GetMovementSpeed();
			move *= TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);
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
	for (auto componentTuple : world->IterateComponents<BulletComponent, TransformComponent>())
	{
		BulletComponent* bullet = std::get<BulletComponent*>(componentTuple);
		TransformComponent* transform = std::get<TransformComponent*>(componentTuple);
		if (time < bullet->GetSpawnTme() + bullet->GetLifeTme())
		{
			Vector move = bullet->GetDirection();
			move *= bullet->GetMovementSpeed();
			move *= TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);
			transform->SetLocalTranslation(transform->GetLocalTranslation() + move);
			bullet->GetCollisionBox().SetPosition(transform->GetLocalTranslation() + move);
		}
		else
		{
			if (!gameManager->GetDeadGameEntities()->Contains(bullet->GetOwnerID()))
				gameManager->GetDeadGameEntities()->PushBack(bullet->GetOwnerID());
		}
	}
	for (auto componentTuple : world->IterateComponents<EnemyMovementComponent>())
	{
		EnemyMovementComponent* enemy = std::get<EnemyMovementComponent*>(componentTuple);
		Vector move(1.0f, 0, 0);


		if (move.Length() > 0)
		{
			move.Normalize();
			move *= enemy->GetMovementSpeed();
			move *= TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);
			TransformComponent* enemy_transform = enemy->GetSibling<TransformComponent>();
			Vector prev_location = enemy_transform->GetLocalTranslation();
			enemy_transform->SetLocalTranslation(prev_location + move);
			enemy->GetCollisionBox().SetPosition(enemy_transform->GetLocalTranslation() + move);
		}
		CheckBulletCollisions(world, gameManager, enemy->GetOwnerID());
	}

	if (world->GetInputWorldComponent().IsReleased(gameManager->GetQuitKey()))
	{
		//quit game
	}

	CleanUpEnitites(gameManager, world);
}

void ControlSystem::SpawnBullet(GameManagerComponent* gameManager, World* world, Vector pos, Vector direction, float speed)
{
	auto bullet = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, bullet);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, bullet, "Models/bullet/lowpolybullet.obj");
	if (direction.Length() > 0)
		direction.Normalize();
	DeferredTaskSystem::AddComponentImmediate<BulletComponent>(world, bullet, speed, direction,
		AARect(world->GetComponent<Poly::TransformComponent>(bullet)->GetLocalTranslation(), Vector(2.0f,2.0f,2.0f)), world->GetTimeWorldComponent().GetGameplayTime());
	Poly::TransformComponent* transform = world->GetComponent<Poly::TransformComponent>(bullet);
	transform->SetLocalScale(Vector(0.25f, 0.25f, 0.25f));
	transform->SetLocalRotation(Quaternion(Vector::UNIT_Y, 180_deg));
	transform->SetLocalTranslation(pos);
	gConsole.LogInfo("Spawning Bullet!");
}
void ControlSystem::CleanUpEnitites(GameManagerComponent* gameManager, World* world)
{
	for (Poly::UniqueID ent : *(gameManager->GetDeadGameEntities()))
	{
		DeferredTaskSystem::DestroyEntity(world, ent);
	}
	gameManager->GetDeadGameEntities()->Clear();	
}


void ControlSystem::CheckBulletCollisions(World* world, GameManagerComponent* gameManager, const UniqueID other)
{
	//todo : Collsioncomponent
	if (world->GetComponent<EnemyMovementComponent>(other) != nullptr)
	{
		EnemyMovementComponent* tank = world->GetComponent<EnemyMovementComponent>(other);
		AARect& rect1 = tank->GetCollisionBox();
		bool to_delete = false;
		for (auto componentTuple : world->IterateComponents<BulletComponent>())
		{
			BulletComponent* bullet = std::get<BulletComponent*>(componentTuple);
			if (to_delete)
				break;
			to_delete = CheckCollision(rect1, bullet->GetCollisionBox());
			if (to_delete)
			{
				gConsole.LogInfo("Collision!");
				if (!gameManager->GetDeadGameEntities()->Contains(bullet->GetOwnerID()))
					gameManager->GetDeadGameEntities()->PushBack(bullet->GetOwnerID());
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