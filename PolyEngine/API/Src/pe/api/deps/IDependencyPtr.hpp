#pragma once

#include <pe/Defines.hpp>
#include <pe/api/deps/IDependency.hpp>

namespace pe::api::deps {

class IDependencyPtr : public ::pe::core::BaseObjectLiteralType<>
{
public:
    virtual IDependency* getMutable() const = 0;
    virtual const IDependency* get() const = 0;
    
protected:
    virtual void inject(IDependency* dep) = 0;
    virtual void reset() = 0;

    virtual std::type_index getType() const = 0;

    friend class DependencyManager;
};

}