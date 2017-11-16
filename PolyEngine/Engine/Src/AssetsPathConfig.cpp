#include "EnginePCH.hpp"

RTTI_DEFINE_TYPE(Poly::AssetsPathConfig)

namespace Poly
{
	static const String DEFAULT_ENGINE_ASSETS_PATH = String("../../Engine/Res/");
	static const String DEFAULT_GAME_ASSETS_PATH = String("../../Games/SGJGame/Res/");

	AssetsPathConfig gAssetsPathConfig;

	AssetsPathConfig::AssetsPathConfig() : ConfigBase("Assets Path Config", eResourceSource::NONE)
	{
		EngineAssetsPath = DEFAULT_ENGINE_ASSETS_PATH;
		GameAssetsPath = DEFAULT_GAME_ASSETS_PATH;
	}

	const String& AssetsPathConfig::GetAssetsPath(eResourceSource source) const
	{
		static String EMPTY;
		switch (source)
		{
		case eResourceSource::ENGINE:
			return EngineAssetsPath;
		case eResourceSource::GAME:
			return GameAssetsPath;
		case eResourceSource::NONE:
		case eResourceSource::USR_LOCAL: //TODO implement usr local directory
			return EMPTY;
		default:
			ASSERTE(false, "Invalid resoruce source!");
			return EMPTY;
		}
	}
}