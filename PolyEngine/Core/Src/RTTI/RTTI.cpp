#include "CorePCH.hpp"
#include "RTTI/RTTI.hpp"

#include <RTTI/RTTISerialization.hpp>
#include <sstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

RTTI_DEFINE_TYPE(Poly::RTTIBase)

using namespace Poly;


Poly::RTTIBase::RTTIBase()
	: UUID(UniqueID::Generate())
{
	RTTIObjectsManager::Get().Register(this);
}

Poly::RTTIBase::~RTTIBase()
{
	RTTIObjectsManager::Get().Unregister(GetUUID());
}

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
	catch (const std::exception&)
	{
		gConsole.LogError("No file {} found for {}.", fileName, GetTypeInfo().GetTypeName());
		return;
	}

	rapidjson::Document DOMObject;
	DOMObject.Parse(json.GetCStr());
	RTTI::DeserializeObject(this, DOMObject);
}

RTTIObjectsManager& Poly::RTTIObjectsManager::Get()
{
	static RTTIObjectsManager instance;
	return instance;
}

void Poly::RTTIObjectsManager::Register(RTTIBase* obj)
{
	HEAVY_ASSERTE(obj, "Pointer to RTTIBase object is null!");
	DeserializedObjectsById.insert({ obj->GetUUID(), obj });
}

void Poly::RTTIObjectsManager::Unregister(const UniqueID& id)
{
	HEAVY_ASSERTE(id.IsValid(), "Pointer to RTTIBase object is null!");
	DeserializedObjectsById.erase(id);
}

RTTIBase* Poly::RTTIObjectsManager::TryGetObjectByID(const UniqueID& id)
{
	const auto it = DeserializedObjectsById.find(id);
	if (it == DeserializedObjectsById.end())
		return nullptr;
	return it->second;
}

RTTIBase* Poly::RTTIObjectsManager::GetObjectByID(const UniqueID& id)
{
	HEAVY_ASSERTE(id.IsValid(), "Invalid UUID!");
	return TryGetObjectByID(id);
}
