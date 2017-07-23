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

		bool GetIsLoadedFromFile() const;

		bool ReloadFromFile();

	private:

		bool IsLoadedFromFile = false;

		std::unordered_map<eResourceSource, Dynarray<String>> AssetsPaths;
	};

	ENGINE_DLLEXPORT extern CoreConfig gCoreConfig;
}