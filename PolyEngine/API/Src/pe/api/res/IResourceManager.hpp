#pragma once

#include <pe/Defines.hpp>

#include <pe/core/UniqueID.hpp>
#include <pe/core/storage/String.hpp>

#include <pe/api/res/IResource.hpp>
#include <pe/api/res/IResourceRepository.hpp>


namespace pe::api::res {

class API_DLLEXPORT IResourceManager : public ::pe::core::BaseObject<>
{   
public:
    virtual ~IResourceManager() = default;

    virtual bool registerRepository(const core::storage::String& name,
        std::unique_ptr<IResourceRepository> resourceRepository) = 0;
    virtual bool unregisterRepository(const core::storage::String& name) = 0;

    virtual std::future<std::shared_ptr<IResource>> loadResourceAsync(const core::UniqueID& id) = 0;
};

}