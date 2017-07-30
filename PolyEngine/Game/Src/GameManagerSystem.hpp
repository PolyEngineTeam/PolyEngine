#pragma once

#include <World.hpp>
#include <String.hpp>
#include <Vector.hpp>

namespace SGJ {
	enum class eTileType;
}

namespace Poly
{
	enum class eRigidBody2DType;
	class String;
}

namespace SGJ
{
	namespace GameManagerSystem
	{
		void Update(Poly::World*);

		Poly::UniqueID CreateTileObject(Poly::World* world, const Poly::Vector& position, eTileType tileType, Poly::String meshSource, Poly::eRigidBody2DType physicsPropertie, const Poly::Vector& size, const Poly::Color& color, bool colliding);
		Poly::UniqueID SpawnPlayer(Poly::World* world, const Poly::Vector& position);

		void LoadLevel(Poly::World* world, const Poly::String& path);
		void SpawnLevel(Poly::World* world, size_t idx);
		void DespawnLevel(Poly::World* world);

		void PlaySample(Poly::World * world, const Poly::String& file, const Poly::Vector& position, float pitch = 1.0f, float gain = 1.0f);

		void PrepareNonlevelObjects(Poly::World* world);
		void Cleanup(Poly::World* world);
	}
}