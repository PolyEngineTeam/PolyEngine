#pragma once

#include <Core.hpp>

namespace Poly {

	namespace Impl
	{
		template<typename T>
		struct ComponentIDByType
		{
		};

		template<typename ID>
		struct ComponentTypeByID
		{
		};
	}

	class Entity;
	constexpr unsigned int MAX_COMPONENTS_COUNT = 64;

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

#define DEFINE_COMPONENT(type, id) template<> struct Poly::Impl::ComponentIDByType<type> { constexpr static size_t ID = id; }; \
								   template<> struct Poly::Impl::ComponentTypeByID<id> { using Type = type; }

#define GET_COMPONENT_ID(type) Poly::Impl::ComponentIDByType<type>::ID
#define GET_COMPONENT_TYPE(id) typename Poly::Impl::ComponentTypeByID<id>::Type