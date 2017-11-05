#include "CorePCH.hpp"
#include "RTTISerialization.hpp"

using namespace Poly;

static const char* JSON_TYPE_ANNOTATION = "@type";

void Poly::RTTI::SerializeObject(const RTTIBase* obj, const String& propertyName, rapidjson::Document& doc)
{
	auto& value = doc.SetObject();
	RTTI::SerializeObject(obj, propertyName, value, doc.GetAllocator());
}

void RTTI::SerializeObject(const RTTIBase* obj, const String& propertyName, rapidjson::Value& currentValue, rapidjson::Document::AllocatorType& alloc)
{
	const TypeInfo typeInfo = obj->GetTypeInfo();
	const PropertyManagerBase* propMgr = obj->GetPropertyManager();

	HEAVY_ASSERTE(currentValue.IsObject(), "JSON value is not an object!");
	currentValue.AddMember(rapidjson::StringRef(propertyName.GetCStr()), rapidjson::Value(rapidjson::kObjectType), alloc);
	rapidjson::Value& object = currentValue[propertyName.GetCStr()];

	object.AddMember(rapidjson::StringRef(JSON_TYPE_ANNOTATION), rapidjson::StringRef(typeInfo.GetTypeName()), alloc);

	for (auto& child : propMgr->GetPropertyList())
	{
		ASSERTE(child.CoreType != eCorePropertyType::UNHANDLED, "Invalid type in property declaration!");
		if (child.Flags.IsSet(ePropertyFlag::DONT_SERIALIZE))
			continue;

		const void* ptr = ((const char*)obj) + child.Offset;
		if (child.CoreType == eCorePropertyType::NONE)
			SerializeObject(reinterpret_cast<const RTTIBase*>(ptr), child.Name, object, alloc);
		else
			object.AddMember(rapidjson::StringRef(child.Name.GetCStr()), GetCorePropertyValue(ptr, child, alloc), alloc);
	}
}

rapidjson::Value RTTI::GetCorePropertyValue(const void* value, const RTTI::Property& prop, rapidjson::Document::AllocatorType& alloc)
{
	rapidjson::Value currentValue;

	switch (prop.CoreType)
	{
	case eCorePropertyType::BOOL:
		{
			bool val = *reinterpret_cast<const bool*>(value);
			currentValue.SetBool(val);
		}
		break;
	case eCorePropertyType::NONE:
		ASSERTE(false, "Invalid property type!");
		break;
	default:
		ASSERTE(false, "Unknown property type!");
	}

	return currentValue;
}

CORE_DLLEXPORT void Poly::RTTI::DeserializeObject(RTTIBase* obj, const String& propertyName, const rapidjson::Document& doc)
{
	auto& it = doc.GetObject().FindMember(propertyName.GetCStr());
	if(it != doc.GetObject().MemberEnd())
		RTTI::DeserializeObject(obj, propertyName, doc.GetObject().MemberBegin()->value);
}

CORE_DLLEXPORT void Poly::RTTI::DeserializeObject(RTTIBase* obj, const String& propertyName, const rapidjson::Value& currentValue)
{
	const TypeInfo typeInfo = obj->GetTypeInfo();
	const PropertyManagerBase* propMgr = obj->GetPropertyManager();

	HEAVY_ASSERTE(currentValue.IsObject(), "JSON value is not an object!");

	for (auto& child : propMgr->GetPropertyList())
	{
		ASSERTE(child.CoreType != eCorePropertyType::UNHANDLED, "Invalid type in property declaration!");
		if (child.Flags.IsSet(ePropertyFlag::DONT_SERIALIZE))
			continue;

		void* ptr = ((char*)obj) + child.Offset;
		auto& it = currentValue.FindMember(child.Name.GetCStr());
		if (it != currentValue.MemberEnd())
		{
			if (child.CoreType == eCorePropertyType::NONE)
				DeserializeObject(reinterpret_cast<RTTIBase*>(ptr), child.Name, it->value);
			else
				SetCorePropertyValue(ptr, child, it->value);
		}
	}
}

CORE_DLLEXPORT void Poly::RTTI::SetCorePropertyValue(void* obj, const RTTI::Property& prop, const rapidjson::Value& value)
{
	switch (prop.CoreType)
	{
	case eCorePropertyType::BOOL:
	{
		bool& val = *reinterpret_cast<bool*>(obj);
		val = value.GetBool();
	}
	break;
	case eCorePropertyType::NONE:
		ASSERTE(false, "Invalid property type!");
		break;
	default:
		ASSERTE(false, "Unknown property type!");
	}
}
