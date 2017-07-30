#include "PlayerUpdateSystem.hpp"
#include "World.hpp"
#include "TransformComponent.hpp"
#include "InputWorldComponent.hpp"
#include "PlayerControllerComponent.hpp"
#include "Rigidbody2DComponent.hpp"
#include "PowerupSystem.hpp"
#include "Timer.hpp"
#include "Physics2DWorldComponent.hpp"
#include "GameManagerWorldComponent.hpp"
#include "GameManagerSystem.hpp"

using namespace Poly;

namespace SGJ
{
	void PlayerUpdateSystem::Update(World* world)
	{
		double deltaTime = TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);

		GameManagerWorldComponent* mgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
		PlayerControllerComponent* playerCmp = world->GetComponent<PlayerControllerComponent>(mgrCmp->Player);
		
		// Jumping
		UpdateInAir(world);	
		if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::SPACE))
			TryPlayerJump(world);
		ProcessJumpStrech(world);

		// Moving
		const float modifier = (playerCmp->InAir ? 0.2f : 1.0f);
		if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_A) || world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::LEFT))
			playerCmp->SetMoveVector(Vector(-deltaTime * modifier * playerCmp->GetMovementSpeed(), 0, 0));
		if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_D) || world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::RIGHT))
			playerCmp->SetMoveVector(Vector(deltaTime * modifier * playerCmp->GetMovementSpeed(), 0, 0));
		PowerupSystem::ApplyPowerupsAndInput(world, playerCmp);


		if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::ESCAPE))
			ResetPlayer(world, world->GetComponent<PlayerControllerComponent>(mgrCmp->Player)->SpawnPoint);
	}

	void PlayerUpdateSystem::KillPlayer(Poly::World * world)
	{
		GameManagerWorldComponent* manager = world->GetWorldComponent<GameManagerWorldComponent>();
		TransformComponent* transCmp = world->GetComponent<TransformComponent>(manager->Player);
		GameManagerSystem::PlaySample(world, "Audio/death-sound.ogg", transCmp->GetGlobalTranslation(), 1.0, 1.8);
		ResetPlayer(world, world->GetComponent<PlayerControllerComponent>(manager->Player)->SpawnPoint);
	}

	void PlayerUpdateSystem::ResetPlayer(Poly::World* world, const Vector& spawnLocation)
	{
		GameManagerWorldComponent* manager = world->GetWorldComponent<GameManagerWorldComponent>();
		TransformComponent* transCmp = world->GetComponent<TransformComponent>(manager->Player);
		PlayerControllerComponent* playerCmp = world->GetComponent<PlayerControllerComponent>(manager->Player);
		RigidBody2DComponent* rbCmp = world->GetComponent<RigidBody2DComponent>(manager->Player);

		transCmp->SetLocalTranslation(spawnLocation);
		playerCmp->SpawnPoint = spawnLocation;

		rbCmp->SetLinearSpeed(Vector::ZERO);
		rbCmp->SetRotationSpeed(0.f);

		playerCmp->InAir = false;
		playerCmp->JumpCooldownTimer = 0.f;

		playerCmp->SetActivePowerup(ePowerup(6));

		rbCmp->UpdatePosition();
	}

	void PlayerUpdateSystem::TryPlayerJump(Poly::World* world)
	{
		UniqueID playerID = world->GetWorldComponent<GameManagerWorldComponent>()->Player;

		RigidBody2DComponent* rigidbodyCmp = world->GetComponent<RigidBody2DComponent>(playerID);
		TransformComponent* transCmp = world->GetComponent<TransformComponent>(playerID);
		PlayerControllerComponent* playerCmp = world->GetComponent<PlayerControllerComponent>(playerID);
		
		if (playerCmp->JumpCooldownTimer >= 0 || playerCmp->InAir)
			return;
		
		// we can jump indeed
		double time = TimeSystem::GetTimerElapsedTime(world, Poly::eEngineTimer::GAMEPLAY);
		playerCmp->LastJumpTimeStart = time;
		playerCmp->JumpCooldownTimer = 0.3f;
		playerCmp->InAir = true;

		Vector jump(0, 0, 0);
		jump.Y = playerCmp->GetJumpForce();

		if (playerCmp->GetActivePowerup() == ePowerup::INVERSED_GRAVITY)
			jump.Y = -jump.Y;

		rigidbodyCmp->ApplyImpulseToCenter(jump);
		GameManagerSystem::PlaySample(world, "Audio/jump-sound.ogg", transCmp->GetGlobalTranslation(), 1.5, 1.5);
	}

	void PlayerUpdateSystem::UpdateInAir(Poly::World* world)
	{
		GameManagerWorldComponent* mgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
		PlayerControllerComponent* playerCmp = world->GetComponent<PlayerControllerComponent>(mgrCmp->Player);
		RigidBody2DComponent* rigidbodyCmp = world->GetComponent<RigidBody2DComponent>(mgrCmp->Player);
		float deltaTime = TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);

		if(playerCmp->JumpCooldownTimer >= 0)
			playerCmp->JumpCooldownTimer -= deltaTime;

		if (playerCmp->JumpCooldownTimer > 0)
			return;

		bool wasInAir = playerCmp->InAir;
		playerCmp->InAir = true;
		Physics2DWorldComponent* physicsWorldComponent = world->GetWorldComponent<Physics2DWorldComponent>();

		for (Physics2DWorldComponent::Collision col : physicsWorldComponent->GetCollidingBodies(rigidbodyCmp))
			if (playerCmp->GetActivePowerup() == ePowerup::INVERSED_GRAVITY ? col.Normal.Dot(Vector::UNIT_Y) > 0.7 : col.Normal.Dot(Vector::UNIT_Y) < -0.7)
			{
				if (wasInAir)
					playerCmp->LastLandTimeStart = TimeSystem::GetTimerElapsedTime(world, Poly::eEngineTimer::GAMEPLAY);
				
				playerCmp->InAir = false;
				break;
			}

		if(!playerCmp->InAir)
			rigidbodyCmp->SetDamping(3);
		else
			rigidbodyCmp->SetDamping(0.1);
	}

	float PlayerUpdateSystem::ElasticEaseOut(float p)
	{
		return sin(-13 * 3.14 * (p + 1)) * pow(2, -10 * p) + 1;
	}

	void PlayerUpdateSystem::ProcessJumpStrech(Poly::World * world)
	{
		GameManagerWorldComponent* mgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
		PlayerControllerComponent* playerCmp = world->GetComponent<PlayerControllerComponent>(mgrCmp->Player);
		double time = TimeSystem::GetTimerElapsedTime(world, Poly::eEngineTimer::GAMEPLAY);
		TransformComponent* playerTrans = playerCmp->GetSibling<TransformComponent>();
		float timeSinceLastJump = time - playerCmp->LastJumpTimeStart;
		float timeSinceLastLand = time - playerCmp->LastLandTimeStart;

		if (timeSinceLastLand < timeSinceLastJump)
		{
			// stretch on jump anim
			float tX = 1.0f * timeSinceLastJump;
			float tY = 1.0f * timeSinceLastJump;
			float scaleX = Lerp(2.5f, 1.0f, Clamp(ElasticEaseOut(tX), 0.0f, 1.0f));
			float scaleY = Lerp(0.3f, 1.0f, Clamp(ElasticEaseOut(tY), 0.0f, 1.0f));
			playerTrans->SetLocalScale(playerTrans->GetGlobalRotation().GetConjugated() * Vector(scaleX, scaleY, 1.0f));
		}
		else
		{
			// stretch on jump anim
			float tX = 0.75f * timeSinceLastJump;
			float tY = 0.5f * timeSinceLastJump;
			float scaleX = Lerp(0.3f, 0.5f, Clamp(ElasticEaseOut(tX), 0.0f, 1.0f));
			float scaleY = Lerp(2.5f, 1.2f, Clamp(ElasticEaseOut(tY), 0.0f, 1.0f));
			playerTrans->SetLocalScale(playerTrans->GetGlobalRotation().GetConjugated() * Vector(scaleX, scaleY, 1.0f));
		}
	}

	void PlayerUpdateSystem::PushPlayer(Poly::World* world, const Poly::Vector& normal, float force)
	{
		GameManagerWorldComponent* manager = world->GetWorldComponent<GameManagerWorldComponent>();
		RigidBody2DComponent* rbCmp = world->GetComponent<RigidBody2DComponent>(manager->Player);
		rbCmp->ApplyImpulseToCenter(normal * force);
	}
}
