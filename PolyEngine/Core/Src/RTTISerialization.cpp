#include "CorePCH.hpp"
#include "RTTISerialization.hpp"

using namespace Poly;

#define JSON_CLASS_NAME_TEXT rapidjson::StringRef("class_name")
#define JSON_NAME_TEXT rapidjson::StringRef("name")
#define JSON_PROPERTIES_TEXT rapidjson::StringRef("properties")
#define JSON_VALUE_TEXT rapidjson::StringRef("value")

void Poly::RTTI::SerializeObject(const RTTIBase* obj, const String& propertyName, rapidjson::Document& doc)
{
	auto& value = doc.SetObject();
	RTTI::SerializeObject(obj, propertyName, value, doc.GetAllocator());
}

void RTTI::SerializeObject(const RTTIBase* obj, const String& propertyName, rapidjson::Value& currentValue, rapidjson::Document::AllocatorType& alloc)
{
	const TypeInfo typeInfo = obj->GetTypeInfo();
	const PropertyManagerBase* propMgr = obj->GetPropertyManager();
	currentValue.SetObject();
	currentValue.AddMember(JSON_CLASS_NAME_TEXT, rapidjson::StringRef(typeInfo.GetTypeName()), alloc);
	currentValue.AddMember(JSON_NAME_TEXT, rapidjson::StringRef(propertyName.GetCStr()), alloc);
	currentValue.AddMember(JSON_PROPERTIES_TEXT, rapidjson::Value(rapidjson::kArrayType), alloc);
	rapidjson::Value& propList = currentValue[JSON_PROPERTIES_TEXT.s];

	for (auto& child : propMgr->GetPropertyList())
	{
		const void* ptr = ((const char*)obj) + child.Offset;
		if (child.CoreType == eCorePropertyType::NONE)
		{
			rapidjson::Value nestedValue;
			SerializeObject(reinterpret_cast<const RTTIBase*>(ptr), child.Name, nestedValue, alloc);
			propList.GetArray().PushBack(std::move(nestedValue), alloc);
		}
		else
			SerializeCoreProperty(ptr, child, propList, alloc);
	}
}

bool RTTI::SerializeCoreProperty(const void* value, const RTTI::Property& prop, rapidjson::Value& currentValue, rapidjson::Document::AllocatorType& alloc)
{
	const TypeInfo& typeInfo = prop.Type;

	rapidjson::Value nestedValue;
	nestedValue.SetObject();
	nestedValue.AddMember(JSON_CLASS_NAME_TEXT, rapidjson::StringRef(typeInfo.GetTypeName()), alloc);
	nestedValue.AddMember(JSON_NAME_TEXT, rapidjson::StringRef(prop.Name.GetCStr()), alloc);
	nestedValue.AddMember(JSON_VALUE_TEXT, GetCorePropertyValue(value, prop, alloc), alloc);
	currentValue.GetArray().PushBack(std::move(nestedValue), alloc);
	return true;
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