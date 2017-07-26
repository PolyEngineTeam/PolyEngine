#include"EnginePCH.hpp"

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <cstdio>

using namespace rapidjson;
using namespace std;

namespace Poly
{
	CoreConfig gCoreConfig;

	Dynarray<String> CoreConfig::GetAssetsPaths(eResourceSource Source) const
	{
		HEAVY_ASSERTE(Source < eResourceSource::_COUNT, "_COUNT enum value passed to GetAssetsPaths(), which is an invalid value");
		return AssetsPaths[Source];
	}

	bool CoreConfig::IsLoadedFromFile() const
	{
		return LoadedFromFile;
	}

	bool CoreConfig::ReloadFromFile()
	{
		gConsole.LogInfo("CoreConfig::ReloadFromFile() called");
		bool IsSuccessfull = true;

		String ConfigFileContent;
		try
		{
			ConfigFileContent = LoadTextFile(CORE_CONFIG_PATH);
			// gConsole.LogInfo("File Content: {}", ConfigFileContent);
		}
		catch (FileIOException)
		{
			IsSuccessfull = false;
			gConsole.LogInfo("Config file reading failed: {}, using defaults", CORE_CONFIG_PATH);

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

		Document Json; // UTF8 by default
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
				const Value& JsonArray = Json["EngineAssetsPaths"];
				for (SizeType i = 0; i < JsonArray.Size(); i++)
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
				const Value& JsonArray = Json["GameAssetsPaths"];
				for (SizeType i = 0; i < JsonArray.Size(); i++)
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

	void CoreConfig::LoadDefaults()
	{
		gConsole.LogInfo("CoreConfig::LoadDefaults() called");

		Dynarray<String> EngineAssetPaths;
		EngineAssetPaths.PushBack(DEFAULT_ENGINE_ASSETS_PATH);
		AssetsPaths[eResourceSource::ENGINE] = EngineAssetPaths;

		Dynarray<String> GameAssetPaths;
		GameAssetPaths.PushBack(DEFAULT_GAME_ASSETS_PATH);
		AssetsPaths[eResourceSource::GAME] = GameAssetPaths;
	}

	void CoreConfig::WriteDefaultJson()
	{
		String JsonContent("{\"EngineAssetsPaths\":[\"../../Engine/Res/\"],\"GameAssetsPaths\":[\"../../Game/Res/\"]}");
		SaveTextFile(CORE_CONFIG_PATH, JsonContent);
	}
}