#include <pe/api/APIPCH.hpp>

#include <pe/api/IEngine.hpp>

#include <pe/api/deps/DependencyManager.hpp>
#include <pe/core/utils/Logger.hpp>

using ::pe::core::utils::gConsole;

namespace pe::api::deps {

DependencyManager& DependencyManager::get()
{
	static DependencyManager instance;
	return instance;
}

DependencyManager::DependencyManager()
{
	gConsole.LogInfo("[DependencyManager] Dependency manager initialized.");
}

DependencyManager::~DependencyManager()
{
	gConsole.LogInfo("[DependencyManager] Dependency manager deinitialized.");
}

void DependencyManager::registerDependency(IDependency* dependency)
{
	ASSERTE(dependency, "Cannot register nullptr");
	auto type = dependency->getType();
	auto it = m_dependencyMap.find(type);
	ASSERTE(it == m_dependencyMap.end(), "Registering dependency of given type twice in a row.");
	m_dependencyMap.emplace(dependency->getType(), dependency);

	gConsole.LogInfo("[DependencyManager] Registered new dependency [{}].", dependency->getName());

	injectAll(dependency);
}

void DependencyManager::unregisterDependency(IDependency* dependency)
{
	ASSERTE(dependency, "Cannot unregister nullptr");
	auto type = dependency->getType();
	auto it = m_dependencyMap.find(type);
	ASSERTE(it != m_dependencyMap.end(), "Cannot unregister non-registered dependency!");
	ASSERTE(it->second == dependency, "Cannot unregister different dependency!");
	m_dependencyMap.erase(it);

	gConsole.LogInfo("[DependencyManager] Unregistered dependency [{}].", dependency->getName());

	resetAll(dependency->getType());
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
	inject(ptr);
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
	reset(ptr);
}

void DependencyManager::inject(IDependencyPtr* ptr)
{
	ASSERTE(ptr, "Ptr is nullptr");
	auto it = m_dependencyMap.find(ptr->getType());
	if (it != m_dependencyMap.end())
	{
		ptr->inject(it->second);
		gConsole.LogInfo("[DependencyManager] Injected dependency [{}] once.", it->second->getName());
	}
}

void DependencyManager::reset(IDependencyPtr* ptr)
{
	ASSERTE(ptr, "Ptr is nullptr");
	ptr->reset();
}

void DependencyManager::injectAll(IDependency* dependency)
{
	size_t count = 0;
	auto&& [it, end_it] = m_dependencyPtrMap.equal_range(dependency->getType());
	for(; it != end_it; ++it)
	{
		it->second->inject(dependency);
		++count;
	}
	if(count > 0)
	{
		gConsole.LogInfo("[DependencyManager] Injected dependency [{}] times {}.",
			dependency->getName(), count);
	}
}

void DependencyManager::resetAll(std::type_index dependencyType)
{
	auto&& [it, end_it] = m_dependencyPtrMap.equal_range(dependencyType);
	for(; it != end_it; ++it)
	{
		it->second->reset();
	}
}

}