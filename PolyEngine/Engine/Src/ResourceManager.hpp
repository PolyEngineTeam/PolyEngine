#pragma once

#include <Defines.hpp>
#include <Logger.hpp>
#include <String.hpp>
#include <RefCountedBase.hpp>

#include <map>
#include <exception>

namespace Poly
{
	ENGINE_DLLEXPORT const String& GetResourcesAbsolutePath();
	
	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ResourceLoadFailedException : public std::exception
	{
	public:
		ResourceLoadFailedException() {}
	};
	
	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ResourceBase : public RefCountedBase
	{
	public:
		const String& GetPath() const { return Path; }

		ResourceBase() = default;
		ResourceBase(const ResourceBase&) = delete;
		ResourceBase& operator=(const ResourceBase&) = delete;

	protected:
		virtual ~ResourceBase() {}
	
	private:
		String Path;

		template<typename T> friend class ResourceManager;
	};

	//------------------------------------------------------------------------------
	template<typename T>
	class ResourceManager
	{
	public:
		//------------------------------------------------------------------------------
		static T* Load(const String& relativePath)
		{
			auto it = GetResources().find(relativePath);
			
			// Check if it is already loaded
			if (it != GetResources().end())
			{
				T* resource = it->second.get();
				resource->AddRef();
				return resource;
			}

			// Load the resource
			T* resource = nullptr;
			try {
				//TODO create wrapper for path
				resource = new T(GetResourcesAbsolutePath() + relativePath);
			}
			catch (const ResourceLoadFailedException& e)
			{
				gConsole.LogError("Resource loading failed! {}", relativePath);
				return nullptr;
			}
			catch (const std::exception& e)
			{
				HEAVY_ASSERTE(false, "Resource creation failed for unknown reason!");
				return nullptr;
			}
			
			GetResources().insert(std::make_pair(relativePath, std::unique_ptr<T>(resource)));
			resource->Path = relativePath;
			resource->AddRef();
			return resource;
		}

		//------------------------------------------------------------------------------
		static void Release(T* resource)
		{
			if (resource->RemoveRef())
			{
				auto it = GetResources().find(resource->GetPath());
				HEAVY_ASSERTE(it != GetResources().end(), "Resource creation failed!");
				GetResources().erase(it);
			}
		}
	private:
		// This is hack for static std::map ...
		static std::map<String, std::unique_ptr<T>>& GetResources()
		{
			static std::map<String, std::unique_ptr<T>> Resources;
			return Resources;
		}
	};
}