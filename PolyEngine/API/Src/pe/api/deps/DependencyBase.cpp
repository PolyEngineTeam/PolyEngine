#include <pe/api/deps/DependencyBase.hpp>
#include <pe/api/deps/DependencyManager.hpp>

namespace pe::api::deps {

DependencyBase::DependencyBase()
{
    DependencyManager::get().registerDependency(this);
}

DependencyBase::~DependencyBase()
{
    DependencyManager::get().unregisterDependency(this);
}

}