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

		const core::storage::String& GetAssetsPath(eResourceSource Source) const;

		const core::storage::String& GetGameLibPath() const { return GameLibPath; }
		const core::storage::String& GetRenderingDeviceLibPath() const { return RenderingDeviceLibPath; }
	private:
		core::storage::String EngineAssetsPath;
		core::storage::String GameAssetsPath;
		core::storage::String RenderingDeviceLibPath;
		core::storage::String GameLibPath;
	};

	ENGINE_DLLEXPORT extern AssetsPathConfig gAssetsPathConfig;
}
