#include <pe/api/IEngine.hpp>

#include <pe/api/deps/DependencyManager.hpp>
// TODO remove this file when other cpp-s are added.

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

void DependencyManager::registerDependency(IDependency* dependency)
{

}

void DependencyManager::registerDependencyPtr(IDependencyPtr& ptr)
{

}

void DependencyManager::unregisterDependency(IDependency* dependency)
{

}

void DependencyManager::unregisterDependencyPtr(IDependencyPtr& ptr)
{

}

void DependencyManager::updateDependencies()
{

}

}