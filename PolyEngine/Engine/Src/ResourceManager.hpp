#pragma once

#include <Defines.hpp>
#include <Logger.hpp>
#include <String.hpp>

#include "ResourceBase.hpp"

#include <map>

namespace Poly
{
	class GLMeshResource;
	class GLTextureResource;
	class FontResource;
	class SoundResource;

	ENGINE_DLLEXPORT const String& GetResourcesAbsolutePath();

	namespace Impl { template<typename T> std::map<String, std::unique_ptr<T>>& GetResources(); }

#define DECLARE_RESOURCE(type, map_name) \
	namespace Impl { \
		ENGINE_DLLEXPORT extern std::map<String, std::unique_ptr<type>> map_name; \
		template<> inline std::map<String, std::unique_ptr<type>>& GetResources<type>() { return map_name; } \
	}

#define DEFINE_RESOURCE(type, map_name) namespace Poly { namespace Impl { std::map<String, std::unique_ptr<type>> map_name = {}; }}

	DECLARE_RESOURCE(GLMeshResource, gGLMeshResourcesMap)
	DECLARE_RESOURCE(GLTextureResource, gGLTextureResourcesMap)
	DECLARE_RESOURCE(FontResource, gFontResourcesMap)
	DECLARE_RESOURCE(SoundResource, gALSoundResourcesMap)

	//------------------------------------------------------------------------------
	template<typename T>
	class ResourceManager
	{
	public:
		//------------------------------------------------------------------------------
		static T* Load(const String& relativePath, bool absolute = true)
		{
			auto it = Impl::GetResources<T>().find(relativePath);

			// Check if it is already loaded
			if (it != Impl::GetResources<T>().end())
			{
				T* resource = it->second.get();
				resource->AddRef();
				return resource;
			}

			// Load the resource
			T* resource = nullptr;
			try {
				//TODO create wrapper for path
				if (absolute)
				{
					resource = new T(GetResourcesAbsolutePath() + relativePath);
				}
				else
				{
					resource = new T(relativePath);
				}
			}
			catch (const ResourceLoadFailedException& e)
			{
				UNUSED(e);
				gConsole.LogError("Resource loading failed! {}", relativePath);
				return nullptr;
			}
			catch (const std::exception& e)
			{
				UNUSED(e);
				HEAVY_ASSERTE(false, "Resource creation failed for unknown reason!");
				return nullptr;
			}

			Impl::GetResources<T>().insert(std::make_pair(relativePath, std::unique_ptr<T>(resource)));
			resource->Path = relativePath;
			resource->AddRef();
			return resource;
		}

		//------------------------------------------------------------------------------
		static void Release(T* resource)
		{
			if (resource->RemoveRef())
			{
				auto it = Impl::GetResources<T>().find(resource->GetPath());
				HEAVY_ASSERTE(it != Impl::GetResources<T>().end(), "Resource creation failed!");
				Impl::GetResources<T>().erase(it);
			}
		}
	};	
}
