#pragma once

#include <pe/Defines.hpp>
#include <pe/engine/config/ConfigBase.hpp>

namespace pe::engine::config
{
	class ENGINE_DLLEXPORT AssetsPathConfig : public ::pe::engine::config::ConfigBase
	{
		RTTI_DECLARE_TYPE_DERIVED(AssetsPathConfig, ConfigBase)
		{
			RTTI_PROPERTY(EngineAssetsPath, "EngineAssetsPath", Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(GameAssetsPath, "GameAssetsPath", Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(RenderingDeviceLibPath, "RenderingDeviceLibPath", Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(GameLibPath, "GameLibPath", Poly::RTTI::ePropertyFlag::NONE);
		}
	public:
		AssetsPathConfig();

		const ::pe::core::storage::String& GetAssetsPath(eResourceSource Source) const;

		const ::pe::core::storage::String& GetGameLibPath() const { return GameLibPath; }
		const ::pe::core::storage::String& GetRenderingDeviceLibPath() const { return RenderingDeviceLibPath; }
	private:
		::pe::core::storage::String EngineAssetsPath;
		::pe::core::storage::String GameAssetsPath;
		::pe::core::storage::String RenderingDeviceLibPath;
		::pe::core::storage::String GameLibPath;
	};

	ENGINE_DLLEXPORT extern AssetsPathConfig gAssetsPathConfig;
}
