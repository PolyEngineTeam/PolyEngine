#pragma once

#include <Core.hpp>
#include <bitset>

#include "Engine.hpp"

namespace Poly
{
	class ComponentBase;
	constexpr unsigned int MAX_COMPONENTS_COUNT = 64;

	//TODO finish implementation of this class

	class ENGINE_DLLEXPORT Entity : public BaseObject<>
	{
	public:
		const UniqueID& GetID() const { HEAVY_ASSERTE(EntityID, "Entity was not properly initialized");  return EntityID; }
		const World* GetWorld() const { HEAVY_ASSERTE(EntityID, "Entity was not properly initialized");  return EntityWorld; }

		
		/// Note: This function should only be accessed by engine's internal logic.
		/// Checks whether there is a component of a given ID under this Entity's ID.
		/// @param ID of a component type
		/// @see GetComponent()
		/// @see HasComponents()
		
		bool HasComponent(size_t ID) const;

		
		/// Note: This function should only be accessed by engine's internal logic.
		/// Checks whether there are all of the specified components under this Entity's ID.
		/// @param IDs is a bit field of component IDs to be checked for, see macro BIT()
		/// @see GetComponent()
		/// @see HasComponent()
		
		inline bool HasComponents(unsigned long int IDs) const
		{
			//TODO This bit field has to be reimplemented - it will not work for MAX_COMPONENTS_COUNT greater than 64 in its current state.
			STATIC_ASSERTE(MAX_COMPONENTS_COUNT <= 64, "MAX_COMPONENTS_COUNT greater than 64.");
			return (ComponentPosessionFlags.to_ullong() & IDs) == IDs;
		}

		
		/// Note: This function should only be accessed by engine's internal logic.
		/// Gets a pointer to a component of a given ID.
		/// @return A pointer to a component or nullptr if it does not exist.
		
		template<class T>
		T* GetComponent(); //defined in World.hpp due to circular inclusion problem; FIXME: circular inclusion

	private:
		Entity(const World* world);

		UniqueID EntityID;
		const World* EntityWorld = nullptr;

		std::bitset<MAX_COMPONENTS_COUNT> ComponentPosessionFlags;
		ComponentBase* Components[MAX_COMPONENTS_COUNT];

		friend class World;
	};
} //namespace Poly
