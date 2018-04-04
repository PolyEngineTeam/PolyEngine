#include "CorePCH.hpp"
#include "RTTI/RTTI.hpp"

#include <RTTI/RTTISerialization.hpp>
#include <sstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

RTTI_DEFINE_PRIMITIVE_TYPE(bool)
RTTI_DEFINE_PRIMITIVE_TYPE(i8)
RTTI_DEFINE_PRIMITIVE_TYPE(i16)
RTTI_DEFINE_PRIMITIVE_TYPE(i32)
RTTI_DEFINE_PRIMITIVE_TYPE(i64)
RTTI_DEFINE_PRIMITIVE_TYPE(u8)
RTTI_DEFINE_PRIMITIVE_TYPE(u16)
RTTI_DEFINE_PRIMITIVE_TYPE(u32)
RTTI_DEFINE_PRIMITIVE_TYPE(u64)
RTTI_DEFINE_PRIMITIVE_TYPE(f32)
RTTI_DEFINE_PRIMITIVE_TYPE(f64)

//RTTI_DEFINE_PRIMITIVE_TYPE(int)
//RTTI_DEFINE_PRIMITIVE_TYPE(uint)

RTTI_DEFINE_TYPE(Poly::RTTIBase)

using namespace Poly;

static String ROOT_NAME = "root";

void RTTIBase::SerializeToFile(const String& fileName, eSerializationType type)
{
	rapidjson::Document DOMObject; // UTF8 by default
	RTTI::SerializeObject(this, ROOT_NAME, DOMObject);

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
	RTTI::DeserializeObject(this, ROOT_NAME, DOMObject);
}