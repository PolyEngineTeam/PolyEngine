#pragma once

#include <unordered_map>
#include <EnumUtils.hpp>

namespace Poly
{
	/// <summary>Enum used to identify source of asset.
	enum class eResourceSource
	{
		ENGINE,
		GAME,
		NONE,
		_COUNT
	};

	class ENGINE_DLLEXPORT AssetsPathConfig
	{
	public:
		Dynarray<String> GetAssetsPaths(eResourceSource Source) const;

		bool IsLoadedFromFile() const;

		bool ReloadFromFile();

		void LoadDefaults();

	private:
		const String ASSETS_PATH_CONFIG_PATH = String("AssetsPath.json");
		const String DEFAULT_ENGINE_ASSETS_PATH = String("../../Engine/Res/");
		const String DEFAULT_GAME_ASSETS_PATH = String("../../Games/SGJGame/Res/");

		bool LoadedFromFile = false;

		EnumArray<Dynarray<String>, eResourceSource> AssetsPaths;

		void WriteDefaultJson();
	};

	ENGINE_DLLEXPORT extern AssetsPathConfig gAssetsPathConfig;
}
