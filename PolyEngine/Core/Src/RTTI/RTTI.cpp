#include "CorePCH.hpp"
#include "RTTI/RTTI.hpp"

#include <RTTI/RTTISerialization.hpp>
#include <sstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

RTTI_DEFINE_TYPE(Poly::RTTIBase)

using namespace Poly;


void RTTIBase::SerializeToFile(const String& fileName, eSerializationType type)
{
	rapidjson::Document DOMObject; // UTF8 by default
	RTTI::SerializeObject(this, DOMObject);

	// Save to pretty string
	rapidjson::StringBuffer buffer;
	buffer.Clear();
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	DOMObject.Accept(writer);

	SaveTextFile(fileName, String(buffer.GetString()));
}

void RTTIBase::DeserializeFromFile(const String& fileName, eSerializationType type)
{
	String json;
	try
	{
		json = LoadTextFile(fileName);
	}
	catch (std::exception)
	{
		gConsole.LogError("No file {} found for {}.", fileName, GetTypeInfo().GetTypeName());
		return;
	}

	rapidjson::Document DOMObject;
	DOMObject.Parse(json.GetCStr());
	RTTI::DeserializeObject(this, DOMObject);
}