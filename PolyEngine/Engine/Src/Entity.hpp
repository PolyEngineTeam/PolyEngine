#pragma once

#include <Core.hpp>
#include <bitset>

#include "Engine.hpp"

namespace Poly
{
	class ComponentBase;
	constexpr unsigned int MAX_COMPONENTS_COUNT = 64;

	/// <summary>Class that represent entity inside core engine systems. Should not be used anywhere else.</summary>
	class ENGINE_DLLEXPORT Entity : public BaseObject<>
	{
	public:
		const UniqueID& GetID() const { HEAVY_ASSERTE(EntityID, "Entity was not properly initialized");  return EntityID; }
		const World* GetWorld() const { HEAVY_ASSERTE(EntityID, "Entity was not properly initialized");  return EntityWorld; }

		/// <summary>Checks whether there is a component of a given ID under this Entity's ID.</summary>
		/// <param name="ID">ID of a component type</param>
		/// <returns>True if has queried component, false otherwise.</summary>
		/// <seealso cref="Entity.GetComponent()"/>
		/// <seealso cref="Entity.HasComponents()"/>
		bool HasComponent(size_t ID) const;

		/// <summary>Checks whether there are all of the specified components under this Entity's ID.</summary>
		/// <param name="IDs">IDs is a bit field of component IDs to be checked for, see macro BIT()</param> 
		/// <returns>True if has queried components, false otherwise.</summary>
		/// <seealso cref="Entity.GetComponent()"/>
		/// <seealso cref="Entity.HasComponent()"/>
		inline bool HasComponents(unsigned long int IDs) const
		{
			//TODO This bit field has to be reimplemented - it will not work for MAX_COMPONENTS_COUNT greater than 64 in its current state.
			STATIC_ASSERTE(MAX_COMPONENTS_COUNT <= 64, "MAX_COMPONENTS_COUNT greater than 64.");
			return (ComponentPosessionFlags.to_ullong() & IDs) == IDs;
		}

		/// <summary>Gets a pointer to a component of a given ID.</summary>
		/// <returns>A pointer to a component or nullptr if it does not exist.</returns>
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
