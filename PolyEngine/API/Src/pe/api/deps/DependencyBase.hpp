#pragma once

#include <pe/Defines.hpp>

#include <pe/core/storage/String.hpp>

#include <pe/api/deps/IDependency.hpp>
#include <pe/api/deps/DependencyManager.hpp>


namespace pe::api::deps {

/// @brief Base abstract class for dependencies that want to be accessible via DI system.
/// 
/// All dependencies must derive (non-virtually) from this abstract class. This class
/// manages registration and unregistration process in the DI system automatically on
/// construction and destruction of the derived class.
template<typename T>
class DependencyBase : public IDependency
{
protected:
	/// @brief Constructor of the DependencyBase abstract base class.
	/// Registers dependency in the DependencyManager.
	/// @param[in] name Human redable name for the dependency. Does not have to be unique.
	explicit DependencyBase(core::storage::String name)
		: m_name(std::move(name))
	{
		DependencyManager::get().registerDependency(this);
	}

	/// @brief Destructor of the DependencyBase abstract base class.
	/// Unregisters dependency from the DependencyManager.
	virtual ~DependencyBase()
	{
		DependencyManager::get().unregisterDependency(this);
	}

	/// @see IDependency::getType()
	std::type_index getType() const override { return typeid(T); }

	/// @see IDependency::getName()
	const core::storage::String& getName() const override { return m_name; }

private:
	const core::storage::String m_name;
};

}