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

		const String& GetAssetsPath(eResourceSource Source) const;

		const String& GetGameLibPath() const { return GameLibPath; }
		const String& GetRenderingDeviceLibPath() const { return RenderingDeviceLibPath; }
	private:
		String EngineAssetsPath;
		String GameAssetsPath;
		String RenderingDeviceLibPath;
		String GameLibPath;
	};

	ENGINE_DLLEXPORT extern AssetsPathConfig gAssetsPathConfig;
}
