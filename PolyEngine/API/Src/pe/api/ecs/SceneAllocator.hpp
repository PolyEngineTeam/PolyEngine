#pragma once

#include <pe/Defines.hpp>
#include <pe/core/memory/IterablePoolAllocator.hpp>
#include <pe/core/rtti/RTTI.hpp>
#include <pe/api/ecs/Deleters.hpp>
#include <pe/api/ecs/ComponentBase.hpp>

namespace pe::api::ecs {

class Scene;
class Entity;
class ComponentBase;

constexpr size_t MAX_ENTITY_COUNT = 65536;
constexpr size_t MAX_COMPONENTS_COUNT = 64;

using EntityUniquePtr = std::unique_ptr<Entity, EntityDeleter>;
template<typename T>
using ComponentUniquePtr = std::unique_ptr<T, ComponentDeleter>;

class API_DLLEXPORT SceneAllocator final : public core::BaseObjectLiteralType<>
{
public:
	using IterablePoolAllocatorBase = ::pe::core::memory::IterablePoolAllocatorBase;
	template<typename T>
	using IterablePoolAllocator = ::pe::core::memory::IterablePoolAllocator<T>;

	SceneAllocator(Scene* scene);
	~SceneAllocator();

	// ########## Entity memory management ##########
	[[nodiscard]] EntityUniquePtr NewEntity();
	void DeleteEntity(Entity* e);
	EntityDeleter& GetEntityDeleter() { return m_entityDeleter; }
	
	// ########## Component memory management ##########
	template<typename T, typename... Args>
	[[nodiscard]] ComponentUniquePtr<T> NewComponent(Args&&... args)
	{
		T* ptr = GetComponentAllocator<T>().Alloc();
		::new(ptr) T(std::forward<Args>(args)...);
		return ComponentUniquePtr<T>(ptr, m_componentDeleter);
	}
	void DeleteComponent(ComponentBase* c);
	ComponentDeleter& GetComponentDeleter() { return m_componentDeleter; }

	// ########## Component iteration ##########
	template<typename T>
	[[nodiscard]] typename IterablePoolAllocator<T>::Iterator componentsBegin()
	{
		return GetComponentAllocator<T>().Begin();
	}
	template<typename T>
	[[nodiscard]] typename IterablePoolAllocator<T>::Iterator componentsEnd()
	{
		return GetComponentAllocator<T>().End();
	}

	// ########## Special component allocation case for RTTI ##########
	// @todo(muniu) get rid of this case
	[[nodiscard]] ComponentBase* AllocateComponentMemory(size_t componentID)
	{
		return reinterpret_cast<ComponentBase*>(GetComponentAllocator(componentID).GenericAlloc());
	}
private:
	template <typename T>
	IterablePoolAllocator<T>& GetComponentAllocator()
	{
		return static_cast<IterablePoolAllocator<T>&>(GetComponentAllocator(GetComponentID<T>()));
	}
	IterablePoolAllocatorBase& GetComponentAllocator(size_t componentID);

	Scene* m_scene;
	
	std::unique_ptr<IterablePoolAllocatorBase> m_entitiesAllocator;
	std::unique_ptr<IterablePoolAllocatorBase> m_componentAllocators[MAX_COMPONENTS_COUNT];
	
	ComponentDeleter m_componentDeleter;
	EntityDeleter m_entityDeleter;
};

}