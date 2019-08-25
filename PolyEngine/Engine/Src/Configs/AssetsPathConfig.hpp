#pragma once

#include <pe/Defines.hpp>
#include <Configs/ConfigBase.hpp>

namespace Poly
{
	class ENGINE_DLLEXPORT AssetsPathConfig : public ConfigBase
	{
		RTTI_DECLARE_TYPE_DERIVED(AssetsPathConfig, ConfigBase)
		{
			RTTI_PROPERTY(EngineAssetsPath, "EngineAssetsPath", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(GameAssetsPath, "GameAssetsPath", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(RenderingDeviceLibPath, "RenderingDeviceLibPath", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(GameLibPath, "GameLibPath", RTTI::ePropertyFlag::NONE);
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
