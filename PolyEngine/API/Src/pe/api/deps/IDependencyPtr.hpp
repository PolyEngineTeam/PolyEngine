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
    virtual void set(IDependency* dep) = 0;
};

}