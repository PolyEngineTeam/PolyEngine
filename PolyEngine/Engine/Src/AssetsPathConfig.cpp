#include "EnginePCH.hpp"

#include <rapidjson/document.h>

namespace Poly
{
	AssetsPathConfig gAssetsPathConfig;

	Dynarray<String> AssetsPathConfig::GetAssetsPaths(eResourceSource Source) const
	{
		HEAVY_ASSERTE(Source < eResourceSource::_COUNT, "_COUNT enum value passed to GetAssetsPaths(), which is an invalid value");
		// TODO: move to very first start block in program
		if (!gAssetsPathConfig.IsLoadedFromFile())
			gAssetsPathConfig.ReloadFromFile();
		
		return AssetsPaths[Source];
	}

	bool AssetsPathConfig::IsLoadedFromFile() const
	{
		return LoadedFromFile;
	}

	bool AssetsPathConfig::ReloadFromFile()
	{
		gConsole.LogInfo("CoreConfig::ReloadFromFile() called");
		bool IsSuccessfull = true;

		String ConfigFileContent;
		try
		{
			ConfigFileContent = LoadTextFile(ASSETS_PATH_CONFIG_PATH);
			// gConsole.LogInfo("File Content: {}", ConfigFileContent);
		}
		catch (FileIOException)
		{
			IsSuccessfull = false;
			gConsole.LogInfo("Config file reading failed: {}, using defaults", ASSETS_PATH_CONFIG_PATH);

			LoadDefaults();

			try
			{
				WriteDefaultJson();
			}
			catch (FileIOException)
			{
				gConsole.LogInfo("Config file saving defaults failed");
			}
		}

		rapidjson::Document Json; // UTF8 by default
		if (Json.Parse(ConfigFileContent.GetCStr()).HasParseError())
		{
			IsSuccessfull = false;
			gConsole.LogInfo("Config file json has error: {}", Json.GetParseError());

			LoadDefaults();
		}
		else
		{
			// validate
			HEAVY_ASSERTE(Json.IsObject(), "Json root must be Object");

			if (Json.HasMember("EngineAssetsPaths") && Json["EngineAssetsPaths"].IsArray())
			{
				Dynarray<String> EngineAssetPaths;
				const rapidjson::Value& JsonArray = Json["EngineAssetsPaths"];
				for (rapidjson::SizeType i = 0; i < JsonArray.Size(); i++)
				{
					if (JsonArray[i].IsString())
					{
						String Path = String(JsonArray[i].GetString());
						EngineAssetPaths.PushBack(Path);
						gConsole.LogInfo("CoreConfig: EngineAssetsPaths element: {}", Path);
					}
					else
					{
						gConsole.LogInfo("CoreConfig: EngineAssetsPaths element {} is not String", i);
					}
				}

				AssetsPaths[eResourceSource::ENGINE] = EngineAssetPaths;
			}
			else
			{
				gConsole.LogInfo("CoreConfig: Array EngineAssetsPaths not found");
			}

			if (Json.HasMember("GameAssetsPaths") && Json["GameAssetsPaths"].IsArray())
			{
				Dynarray<String> GameAssetsPaths;
				const rapidjson::Value& JsonArray = Json["GameAssetsPaths"];
				for (rapidjson::SizeType i = 0; i < JsonArray.Size(); i++)
				{
					if (JsonArray[i].IsString())
					{
						String Path = String(JsonArray[i].GetString());
						GameAssetsPaths.PushBack(Path);
						gConsole.LogInfo("CoreConfig: GameAssetsPaths element: {}", Path);
					}
					else
					{
						gConsole.LogInfo("CoreConfig: GameAssetsPaths element {} is not String", i);
					}
				}

				AssetsPaths[eResourceSource::GAME] = GameAssetsPaths;
			}
			else
			{
				gConsole.LogInfo("CoreConfig: Array GameAssetsPaths not found");
			}

		}

		LoadedFromFile = IsSuccessfull ? true : LoadedFromFile;

		return IsSuccessfull;
	}

	void AssetsPathConfig::LoadDefaults()
	{
		gConsole.LogInfo("CoreConfig::LoadDefaults() called");

		Dynarray<String> EngineAssetPaths;
		EngineAssetPaths.PushBack(DEFAULT_ENGINE_ASSETS_PATH);
		AssetsPaths[eResourceSource::ENGINE] = EngineAssetPaths;

		Dynarray<String> GameAssetPaths;
		GameAssetPaths.PushBack(DEFAULT_GAME_ASSETS_PATH);
		AssetsPaths[eResourceSource::GAME] = GameAssetPaths;
	}

	void AssetsPathConfig::WriteDefaultJson()
	{
		String JsonContent = String("{\"EngineAssetsPaths\":[") + DEFAULT_ENGINE_ASSETS_PATH + String("],\"GameAssetsPaths\":[") + DEFAULT_GAME_ASSETS_PATH + String("]}");
		SaveTextFile(ASSETS_PATH_CONFIG_PATH, JsonContent);
	}
}