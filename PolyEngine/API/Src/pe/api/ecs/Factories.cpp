#include <pe/api/APIPCH.hpp>

#include <pe/api/ecs/Factories.hpp>
#include <pe/api/ecs/Scene.hpp>

namespace pe::api::ecs {

EntityUniquePtr entityFactory(::Poly::RTTIBase* parent, ::Poly::RTTI::TypeInfo t)
{
	Scene* s = rtti_cast<Scene*>(parent);
	if(Entity* e = rtti_cast<Entity*>(parent))
	{
		s = e->getScene();
		ASSERTE(s, "Unknown scene!");
	}
	
	return s->getSceneAllocator().newEntity();
}

ComponentUniquePtr<ComponentBase> componentFactory(::Poly::RTTIBase* parent, ::Poly::RTTI::TypeInfo t)
{
	Scene* s = rtti_cast<Scene*>(parent);
	if (!s)
	{
		Entity* e = rtti_cast<Entity*>(parent);
		s = e ? e->getScene() : nullptr;
	}
	ASSERTE(s, "Unknown parent type!");

	const size_t id = ComponentManager::Get().GetComponentID(t).value();
	core::utils::gConsole.LogInfo("Allocationg component: {} with ID {} and typeID {}", t.GetTypeName(), id, t);
	ComponentBase* ptr = s->getSceneAllocator().allocateComponentMemory(id);
	t.CreateInstanceInPlace(ptr);
	return ComponentUniquePtr<ComponentBase>(ptr, s->getSceneAllocator().getComponentDeleter());
}

}