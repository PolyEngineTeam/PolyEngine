#pragma once

#include <pe/Defines.hpp>
#include <pe/api/deps/IDependency.hpp>
#include <pe/api/deps/DependencyManager.hpp>


namespace pe::api::deps {

template<typename T>
class DependencyBase : public IDependency
{
public:
	DependencyBase()
	{
		DependencyManager::get().registerDependency(this);
	}

	virtual ~DependencyBase()
	{
		DependencyManager::get().unregisterDependency(this);
	}

	std::type_index getType() const override { return typeid(T); }
};

}