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
		return AssetsPaths.at(Source);
	}

	bool CoreConfig::GetIsLoadedFromFile() const
	{
		return IsLoadedFromFile;
	}

	bool CoreConfig::ReloadFromFile()
	{
		gConsole.LogInfo("CoreConfig::ReloadFromFile() called");
		bool IsSuccessfull = true;

		static const String CORE_CONFIG_PATH("CoreConfig.json");

		String ConfigFileContent;
		try
		{
			ConfigFileContent = LoadTextFile(CORE_CONFIG_PATH);
			// gConsole.LogInfo("File Content: {}", ConfigFileContent);
		}
		catch (FileIOException)
		{
			IsSuccessfull = false;
			gConsole.LogInfo("Config file reading failed: {}", CORE_CONFIG_PATH);
		}

		Document Json; // UTF8 by default
		if (Json.Parse(ConfigFileContent.GetCStr()).HasParseError()) 
		{
			IsSuccessfull = false;
			gConsole.LogInfo("Config file json has error: {}", Json.GetParseError());
		}
		else
		{
			// validate
			HEAVY_ASSERTE(Json.IsObject(), "Json root must be Object");

			// read properties
			if (Json.HasMember("DebugNormalsFlag") && Json["DebugNormalsFlag"].IsBool())
			{
				DebugNormalsFlag = Json["DebugNormalsFlag"].GetBool();
				gConsole.LogInfo("CoreConfig: bool DebugNormalsFlag found: {}", DebugNormalsFlag);
			}
			else 
			{
				gConsole.LogInfo("CoreConfig: bool DebugNormalsFlag not found");
			}

			if (Json.HasMember("WireframeRendering") && Json["WireframeRendering"].IsBool())
			{
				WireframeRendering = Json["WireframeRendering"].GetBool();
				gConsole.LogInfo("CoreConfig: bool WireframeRendering found: {}", WireframeRendering);
			}
			else
			{
				gConsole.LogInfo("CoreConfig: bool WireframeRendering not found");
			}

			if (Json.HasMember("DisplayFPS") && Json["DisplayFPS"].IsBool())
			{
				DisplayFPS = Json["DisplayFPS"].GetBool();
				gConsole.LogInfo("CoreConfig: bool DisplayFPS found: {}", WireframeRendering);
			}
			else
			{
				gConsole.LogInfo("CoreConfig: bool DisplayFPS not found");
			}


			// read assets paths
			AssetsPaths.clear();

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

				AssetsPaths[eResourceSource::ENGINE] = std::move(EngineAssetPaths);
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

				AssetsPaths[eResourceSource::GAME] = std::move(GameAssetsPaths);
			}
			else
			{
				gConsole.LogInfo("CoreConfig: Array GameAssetsPaths not found");
			}
		}

		IsLoadedFromFile = IsSuccessfull ? true : IsLoadedFromFile;

		return IsSuccessfull;
	}
}