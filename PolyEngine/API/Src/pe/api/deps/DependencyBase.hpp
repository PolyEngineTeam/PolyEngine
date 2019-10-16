#pragma once

#include <pe/Defines.hpp>
#include <pe/core/storage/String.hpp>

namespace pe::api::deps {

class API_DLLEXPORT DependencyBase : public ::pe::core::BaseObject<>
{   
public:
    DependencyBase();
    virtual ~DependencyBase();

    virtual std::type_index getType() const = 0;
};

}