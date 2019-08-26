#include <EnginePCH.hpp>

#include <Configs/AssetsPathConfig.hpp>

RTTI_DEFINE_TYPE(Poly::AssetsPathConfig)

namespace Poly
{
	static const core::storage::String DEFAULT_ENGINE_ASSETS_PATH = core::storage::String("../Engine/Res/");
	static const core::storage::String DEFAULT_GAME_ASSETS_PATH = core::storage::String("../Games/SGJGame/Res/");

	AssetsPathConfig gAssetsPathConfig;

	// Location needs to be set to eResourceSource::NONE, as this config prepares all required source paths
	AssetsPathConfig::AssetsPathConfig() : ConfigBase("Assets Path Config", eResourceSource::NONE) 
	{
		EngineAssetsPath = DEFAULT_ENGINE_ASSETS_PATH;
		GameAssetsPath = DEFAULT_GAME_ASSETS_PATH;
	}

	const core::storage::String& AssetsPathConfig::GetAssetsPath(eResourceSource source) const
	{
		switch (source)
		{
		case eResourceSource::ENGINE:
			return EngineAssetsPath;
		case eResourceSource::GAME:
			return GameAssetsPath;
		case eResourceSource::NONE:
		case eResourceSource::USR_LOCAL: //TODO implement usr local directory
			return core::storage::String::EMPTY;
		default:
			ASSERTE(false, "Invalid resource source!");
			return core::storage::String::EMPTY;
		}
	}
}