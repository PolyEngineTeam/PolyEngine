#pragma once

#include <pe/Defines.hpp>

#include <pe/api/deps/IDependency.hpp>
#include <pe/api/deps/IDependencyPtr.hpp>

namespace pe::api::deps {

class API_DLLEXPORT DependencyManager final : public ::pe::core::BaseObjectLiteralType<>
{   
public:
    static DependencyManager& get();

    void registerDependency(IDependency* dependency);
    void unregisterDependency(IDependency* dependency);

    void registerDependencyPtr(IDependencyPtr* ptr);
    void unregisterDependencyPtr(IDependencyPtr* ptr);

private:
    void Inject(IDependencyPtr* ptr);
    void Reset(IDependencyPtr* ptr);
    void InjectAll(IDependency* dependency);
    void ResetAll(std::type_index dependencyType);

    DependencyManager();
    ~DependencyManager();
    DependencyManager(const DependencyManager&) = delete;
    DependencyManager(DependencyManager&&) = delete;
    DependencyManager& operator=(const DependencyManager&) = delete;
    DependencyManager& operator=(DependencyManager&&) = delete;

    std::unordered_map<std::type_index, IDependency*> m_dependencyMap;
    std::unordered_multimap<std::type_index, IDependencyPtr*> m_dependencyPtrMap;
};

}