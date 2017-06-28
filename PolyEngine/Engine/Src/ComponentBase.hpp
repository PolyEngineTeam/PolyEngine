#pragma once

#include <Core.hpp>
#include "Entity.hpp"

namespace Poly {

	enum class eComponentBaseFlags
	{
		NONE = 0x00,
		NEWLY_CREATED = 0x01,
		ABOUT_TO_BE_REMOVED = 0x02
	};

	/// <summary>Base type for every component type</summary>
	class ENGINE_DLLEXPORT ComponentBase : public BaseObject<>
	{
	friend class World;
	public:
		
		/// <summary>Getter for a component of a specified type that shares UniqueID with this one.</summary>
		/// <returns>Pointer to a component of a specified type or a nullptr, if it does not exist.</returns>
		template<typename T>
		T* GetSibling()
		{
			HEAVY_ASSERTE(Owner, "Component was not properly initialized.");
			return Owner->GetComponent<T>();
		}

		UniqueID GetOwnerID() const
		{
			HEAVY_ASSERTE(Owner, "Component was not properly initialized.");
			return Owner->GetID();
		}

		void SetFlags(const EnumFlags<eComponentBaseFlags>& rhs) { Flags |= rhs; }
		void ResetFlags(const EnumFlags<eComponentBaseFlags>& rhs) { Flags &= ~rhs; }
		const EnumFlags<eComponentBaseFlags>& GetFlags() { return Flags; }
		bool CheckFlags(const EnumFlags<eComponentBaseFlags>& rhs) const { return (Flags & rhs) == rhs; }

	private:
		Entity* Owner = nullptr;

		EnumFlags<eComponentBaseFlags> Flags;
	};
}
