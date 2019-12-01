#include <pe/api/APIPCH.hpp>

#include <pe/api/ecs/Factories.hpp>
#include <pe/api/ecs/Scene.hpp>

namespace pe::api::ecs {

EntityUniquePtr EntityFactory(::Poly::RTTIBase* parent, ::Poly::RTTI::TypeInfo t)
{
	Scene* s = rtti_cast<Scene*>(parent);
	if(Entity* e = rtti_cast<Entity*>(parent))
	{
		s = e->GetScene();
		ASSERTE(s, "Unknown scene!");
	}
	
	return s->GetSceneAllocator().NewEntity();
}

ComponentUniquePtr<ComponentBase> ComponentFactory(::Poly::RTTIBase* parent, ::Poly::RTTI::TypeInfo t)
{
	Scene* s = rtti_cast<Scene*>(parent);
	if (!s)
	{
		Entity* e = rtti_cast<Entity*>(parent);
		s = e ? e->GetScene() : nullptr;
	}
	ASSERTE(s, "Unknown parent type!");

	const size_t id = ComponentManager::Get().GetComponentID(t).value();
	core::utils::gConsole.LogInfo("Allocationg component: {} with ID {} and typeID {}", t.GetTypeName(), id, t);
	ComponentBase* ptr = s->GetSceneAllocator().AllocateComponentMemory(id);
	t.CreateInstanceInPlace(ptr);
	return ComponentUniquePtr<ComponentBase>(ptr, s->GetSceneAllocator().GetComponentDeleter());
}

}