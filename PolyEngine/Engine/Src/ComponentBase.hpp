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

#define DEFINE_COMPONENT(type, id) template<> struct Impl::ComponentIDByType<type> { constexpr static size_t ID = id; }; \
								   template<> struct Impl::ComponentTypeByID<id> { using Type = type; }

#define GET_COMPONENT_ID(type) Impl::ComponentIDByType<type>::ID
#define GET_COMPONENT_TYPE(id) typename Impl::ComponentTypeByID<id>::Type

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