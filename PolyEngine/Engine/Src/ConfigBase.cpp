#include "EnginePCH.hpp"
#include "ConfigBase.hpp"

#include <RTTISerialization.hpp>
#include <sstream>
#include <rapidjson\stringbuffer.h>
#include <rapidjson\prettywriter.h>

RTTI_DEFINE_TYPE(Poly::ConfigBase)

Poly::ConfigBase::ConfigBase(const String& displayName, eConfigLocation location)
	: DisplayName(displayName), Location(location)
{
	FileName = GetTypeInfo().GetTypeName();
}

void Poly::ConfigBase::Save()
{
	rapidjson::Document DOMObject; // UTF8 by default
	auto& value = DOMObject.SetObject();
	RTTI::SerializeObject(this, DisplayName, value, DOMObject.GetAllocator());
	
	rapidjson::StringBuffer buffer;

	buffer.Clear();

	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	DOMObject.Accept(writer);
	
	gConsole.LogDebug("JSON:\n{}", buffer.GetString());
}

void Poly::ConfigBase::Load()
{
}
