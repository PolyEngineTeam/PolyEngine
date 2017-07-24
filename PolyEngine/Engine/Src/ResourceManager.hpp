#pragma once

#include <Defines.hpp>
#include <Logger.hpp>
#include <String.hpp>
#include <FileIO.hpp>

#include "CoreConfig.hpp"
#include "ResourceBase.hpp"

#include <map>

namespace Poly
{
	class MeshResource;
	class TextureResource;
	class FontResource;

	ENGINE_DLLEXPORT String LoadTextFileRelative(eResourceSource Source, const String& path);

	namespace Impl { template<typename T> std::map<String, std::unique_ptr<T>>& GetResources(); }

#define DECLARE_RESOURCE(type, map_name) \
	namespace Impl { \
		ENGINE_DLLEXPORT extern std::map<String, std::unique_ptr<type>> map_name; \
		template<> inline std::map<String, std::unique_ptr<type>>& GetResources<type>() { return map_name; } \
	}

#define DEFINE_RESOURCE(type, map_name) namespace Poly { namespace Impl { std::map<String, std::unique_ptr<type>> map_name = {}; }}

	DECLARE_RESOURCE(MeshResource, gMeshResourcesMap)
	DECLARE_RESOURCE(TextureResource, gTextureResourcesMap)
	DECLARE_RESOURCE(FontResource, gFontResourcesMap)

	//------------------------------------------------------------------------------
	template<typename T>
	class ResourceManager
	{
	public:
		//------------------------------------------------------------------------------
		static T* Load(const String& path, bool isAbsolute = true, eResourceSource source = eResourceSource::ENGINE)
		{
			auto it = Impl::GetResources<T>().find(path);

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
				if (isAbsolute)
				{
					String absolutePath;
					bool isFound = false;
					Dynarray<String> paths = gCoreConfig.GetAssetsPaths(source);
					for (int i = 0; i < paths.GetSize() && !isFound; ++i)
					{
						absolutePath = paths[i] + path;
						isFound = FileExists(absolutePath);
					}

					if (isFound)
					{
						gConsole.LogError("Resource loading found absolute path: {}", absolutePath);
						resource = new T(absolutePath);
					}
					else 
					{
						gConsole.LogError("Resource absolute path not found! {}", path);
						return nullptr;
					}
				}
				else
				{
					resource = new T(path);
				}
			}
			catch (const ResourceLoadFailedException& e)
			{
				UNUSED(e);
				gConsole.LogError("Resource loading failed! {}", path);
				return nullptr;
			}
			catch (const std::exception& e)
			{
				UNUSED(e);
				HEAVY_ASSERTE(false, "Resource creation failed for unknown reason!");
				return nullptr;
			}

			Impl::GetResources<T>().insert(std::make_pair(path, std::unique_ptr<T>(resource)));
			resource->Path = path;
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
