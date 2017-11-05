#include "EnginePCH.hpp"
#include "ConfigBase.hpp"

#include <RTTISerialization.hpp>
#include <sstream>
#include <rapidjson\stringbuffer.h>
#include <rapidjson\prettywriter.h>

using namespace Poly;

RTTI_DEFINE_TYPE(Poly::ConfigBase)

ConfigBase::ConfigBase(const String& displayName, eConfigLocation location)
	: DisplayName(displayName), Location(location)
{
}

//TODO correct config location!

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
	SaveTextFile(GetFileName(), String(buffer.GetString()));
}

void ConfigBase::Load()
{
	String json;
	try
	{
		json = LoadTextFile(GetFileName());
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
	Save(); // For now, ensure newest state of config file.
}

const String& ConfigBase::GetFileName() const
{
	if (FileName.GetLength() == 0)
		FileName = String(GetTypeInfo().GetTypeName()) + String(".json");
	return FileName;
}
