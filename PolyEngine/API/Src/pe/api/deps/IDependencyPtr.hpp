#pragma once

#include <pe/Defines.hpp>
#include <pe/api/deps/IDependency.hpp>

namespace pe::api::deps {

/// Interface for all DependencyPtr specializations.
class IDependencyPtr : public ::pe::core::BaseObjectLiteralType<>
{
public:
	/// Getter for mutable pointer to the dependency.
	/// @return Mutable pointer to the dependency.
	virtual IDependency* getMutable() const = 0;

	/// Getter for constant pointer to the dependency.
	/// @return Constant pointer to the dependency.
	virtual const IDependency* get() const = 0;
	
protected:
	/// Injects the dependency to the DependencyPtr.
	/// @note Protected api used only by DependecyManager.
	/// @param[in] dependency Pointer to the dependency.
	virtual void inject(IDependency* dependency) = 0;

	/// Resets the dependency in the DependencyPtr.
	/// @note Protected api used only by DependecyManager.
	virtual void reset() = 0;

	/// Provides std::type_index describing the type of the held dependency.
	/// @note Protected api used only by DependecyManager.
	/// @return std::type_index of the held dependency.
	virtual std::type_index getType() const = 0;

	friend class DependencyManager;
};

}