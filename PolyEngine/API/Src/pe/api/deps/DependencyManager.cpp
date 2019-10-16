#include <pe/api/IEngine.hpp>

#include <pe/api/deps/DependencyManager.hpp>

namespace pe::api::deps {

DependencyManager& DependencyManager::get()
{
    static DependencyManager instance;
    return instance;
}

DependencyManager::DependencyManager()
{

}

DependencyManager::~DependencyManager()
{
    
}

void DependencyManager::registerDependency(DependencyBase* dependency)
{
    ASSERTE(dependency, "Cannot register nullptr");
	auto type = dependency->getType();
	auto it = m_dependencyMap.find(type);
    ASSERTE(it == m_dependencyMap.end(), "Registering dependency of given type twice in a row.");
    m_dependencyMap.emplace(dependency->getType(), dependency);
    InjectAll(dependency);
}

void DependencyManager::unregisterDependency(DependencyBase* dependency)
{
    ASSERTE(dependency, "Cannot unregister nullptr");
    auto it = m_dependencyMap.find(dependency->getType());
    ASSERTE(it != m_dependencyMap.end(), "Cannot unregister non-registered dependency!");
    ASSERTE(it->second == dependency, "Cannot unregister different dependency!");
    m_dependencyMap.erase(it);
    ResetAll(dependency->getType());
}

void DependencyManager::registerDependencyPtr(IDependencyPtr* ptr)
{
    ASSERTE(ptr, "Cannot register nullptr");
#if defined(DEBUG)
    auto&& [it, end_it] = m_dependencyPtrMap.equal_range(ptr->getType());
    for(; it != end_it; ++it)
    {
        ASSERTE(it->second != ptr, "This pointer already exists in the map!");
    }
#endif
    m_dependencyPtrMap.emplace(ptr->getType(), ptr);
    Inject(ptr);
}

void DependencyManager::unregisterDependencyPtr(IDependencyPtr* ptr)
{
    ASSERTE(ptr, "Cannot unregister nullptr");
    auto&& [it, end_it] = m_dependencyPtrMap.equal_range(ptr->getType());
    for(; it != end_it; ++it)
    {
        if(it->second == ptr)
            break;
    }

    ASSERTE(it != end_it, "Cannot unregister non-registered dependency pointer!");
    m_dependencyPtrMap.erase(it);
    Reset(ptr);
}

void DependencyManager::Inject(IDependencyPtr* ptr)
{
    ASSERTE(ptr, "Ptr is nullptr");
    auto it = m_dependencyMap.find(ptr->getType());
    if (it != m_dependencyMap.end())
    {
        ptr->inject(it->second);
    }
}

void DependencyManager::Reset(IDependencyPtr* ptr)
{
    ASSERTE(ptr, "Ptr is nullptr");
    ptr->reset();
}

void DependencyManager::InjectAll(DependencyBase* dependency)
{
    auto&& [it, end_it] = m_dependencyPtrMap.equal_range(dependency->getType());
    for(; it != end_it; ++it)
    {
        it->second->inject(dependency);
    }
}

void DependencyManager::ResetAll(std::type_index dependencyType)
{
    auto&& [it, end_it] = m_dependencyPtrMap.equal_range(dependencyType);
    for(; it != end_it; ++it)
    {
        it->second->reset();
    }
}

}