#pragma once

#include <Core.hpp>

namespace Poly {

	class Entity;

	constexpr unsigned int MAX_COMPONENTS_COUNT = 64;
	constexpr unsigned int MAX_ENTITY_COUNT = 1024;

	class ENGINE_DLLEXPORT ComponentBase : public BaseObject<>
	{
	public:

	private:
		Entity* Owner = nullptr;
	};


	template<typename T>
	class ENGINE_DLLEXPORT ComponentManager : public BaseObject<>
	{
	private:
		static T* CreateComponent() { return ComponentsData.Alloc(); }
		static void RemoveComponent(T* cmp) { ComponentsData.Free(cmp); }
		static PoolAllocator<T> ComponentsData;

		friend class World;
	};
}