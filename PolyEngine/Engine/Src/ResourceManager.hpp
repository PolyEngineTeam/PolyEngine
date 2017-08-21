#pragma once

#include <Core.hpp>
#include <Defines.hpp>
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
	class SoundResource;

	ENGINE_DLLEXPORT String LoadTextFileRelative(eResourceSource Source, const String& path);

	namespace Impl { template<typename T> std::map<String, std::unique_ptr<T>>& GetResources(); }

#define ENGINE_DECLARE_RESOURCE(type, map_name) \
	namespace Impl { \
		ENGINE_DLLEXPORT extern std::map<String, std::unique_ptr<type>> map_name; \
		template<> inline std::map<String, std::unique_ptr<type>>& GetResources<type>() { return map_name; } \
	}

#define DECLARE_RESOURCE(type, map_name) \
	namespace Impl { \
		GAME_DLLEXPORT extern std::map<String, std::unique_ptr<type>> map_name; \
		template<> inline std::map<String, std::unique_ptr<type>>& GetResources<type>() { return map_name; } \
	}

#define DEFINE_RESOURCE(type, map_name) namespace Poly { namespace Impl { std::map<String, std::unique_ptr<type>> map_name = {}; }}

	ENGINE_DECLARE_RESOURCE(MeshResource, gMeshResourcesMap)
	ENGINE_DECLARE_RESOURCE(TextureResource, gTextureResourcesMap)
	ENGINE_DECLARE_RESOURCE(FontResource, gFontResourcesMap)
	ENGINE_DECLARE_RESOURCE(SoundResource, gALSoundResourcesMap)

	//------------------------------------------------------------------------------
	template<typename T>
	class ResourceManager
	{
	public:
		static T* LoadEngineAsset(const String& path)
		{
			return Load(path, true, eResourceSource::ENGINE);
		}

		static T* LoadGameAsset(const String& path)
		{
			return Load(path, true, eResourceSource::GAME);
		}

		//------------------------------------------------------------------------------
		static T* Load(const String& path, eResourceSource source = eResourceSource::NONE)
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
			gConsole.LogInfo("ResourceManager: Loading: {}", path);
			T* resource = nullptr;
			Dynarray<String> paths = (source == eResourceSource::NONE) ? Dynarray<String>({String()}) : gAssetsPathConfig.GetAssetsPaths(source);
			for (size_t i = 0; i < paths.GetSize(); ++i)
			{
				String absolutePath = paths[i] + path;

				try
				{
					auto new_resource = new T(absolutePath);
					resource = new_resource;
					break;
				} catch (const ResourceLoadFailedException&) {
					resource = nullptr;
				} catch (const std::exception&) {
					HEAVY_ASSERTE(false, "Resource creation failed for unknown reason!");
					return nullptr;
				}
			}

			if (!resource)
			{
				gConsole.LogError("Resource loading failed! {}", path);
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
