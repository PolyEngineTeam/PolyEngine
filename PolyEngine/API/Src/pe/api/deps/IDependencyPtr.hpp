#pragma once

#include <pe/Defines.hpp>
#include <pe/api/deps/DependencyBase.hpp>

namespace pe::api::deps {

class IDependencyPtr : public ::pe::core::BaseObjectLiteralType<>
{
public:
    virtual DependencyBase* getMutable() const = 0;
    virtual const DependencyBase* get() const = 0;
    
protected:
    virtual void inject(DependencyBase* dep) = 0;
    virtual void reset() = 0;

    virtual std::type_index getType() const = 0;

    friend class DependencyManager;
};

}