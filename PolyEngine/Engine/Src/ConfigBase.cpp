#include "EnginePCH.hpp"
#include "ConfigBase.hpp"

#include <RTTISerialization.hpp>
#include <sstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

using namespace Poly;

RTTI_DEFINE_TYPE(Poly::ConfigBase)

ConfigBase::ConfigBase(const String& displayName, eConfigLocation location)
	: DisplayName(displayName), Location(location)
{
}

void ConfigBase::Save()
{
	rapidjson::Document DOMObject; // UTF8 by default
	RTTI::SerializeObject(this, DisplayName, DOMObject);
	
	// Save to pretty string
	rapidjson::StringBuffer buffer;
	buffer.Clear();
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	DOMObject.Accept(writer);
	
	gConsole.LogDebug("{} json:\n{}", DisplayName, buffer.GetString());
	switch (Location)
	{
	case Poly::eConfigLocation::ENGINE:
		SaveTextFileRelative(eResourceSource::ENGINE, GetFileName(), String(buffer.GetString()));
		break;
	case Poly::eConfigLocation::GAME:
		SaveTextFileRelative(eResourceSource::GAME, GetFileName(), String(buffer.GetString()));
		break;
	case Poly::eConfigLocation::LOCAL: // local for now is similar to RUNTIME_DIR
	case Poly::eConfigLocation::RUNTIME_DIR:
		SaveTextFile(GetFileName(), String(buffer.GetString()));
		break;
	default:
		ASSERTE(false, "Invalid location");
	}
}

void ConfigBase::Load()
{
	String json;
	try
	{
		switch (Location)
		{
		case Poly::eConfigLocation::ENGINE:
			json = LoadTextFileRelative(eResourceSource::ENGINE, GetFileName());
			break;
		case Poly::eConfigLocation::GAME:
			json = LoadTextFileRelative(eResourceSource::GAME, GetFileName());
			break;
		case Poly::eConfigLocation::LOCAL: // local for now is similar to RUNTIME_DIR
		case Poly::eConfigLocation::RUNTIME_DIR:
			json = LoadTextFile(GetFileName());
			break;
		default:
			ASSERTE(false, "Invalid location");
		}
	} 
	catch (std::exception)
	{
		gConsole.LogWarning("No configuration file found for {}. Using default values.", DisplayName);
		Save(); // Create file
		return;
	}
		
	rapidjson::Document DOMObject;
	DOMObject.Parse(json.GetCStr());
	RTTI::DeserializeObject(this, DisplayName, DOMObject);

	// For now, ensure newest state of config file.
	Save(); 
}

const String& ConfigBase::GetFileName() const
{
	if (FileName.GetLength() == 0)
	{
		if(Location == eConfigLocation::RUNTIME_DIR)
			FileName = String(GetTypeInfo().GetTypeName()) + String(".json");
		else
			FileName = String("Configs/") + String(GetTypeInfo().GetTypeName()) + String(".json");
	}
		
	return FileName;
}
