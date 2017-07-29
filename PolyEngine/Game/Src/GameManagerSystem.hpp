#pragma once

#include <World.hpp>

namespace SGJ {
	enum class eTileType;
}

namespace Poly
{
	enum class eRigidBody2DType;
}

namespace SGJ
{
	namespace GameManagerSystem
	{
		void GameManagerSystem::Update(Poly::World*);

		Poly::UniqueID CreateGroundObject(Poly::World* world, const Poly::Vector& position, eTileType tileType);
		Poly::UniqueID CreateObstacleObject(Poly::World* world, const Poly::Vector& position, const Poly::Vector& size, eTileType tileType);
		Poly::UniqueID SpawnPlayer(Poly::World* world, const Poly::Vector& position);

		void LoadLevel(Poly::World* world, const Poly::String& path);
		void SpawnLevel(Poly::World* world, size_t idx);
		void DespawnLevel(Poly::World* world);

		void PrepareNonlevelObjects(Poly::World* world);
		void Cleanup(Poly::World* world);
	}
}