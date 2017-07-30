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
		double time = TimeSystem::GetTimerElapsedTime(world, Poly::eEngineTimer::GAMEPLAY);

		for (auto playerTuple : world->IterateComponents<PlayerControllerComponent, RigidBody2DComponent>())
		{
			RigidBody2DComponent* rigidbodyCmp = std::get<RigidBody2DComponent*>(playerTuple);
			PlayerControllerComponent* playerCmp = std::get<PlayerControllerComponent*>(playerTuple);

			Vector move(0, 0, 0);
			if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_A) || world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::LEFT))
				playerCmp->SetMoveVector(Vector(-deltaTime * playerCmp->GetMovementSpeed(), 0, 0));

			if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_D) || world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::RIGHT))				
				playerCmp->SetMoveVector(Vector(deltaTime * playerCmp->GetMovementSpeed(), 0, 0));

			if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::SPACE) && playerCmp->AllowJump)
			{
				PlayerJump(world);
			}

			try
			{
				Physics2DWorldComponent* physicsWorldComponent = world->GetWorldComponent<Physics2DWorldComponent>();
				for (Physics2DWorldComponent::Collision col : physicsWorldComponent->GetCollidingBodies(rigidbodyCmp))
					if (col.Normal.Dot(Vector::UNIT_Y) < -0.7)
					{
						if (!playerCmp->AllowJump) 
						{
							playerCmp->LastLandTimeStart = time;
						}
						playerCmp->LastAllowJumpChecked = 0;
						playerCmp->AllowJump = true;

						break;
					}
			}
			catch (std::exception)
			{

			}
			playerCmp->LastAllowJumpChecked += deltaTime;
			if (playerCmp->LastAllowJumpChecked > 0.5)
				playerCmp->AllowJump = false;



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

			PowerupSystem::ApplyPowerupsAndInput(world, playerCmp);
		}
	}

	float PlayerUpdateSystem::ElasticEaseOut(float p)
	{
		return sin(-13 * 3.14 * (p + 1)) * pow(2, -10 * p) + 1;
	}

	void PlayerUpdateSystem::KillPlayer(Poly::World * world)
	{
		GameManagerWorldComponent* manager = world->GetWorldComponent<GameManagerWorldComponent>();
		world->GetComponent<TransformComponent>(manager->Player)->SetLocalTranslation(world->GetComponent<PlayerControllerComponent>(manager->Player)->SpawnPoint);
	}

	void PlayerUpdateSystem::PlayerJump(Poly::World* world)
	{
		UniqueID playerID = world->GetWorldComponent<GameManagerWorldComponent>()->Player;

		RigidBody2DComponent* rigidbodyCmp = world->GetComponent<RigidBody2DComponent>(playerID);
		TransformComponent* transCmp = world->GetComponent<TransformComponent>(playerID);
		PlayerControllerComponent* playerCmp = world->GetComponent<PlayerControllerComponent>(playerID);

		double time = TimeSystem::GetTimerElapsedTime(world, Poly::eEngineTimer::GAMEPLAY);
		if (playerCmp->AllowJump)
		{
			playerCmp->LastJumpTimeStart = time;
		}
		playerCmp->AllowJump = false;
		Vector jump(0, 0, 0);
		jump.Y = playerCmp->GetJumpForce();
		rigidbodyCmp->ApplyImpulseToCenter(jump);
		GameManagerSystem::PlaySample(world, "Audio/jump-sound.ogg", transCmp->GetGlobalTranslation(), 1.5, 1.8);
	}
}
