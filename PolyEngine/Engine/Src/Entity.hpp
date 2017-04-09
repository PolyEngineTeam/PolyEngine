#pragma once

#include <Core.hpp>

#include "ComponentBase.hpp"

namespace Poly {

	class World;

	inline unsigned long GetUniqueEntityID() { static unsigned long counter = 0; return ++counter; }

	class ENGINE_DLLEXPORT Entity : public BaseObject<>
	{
	public:
		unsigned long GetID() const { HEAVY_ASSERTE(EntityID != 0, "Entity was not properly initialized");  return EntityID; }
		const World* GetWorld() const { HEAVY_ASSERTE(EntityID != 0, "Entity was not properly initialized");  return EntityWorld; }

	private:
		Entity(const World* world);

		unsigned long EntityID = 0;
		const World* EntityWorld = nullptr;

		ComponentBase* Components[MAX_COMPONENTS_COUNT];

		friend class World;
	};
}