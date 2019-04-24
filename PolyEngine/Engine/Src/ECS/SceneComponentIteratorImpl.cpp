#include <EnginePCH.hpp>

#include <ECS/SceneComponentIteratorImpl.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

SceneComponentIteratorImpl::SceneComponentIteratorImpl(std::unique_ptr<IAllocatorIterator> begin, std::unique_ptr<IAllocatorIterator> end) 
{
    Iterator = std::move(begin);
    End = std::move(end);
}

SceneComponentIteratorImpl SceneComponentIteratorImpl::ConstructBegin(Scene* scene, size_t primaryComponentID)
{
    auto componentAllocator = scene->GetComponentAllocator(primaryComponentID);
    return SceneComponentIteratorImpl(componentAllocator->GetAllocatorBegin(), componentAllocator->GetAllocatorEnd());
}

SceneComponentIteratorImpl SceneComponentIteratorImpl::ConstructEnd(Scene* scene, size_t primaryComponentID)
{
    auto componentAllocator = scene->GetComponentAllocator(primaryComponentID);
    return SceneComponentIteratorImpl(componentAllocator->GetAllocatorEnd(), componentAllocator->GetAllocatorEnd());
}

bool SceneComponentIteratorImpl::operator==(const IComponentIteratorImpl& rhs) const
{
    return Get() == rhs.Get();
}

bool SceneComponentIteratorImpl::operator!=(const IComponentIteratorImpl& rhs) const
{
    return !(*this == rhs); // can move to interface after it works
}

ComponentBase* SceneComponentIteratorImpl::Get() const
{
    return reinterpret_cast<ComponentBase*>(Iterator->data());
}

void SceneComponentIteratorImpl::Increment()
{
    Iterator->increment();
}

bool SceneComponentIteratorImpl::IsValid() const 
{
    return Iterator->data() != End->data();
}