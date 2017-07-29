#pragma once

#include <World.hpp>

namespace Poly
{
	enum class eRigidBody2DType;
}

namespace SGJ
{
	namespace GameManagerSystem
	{
		Poly::UniqueID CreateTileObject(Poly::World* world, const Poly::Vector& position, const Poly::Vector& size, const Poly::Color& color, Poly::eRigidBody2DType type);


		void LoadLevel(Poly::World* world, const Poly::String& path);
		void SpawnLevel(Poly::World* world, size_t idx);
		void DespawnLevel(Poly::World* world);

		void PrepareNonlevelObjects(Poly::World* world);
		void Cleanup(Poly::World* world);
	}
}