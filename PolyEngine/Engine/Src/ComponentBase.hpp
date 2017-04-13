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

	private:
		Entity* Owner = nullptr;

		friend class World;
	};
}

#define DEFINE_COMPONENT(type, id) template<> struct Poly::Impl::ComponentIDByType<type> { constexpr static size_t ID = id; }; \
								   template<> struct Poly::Impl::ComponentTypeByID<id> { using Type = type; }

#define GET_COMPONENT_ID(type) Poly::Impl::ComponentIDByType<type>::ID
#define GET_COMPONENT_TYPE(id) typename Poly::Impl::ComponentTypeByID<id>::Type