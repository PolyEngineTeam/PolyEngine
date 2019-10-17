#pragma once

#include <pe/Defines.hpp>
#include <pe/core/storage/String.hpp>

namespace pe::api::deps {

/// Interface for all dependencies.
/// @note Never implement this interface directly. Instead use the abstract base class DependencyBase
/// @see DependencyBase
class API_DLLEXPORT IDependency : public ::pe::core::BaseObject<>
{   
public:
	/// Provides the human redable name of the dependency. Does not have to be unique.
	/// @return Name of the dependency.
	virtual const core::storage::String& getName() const = 0;

	/// Provides std::type_index describing the type of the dependency.
	/// @return std::type_index of the dependency.
	virtual std::type_index getType() const = 0;

	friend class DependencyManager;
};

}