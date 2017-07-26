#pragma once

#include <unordered_map>

namespace Poly
{
	/// <summary>Enum used to identify source of asset.
	/// <see cref="RegisterComponent()">
	enum class eResourceSource
	{
		ENGINE,
		GAME,
		_COUNT
	};

	/// <summary>This is class where all core configuration variables are placed</summary>
	class ENGINE_DLLEXPORT CoreConfig
	{
	public:

		// Rendering
		bool DebugNormalsFlag = false;
		bool WireframeRendering = false;
		bool DisplayFPS = true;

		Dynarray<String> GetAssetsPaths(eResourceSource Source) const;

		bool IsLoadedFromFile() const;

		bool ReloadFromFile();

		void LoadDefaults();

	private:

		const String CORE_CONFIG_PATH = String("CoreConfig.json");
		const String DEFAULT_ENGINE_ASSETS_PATH = String("../../Engine/Res/");
		const String DEFAULT_GAME_ASSETS_PATH = String("../../Gamee/Res/");

		bool LoadedFromFile = false;

		EnumArray<Dynarray<String>, eResourceSource> AssetsPaths;

		void WriteDefaultJson();

		//std::unordered_map<eResourceSource, Dynarray<String>> AssetsPaths;
	};

	ENGINE_DLLEXPORT extern CoreConfig gCoreConfig;
}