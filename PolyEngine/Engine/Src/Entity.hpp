#pragma once

#include <Core.hpp>

#include "ComponentBase.hpp"

namespace Poly {

	class World;

	//TODO finish implementation of this class

	class ENGINE_DLLEXPORT Entity : public BaseObject<>
	{
	public:
		const UniqueID& GetID() const { HEAVY_ASSERTE(EntityID, "Entity was not properly initialized");  return EntityID; }
		const World* GetWorld() const { HEAVY_ASSERTE(EntityID, "Entity was not properly initialized");  return EntityWorld; }

	private:
		Entity(const World* world);

		UniqueID EntityID;
		const World* EntityWorld = nullptr;

		ComponentBase* Components[MAX_COMPONENTS_COUNT];

		friend class World;
	};
}