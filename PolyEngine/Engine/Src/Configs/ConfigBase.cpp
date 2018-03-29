#include "EnginePCH.hpp"
#include "ConfigBase.hpp"

#include <RTTI/RTTISerialization.hpp>
#include <sstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

using namespace Poly;

RTTI_DEFINE_TYPE(Poly::ConfigBase)

ConfigBase::ConfigBase(const String& displayName, eResourceSource location)
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
	
	//gConsole.LogDebug("{} json:\n{}", DisplayName, buffer.GetString());
	SaveTextFileRelative(Location, GetFileName(), String(buffer.GetString()));
}

void ConfigBase::Load()
{
	String json;
	try
	{
		json = LoadTextFileRelative(Location, GetFileName());
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
		if(Location == eResourceSource::NONE)
			FileName = String(GetTypeInfo().GetTypeName()) + String(".json");
		else
			FileName = String("Configs/") + String(GetTypeInfo().GetTypeName()) + String(".json");
	}
		
	return FileName;
}
