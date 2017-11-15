#include "CorePCH.hpp"
#include "RTTISerialization.hpp"
#include "String.hpp"

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
		currentValue.SetBool(*reinterpret_cast<const bool*>(value));
		break;
	case eCorePropertyType::INT8:
		currentValue.SetInt(*reinterpret_cast<const i8*>(value));
		break;
	case eCorePropertyType::INT16:
		currentValue.SetInt(*reinterpret_cast<const i16*>(value));
		break;
	case eCorePropertyType::INT32:
		currentValue.SetInt(*reinterpret_cast<const i32*>(value));
		break;
	case eCorePropertyType::INT64:
		currentValue.SetInt64(*reinterpret_cast<const i64*>(value));
		break;
	case eCorePropertyType::UINT8:
		currentValue.SetUint(*reinterpret_cast<const u8*>(value));
		break;
	case eCorePropertyType::UINT16:
		currentValue.SetUint(*reinterpret_cast<const u16*>(value));
		break;
	case eCorePropertyType::UINT32:
		currentValue.SetUint(*reinterpret_cast<const u32*>(value));
		break;
	case eCorePropertyType::UINT64:
		currentValue.SetUint64(*reinterpret_cast<const u64*>(value));
		break;
	case eCorePropertyType::FLOAT:
		currentValue.SetFloat(*reinterpret_cast<const float*>(value));
		break;
	case eCorePropertyType::DOUBLE:
		currentValue.SetDouble(*reinterpret_cast<const double*>(value));
		break;
	case eCorePropertyType::STRING:
	{
		const String* str = reinterpret_cast<const String*>(value);
		currentValue.SetString(str->GetCStr(), alloc);
		break;
	}
	case eCorePropertyType::ENUM:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid enum impl data!");
		const EnumPropertyImplData* implData = static_cast<const EnumPropertyImplData*>(prop.ImplData.get());
		i64 val;
		if (implData->EnumInfo->GetUnderlyingValueSize() == sizeof(i32))
			val = *reinterpret_cast<const i32*>(value);
		else if (implData->EnumInfo->GetUnderlyingValueSize() == sizeof(i64))
			val = *reinterpret_cast<const i64*>(value);
		else
			ASSERTE(false, "Unhadled value size!");

		currentValue.SetString(implData->EnumInfo->GetEnumName(val), alloc);
		break;
	}
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
	const auto& it = doc.GetObject().FindMember(propertyName.GetCStr());
	if(it != doc.GetObject().MemberEnd())
		RTTI::DeserializeObject(obj, propertyName, it->value);
}

CORE_DLLEXPORT void Poly::RTTI::DeserializeObject(RTTIBase* obj, const String& propertyName, const rapidjson::Value& currentValue)
{
	UNUSED(propertyName);
	const PropertyManagerBase* propMgr = obj->GetPropertyManager();

	HEAVY_ASSERTE(currentValue.IsObject(), "JSON value is not an object!");

	for (auto& child : propMgr->GetPropertyList())
	{
		ASSERTE(child.CoreType != eCorePropertyType::UNHANDLED, "Invalid type in property declaration!");
		if (child.Flags.IsSet(ePropertyFlag::DONT_SERIALIZE))
			continue;

		void* ptr = ((char*)obj) + child.Offset;
		const auto& it = currentValue.FindMember(child.Name.GetCStr());
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
		*reinterpret_cast<bool*>(obj) = value.GetBool();
		break;
	case eCorePropertyType::INT8:
	{
		int result = value.GetInt();
		HEAVY_ASSERTE((result < 0) ? ((-result & 0xFF) == -result) : ((result & 0xFF) == result), "Value outside of int8 range");
		*reinterpret_cast<i8*>(obj) = (i8)result;
		break;
	}
	case eCorePropertyType::INT16:
	{
		int result = value.GetInt();
		HEAVY_ASSERTE((result < 0) ? ((-result & 0xFFFF) == -result) : ((result & 0xFFFF) == result), "Value outside of int16 range");
		*reinterpret_cast<i16*>(obj) = (i16)result;
		break;
	}
	case eCorePropertyType::INT32:
		*reinterpret_cast<i32*>(obj) = value.GetInt();
		break;
	case eCorePropertyType::INT64:
		*reinterpret_cast<i64*>(obj) = value.GetInt64();
		break;
	case eCorePropertyType::UINT8:
	{
		uint result = value.GetUint();
		HEAVY_ASSERTE((result & 0xFF) == result, "Value outside of uint8 range");
		*reinterpret_cast<u8*>(obj) = (u8)result;
		break;
	}
	case eCorePropertyType::UINT16:
	{
		uint result = value.GetUint();
		HEAVY_ASSERTE((result & 0xFFFF) == result, "Value outside of uint16 range");
		*reinterpret_cast<u16*>(obj) = (u16)result;
		break;
	}
	case eCorePropertyType::UINT32:
		*reinterpret_cast<u32*>(obj) = value.GetUint();
		break;
	case eCorePropertyType::UINT64:
		*reinterpret_cast<u64*>(obj) = value.GetUint64();
		break;
	case eCorePropertyType::FLOAT:
		*reinterpret_cast<float*>(obj) = value.GetFloat();
		break;
	case eCorePropertyType::DOUBLE:
		*reinterpret_cast<double*>(obj) = value.GetDouble();
		break;
	case eCorePropertyType::STRING:
	{
		String* str = reinterpret_cast<String*>(obj);
		*str = String(value.GetString());
		HEAVY_ASSERTE(str->GetLength() == value.GetStringLength(), "String deserialization failed");
		break;
	}
	case eCorePropertyType::ENUM:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid enum impl data!");
		const EnumPropertyImplData* implData = static_cast<const EnumPropertyImplData*>(prop.ImplData.get());

		const i64 val = implData->EnumInfo->GetEnumValue(value.GetString());

		if (implData->EnumInfo->GetUnderlyingValueSize() == sizeof(i32))
			*reinterpret_cast<i32*>(obj) = (i32)val;
		else if (implData->EnumInfo->GetUnderlyingValueSize() == sizeof(i64))
			*reinterpret_cast<i64*>(obj) = val;
		else
			ASSERTE(false, "Unhadled value size!");
		break;
	}
	case eCorePropertyType::NONE:
		ASSERTE(false, "Invalid property type!");
		break;
	default:
		ASSERTE(false, "Unknown property type!");
	}
}
