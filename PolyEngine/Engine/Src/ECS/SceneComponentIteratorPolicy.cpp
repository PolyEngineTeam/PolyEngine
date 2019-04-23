#include <EnginePCH.hpp>

#include <ECS/SceneComponentIteratorPolicy.hpp>
#include <ECS/Entity.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

SceneComponentIteratorPolicy::SceneComponentIteratorPolicy(std::unique_ptr<IAllocatorIterator> begin, std::unique_ptr<IAllocatorIterator> end) 
{
    Iterator = std::move(begin);
    End = std::move(end);
}

SceneComponentIteratorPolicy SceneComponentIteratorPolicy::ConstructBegin(Entity* entity, size_t primaryComponentID)
{
    auto componentAllocator = entity->GetEntityScene()->GetComponentAllocator(primaryComponentID);
    return SceneComponentIteratorPolicy(std::move(componentAllocator->GetAllocatorBegin()), std::move(componentAllocator->GetAllocatorEnd()));
}

SceneComponentIteratorPolicy SceneComponentIteratorPolicy::ConstructEnd(Entity* entity, size_t primaryComponentID)
{
    auto componentAllocator = entity->GetEntityScene()->GetComponentAllocator(primaryComponentID);
    return SceneComponentIteratorPolicy(std::move(componentAllocator->GetAllocatorEnd()), std::move(componentAllocator->GetAllocatorEnd()));
}

bool SceneComponentIteratorPolicy::operator==(const IEntityIteratorPolicy& rhs) const
{
    return Get() == rhs.Get();
}

bool SceneComponentIteratorPolicy::operator!=(const IEntityIteratorPolicy& rhs) const
{
    return !(*this == rhs); // can move to interface after it works
}

ComponentBase* SceneComponentIteratorPolicy::Get() const
{
    return reinterpret_cast<ComponentBase*>(Iterator->data());
}

void SceneComponentIteratorPolicy::Increment()
{
    Iterator->increment();
}

bool SceneComponentIteratorPolicy::IsValid() const 
{
    return Iterator->data() != End->data();
}