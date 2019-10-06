#pragma once

#include <pe/Defines.hpp>

#include <pe/api/res/IResource.hpp>

namespace pe::api::res {

class API_DLLEXPORT IResourceRepository : public ::pe::core::BaseObject<>
{   
public:
    virtual bool containsResource(const core::UniqueID& id) const = 0;
    virtual std::future<std::shared_ptr<IResource>> loadResourceAsync(const core::UniqueID& id) = 0;
};

}