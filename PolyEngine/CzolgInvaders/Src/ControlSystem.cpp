#include "ControlSystem.hpp"
#include <DeferredTaskSystem.hpp>
#include <TimeWorldComponent.hpp>
#include <InputWorldComponent.hpp>

#include "MovementComponent.hpp"
#include "CollisionComponent.hpp"
#include "TankComponent.hpp"

using namespace Poly;

void ControlSystem::ControlSystemPhase(World* world)
{

	GameManagerComponent* gameManager = nullptr;
	double time = world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime();
	for (auto componentTuple : world->IterateComponents<GameManagerComponent>())
	{
		gameManager = std::get<GameManagerComponent*>(componentTuple);
	}
	for (auto componentTuple : world->IterateComponents<PlayerControllerComponent>())
	{
		PlayerControllerComponent* player = std::get<PlayerControllerComponent*>(componentTuple);
		Vector move(0, 0, 0);
		if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(player->GetLeftKey()))
			move -= Vector::UNIT_X;
		else if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(player->GetRightKey()))
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
		if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(player->GetShootKey()) && (world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime() - player->GetLastShoot() >= player->GetShootInterval()))
		{
			SpawnBullet(gameManager, world, player->GetSibling<TransformComponent>()->GetLocalTranslation(), Vector(0.0f, 0.0f, -1.0f), player->GetBulletSpeed());
			player->SetLastShoot(world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());
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

	Invaders::CollisionSystem::CollisionComponent* collider;
	
	for (auto tuple : world->IterateComponents<Invaders::CollisionSystem::CollisionComponent>())
	{
		collider = std::get<Invaders::CollisionSystem::CollisionComponent*>(tuple);
		if (collider->IsColliding())
		{
			gameManager->SetKillCount(gameManager->GetKillCount() + 1);
			DeferredTaskSystem::DestroyEntity(world, collider->GetOwnerID());
		}
	}

	if (world->GetWorldComponent<InputWorldComponent>()->IsReleased(gameManager->GetQuitKey()))
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
	DeferredTaskSystem::AddComponentImmediate<Invaders::CollisionSystem::CollisionComponent>(world, bullet,  Vector(0, 0, 0), Vector(2.0f,2.0f,2.0f));

	if (direction.Length() > 0)
		direction.Normalize();
	DeferredTaskSystem::AddComponentImmediate<BulletComponent>(world, bullet, speed, direction,
		AABox(world->GetComponent<Poly::TransformComponent>(bullet)->GetLocalTranslation(), Vector(2.0f,2.0f,2.0f)), world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());
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