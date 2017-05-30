#pragma once

#include <World.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <Timer.hpp>
#include "PlayerControllerComponent.hpp"
#include "BulletComponent.hpp"
#include "EnemyMovementComponent.hpp"
#include "GameManagerComponent.hpp"

namespace Poly
{
	class World;


	namespace ControlSystem
	{

		void ControlSystemPhase(World*);

		void SpawnBullet(GameManagerComponent* gameManager, World* world, Vector pos, Vector direction, float speed);
		void CleanUpEnitites(GameManagerComponent* gameManager, World* world);
		void SpawnEnitites(GameManagerComponent* gameManager, World* world);
	}
}
	
