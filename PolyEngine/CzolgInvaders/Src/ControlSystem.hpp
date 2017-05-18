#pragma once

#include <World.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include "PlayerControllerComponent.hpp"
#include "BulletMovementComponent.hpp"

namespace Poly
{
	class World;

	namespace ControlSystem
	{
		void ControlSystemPhase(World*);

		void SpawnBullet(World* world, Vector pos, Vector direction, float speed);
	}
}
	
