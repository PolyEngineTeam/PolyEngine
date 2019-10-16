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
    void registerDependencyPtr(IDependencyPtr& ptr);
    void unregisterDependency(IDependency* dependency);
    void unregisterDependencyPtr(IDependencyPtr& ptr);

    void updateDependencies();
private:
    DependencyManager();
    ~DependencyManager();
    DependencyManager(const DependencyManager&) = delete;
    DependencyManager(DependencyManager&&) = delete;
    DependencyManager& operator=(const DependencyManager&) = delete;
    DependencyManager& operator=(DependencyManager&&) = delete;
};

}