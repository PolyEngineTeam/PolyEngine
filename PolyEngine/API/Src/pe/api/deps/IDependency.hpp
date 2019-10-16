#pragma once

#include <pe/Defines.hpp>
#include <pe/core/storage/String.hpp>

namespace pe::api::deps {

class API_DLLEXPORT IDependency : public ::pe::core::BaseObject<>
{   
public:
    virtual core::storage::String GetName() const = 0;
};

class API_DLLEXPORT DependencyBase : public IDependency
{   
public:
    DependencyBase()
};

}