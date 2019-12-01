#pragma once

#include <pe/Defines.hpp>
#include <pe/engine/config/AssetsPathConfig.hpp>
#include <pe/engine/res/ResourceBase.hpp>
#include <pe/engine/res/FontResource.hpp>

namespace pe::engine::res
{
	class MeshResource;
	class TextureResource;
	class FontResource;
	class SoundResource;

	ENGINE_DLLEXPORT ::pe::core::storage::String EvaluateFullResourcePath(config::eResourceSource source, const ::pe::core::storage::String& path);

	ENGINE_DLLEXPORT ::pe::core::storage::String LoadTextFileRelative(config::eResourceSource source, const ::pe::core::storage::String& path);
	ENGINE_DLLEXPORT void SaveTextFileRelative(config::eResourceSource Source, const ::pe::core::storage::String& path, const ::pe::core::storage::String& text);

	ENGINE_DLLEXPORT float* LoadImageHDR(const ::pe::core::storage::String& path, int* width, int* height, int* fileChannels);
	ENGINE_DLLEXPORT void FreeImageHDR(float* data);
	ENGINE_DLLEXPORT unsigned char* LoadImage(const ::pe::core::storage::String& path, int* width, int* height, int* fileChannels, int desiredChannels = 0);
	ENGINE_DLLEXPORT void FreeImage(unsigned char* data);


	namespace impl { template<typename T> std::map<::pe::core::storage::String, std::unique_ptr<T>>& GetResources(); }

#define ENGINE_DECLARE_RESOURCE(type, map_name) \
	namespace impl { \
		ENGINE_DLLEXPORT extern std::map<::pe::core::storage::String, std::unique_ptr<type>> map_name; \
		template<> inline std::map<::pe::core::storage::String, std::unique_ptr<type>>& GetResources<type>() { return map_name; } \
	}

#define DECLARE_RESOURCE(type, map_name) \
	namespace impl { \
		GAME_DLLEXPORT extern std::map<::pe::core::storage::String, std::unique_ptr<type>> map_name; \
		template<> inline std::map<::pe::core::storage::String, std::unique_ptr<type>>& GetResources<type>() { return map_name; } \
	}

#define TEST_DECLARE_RESOURCE(type, map_name) \
	namespace impl { \
		extern std::map<::pe::core::storage::String, std::unique_ptr<type>> map_name; \
		template<> inline std::map<::pe::core::storage::String, std::unique_ptr<type>>& GetResources<type>() { return map_name; } \
	}

#define DEFINE_RESOURCE(type, map_name) namespace pe::engine::res::impl { std::map<::pe::core::storage::String, std::unique_ptr<type>> map_name = {}; }

	ENGINE_DECLARE_RESOURCE(MeshResource, gMeshResourcesMap)
	ENGINE_DECLARE_RESOURCE(TextureResource, gTextureResourcesMap)
	ENGINE_DECLARE_RESOURCE(FontResource, gFontResourcesMap)
	ENGINE_DECLARE_RESOURCE(SoundResource, gALSoundResourcesMap)

	template<typename T>
	class ResourceManager
	{
	public:
		static T* LoadEngineAsset(const ::pe::core::storage::String& path)
		{
			return Load(path, config::eResourceSource::ENGINE);
		}

		static T* LoadGameAsset(const ::pe::core::storage::String& path)
		{
			return Load(path, config::eResourceSource::GAME);
		}

		template<typename... Args>
		static T* Load(const ::pe::core::storage::String& path, config::eResourceSource source, Args&&... args)
		{
			auto it = impl::GetResources<T>().find(path);

			// Check if it is already loaded
			if (it != impl::GetResources<T>().end())
			{
				T* resource = it->second.get();
				resource->AddRef();
				return resource;
			}

			// Load the resource
			T* resource = nullptr;
			::pe::core::storage::String absolutePath = config::gAssetsPathConfig.GetAssetsPath(source) + path;
			::pe::core::utils::gConsole.LogInfo("ResourceManager: Loading: {}", absolutePath);

			try
			{
				auto new_resource = new T(absolutePath, std::forward<Args>(args)...);
				resource = new_resource;
			} catch (const ResourceLoadFailedException&) {
				resource = nullptr;
			} catch (const std::exception&) {
				HEAVY_ASSERTE(false, "Resource creation failed for unknown reason!");
				return nullptr;
			}

			if (!resource)
			{
				::pe::core::utils::gConsole.LogError("Resource loading failed! {}", path);
				return nullptr;
			}

			impl::GetResources<T>().insert(std::make_pair(path, std::unique_ptr<T>(resource)));
			resource->Path = path;
			resource->AddRef();
			return resource;
		}

		static void Release(T* resource)
		{
			if (resource->RemoveRef())
			{
				auto it = impl::GetResources<T>().find(resource->GetPath());
				HEAVY_ASSERTE(it != impl::GetResources<T>().end(), "Resource creation failed!");
				impl::GetResources<T>().erase(it);
			}
		}
	};
}
