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
		void Update(Poly::World*);

		Poly::UniqueID CreateTileObject(Poly::World* world, const Poly::Vector& position, eTileType tileType, Poly::eRigidBody2DType physicsPropertie, const Poly::Vector& size, const Poly::Color& color);
		Poly::UniqueID SpawnPlayer(Poly::World* world, const Poly::Vector& position);

		void KillPlayer(Poly::World* world);

		void LoadLevel(Poly::World* world, const Poly::String& path);
		void SpawnLevel(Poly::World* world, size_t idx);
		void DespawnLevel(Poly::World* world);

		void PrepareNonlevelObjects(Poly::World* world);
		void Cleanup(Poly::World* world);
	}
}