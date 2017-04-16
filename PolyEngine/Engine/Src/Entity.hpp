#pragma once

#include <Core.hpp>
#include <bitset>

#include "ComponentBase.hpp"

namespace Poly 
{

	class World;

	//TODO finish implementation of this class

	class ENGINE_DLLEXPORT Entity : public BaseObject<>
	{
	public:
		const UniqueID& GetID() const { HEAVY_ASSERTE(EntityID, "Entity was not properly initialized");  return EntityID; }
		const World* GetWorld() const { HEAVY_ASSERTE(EntityID, "Entity was not properly initialized");  return EntityWorld; }

		//////////////////////////////
		/// Note: This function should only be accessed by engine's internal logic.
		/// Checks whether there is a component of a given ID under this Entity's ID.
		/// @param ID of a component type
		/// @see GetComponent()
		/// @see HasComponents()
		//////////////////////////////
		bool HasComponent(size_t ID) const;
		
		//////////////////////////////
		/// Note: This function should only be accessed by engine's internal logic.
		/// Checks whether there are all of the specified components under this Entity's ID.
		/// @param IDs is a bit field of component IDs to be checked for, see macro BIT()
		/// @see GetComponent()
		/// @see HasComponent()
		//////////////////////////////
		inline bool HasComponents(unsigned long int IDs) const
		{
			return ComponentPosessionFlags.to_ullong() & IDs;
		}

		//////////////////////////////
		/// Note: This function should only be accessed by engine's internal logic.
		/// Gets a pointer to a component of a given ID.
		/// @return A pointer to a component or nullptr if it does not exist.
		//////////////////////////////
		template<class T>
		T* GetComponent()
		{
			if (HasComponent(GET_COMPONENT_ID(T)))
				return static_cast<T*>(Components[GET_COMPONENT_ID(T)]);
			else
				return nullptr;
		}

	private:
		Entity(const World* world);

		UniqueID EntityID;
		const World* EntityWorld = nullptr;

		std::bitset<MAX_COMPONENTS_COUNT> ComponentPosessionFlags;
		ComponentBase* Components[MAX_COMPONENTS_COUNT];

		friend class World;
	};
}