#pragma once

#include <Core.hpp>
#include "Entity.hpp"

namespace Poly {
	class ENGINE_DLLEXPORT ComponentBase : public BaseObject<>
	{
	public:
		//////////////////////////////
		/// Getter for a component of a specified type that shares UniqueID with this one.
		/// @tparam T is a type of a component to get.
		/// @return pointer to a component of a specified type or a nullptr, if it does not exist.
		//////////////////////////////
		template<typename T>
		T* GetSibling()
		{
			HEAVY_ASSERTE(Owner, "Component was not properly initialized.");
			return Owner->GetComponent<T>();
		}

	private:
		Entity* Owner = nullptr;

		friend class World;
	};
}
