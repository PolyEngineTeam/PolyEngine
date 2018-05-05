#include "CorePCH.hpp"
#include "RTTISerialization.hpp"
#include "Collections/String.hpp"
#include "Collections/Dynarray.hpp"

using namespace Poly;

static const char* JSON_TYPE_ANNOTATION = "@type";

void Poly::RTTI::SerializeObject(const RTTIBase* obj, rapidjson::Document& doc)
{
	doc.SetObject();
	RTTI::SerializeObject(obj, doc.GetObject(), doc.GetAllocator());
}

void RTTI::SerializeObject(const RTTIBase* obj, rapidjson::GenericObject<false, rapidjson::Value> currentValue, rapidjson::Document::AllocatorType& alloc)
{
	const TypeInfo typeInfo = obj->GetTypeInfo();
	const PropertyManagerBase* propMgr = obj->GetPropertyManager();

	currentValue.AddMember(rapidjson::StringRef(JSON_TYPE_ANNOTATION), rapidjson::StringRef(typeInfo.GetTypeName()), alloc);

	for (auto& child : propMgr->GetPropertyList())
	{
		ASSERTE(child.CoreType != eCorePropertyType::UNHANDLED, "Invalid type in property declaration!");
		if (child.Flags.IsSet(ePropertyFlag::DONT_SERIALIZE))
			continue;

		const void* ptr = ((const char*)obj) + child.Offset;
		currentValue.AddMember(rapidjson::StringRef(child.Name.GetCStr()), GetCorePropertyValue(ptr, child, alloc), alloc);
	}
}

namespace Poly 
{
	namespace RTTI
	{
		void SetValueFromFundamental(rapidjson::Value& currentValue, eCorePropertyType type, const void* value)
		{
			switch (type)
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
			default:
				ASSERTE(false, "Invalid type");
			}
		}

		void SetFundamentalFromValue(const rapidjson::Value& value, eCorePropertyType type, void* obj)
		{
			switch (type)
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
			default:
				ASSERTE(false, "Invalid type");
			}
		}

		//TODO implement
		/*template<typename T>
		rapidjson::Value GetVectorValueHelper(const RTTI::Property& prop, rapidjson::Document::AllocatorType& alloc, const T* x, const T* y, const T* z, const T* w)
		{
			HEAVY_ASSERTE(count > 1 && count <= 2, "Invalid vector size!");
			rapidjson::Value currentValue;
			currentValue.SetObject();
			eCorePropertyType type = GetCorePropertyType<T>();
			currentValue.GetObject().AddMember("x", x, alloc);
			currentValue.GetObject().AddMember("y", y, alloc);
			if (count >= 3) currentValue.GetObject().AddMember("z", z, alloc);
			if (count >= 4) currentValue.GetObject().AddMember("w", w, alloc);
			return currentValue;
		}

		template<typename T>
		void SetVectorValueHelper(const RTTI::Property& prop, const rapidjson::Value& value, T* x, T* y, T* z = nullptr, T* w = nullptr)
		{
			HEAVY_ASSERTE(count > 1 && count <= 2, "Invalid vector size!");
			rapidjson::Value currentValue;
			currentValue.SetObject();
			currentValue.GetObject().AddMember("x", x, alloc);
			currentValue.GetObject().AddMember("y", y, alloc);
			if (z) currentValue.GetObject().AddMember("z", z, alloc);
			if (w) currentValue.GetObject().AddMember("w", w, alloc);
			return currentValue;
		}*/
	}
}


rapidjson::Value RTTI::GetCorePropertyValue(const void* value, const RTTI::Property& prop, rapidjson::Document::AllocatorType& alloc)
{
	rapidjson::Value currentValue;

	switch (prop.CoreType)
	{
	case eCorePropertyType::BOOL:
	case eCorePropertyType::INT8:
	case eCorePropertyType::INT16:
	case eCorePropertyType::INT32:
	case eCorePropertyType::INT64:
	case eCorePropertyType::UINT8:
	case eCorePropertyType::UINT16:
	case eCorePropertyType::UINT32:
	case eCorePropertyType::UINT64:
	case eCorePropertyType::FLOAT:
	case eCorePropertyType::DOUBLE:
		SetValueFromFundamental(currentValue, prop.CoreType, value);
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
	case eCorePropertyType::DYNARRAY:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid dynarray impl data!");
		const CollectionPropertyImplDataBase* implData = static_cast<const CollectionPropertyImplDataBase*>(prop.ImplData.get());
		currentValue.SetArray();
		for (size_t i = 0; i < implData->GetSize(value); ++i)
			currentValue.GetArray().PushBack(GetCorePropertyValue(implData->GetValue(value, i), implData->PropertyType, alloc), alloc);
	}
		break;
	case eCorePropertyType::ORDERED_MAP:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid ordered map impl data!");
		const DictionaryPropertyImplDataBase* implData = static_cast<const DictionaryPropertyImplDataBase*>(prop.ImplData.get());
		currentValue.SetArray();
		for (const void* key : implData->GetKeys(value))
		{
			rapidjson::Value keyObj = GetCorePropertyValue(key, implData->KeyPropertyType, alloc);
			rapidjson::Value valObj = GetCorePropertyValue(implData->GetValue(value, key), implData->ValuePropertyType, alloc);
			rapidjson::Value pair;
			pair.SetObject();
			pair.GetObject().AddMember("Key", keyObj, alloc);
			pair.GetObject().AddMember("Value", valObj, alloc);
			currentValue.GetArray().PushBack(pair, alloc);
		}
	}
	break;
	case eCorePropertyType::VECTOR:
	case eCorePropertyType::VECTOR_2F:
	case eCorePropertyType::VECTOR_2I:
	case eCorePropertyType::QUATERNION:
	case eCorePropertyType::ANGLE:
	case eCorePropertyType::COLOR:
	case eCorePropertyType::MATRIX:
		//TODO implement
		break;
	case eCorePropertyType::CUSTOM:
		currentValue.SetObject();
		SerializeObject(reinterpret_cast<const RTTIBase*>(value), currentValue.GetObject(), alloc);
		break;
	default:
		ASSERTE(false, "Unknown property type!");
	}

	return currentValue;
}

CORE_DLLEXPORT void Poly::RTTI::DeserializeObject(RTTIBase* obj, const rapidjson::Document& doc)
{
	RTTI::DeserializeObject(obj, doc.GetObject());
}

CORE_DLLEXPORT void Poly::RTTI::DeserializeObject(RTTIBase* obj, const rapidjson::GenericObject<true, rapidjson::Value> currentValue)
{
	const PropertyManagerBase* propMgr = obj->GetPropertyManager();

	for (auto& child : propMgr->GetPropertyList())
	{
		ASSERTE(child.CoreType != eCorePropertyType::UNHANDLED, "Invalid type in property declaration!");
		if (child.Flags.IsSet(ePropertyFlag::DONT_SERIALIZE))
			continue;

		void* ptr = ((char*)obj) + child.Offset;
		const auto& it = currentValue.FindMember(child.Name.GetCStr());
		if (it != currentValue.MemberEnd())
		{
			SetCorePropertyValue(ptr, child, it->value);
		}
	}
}

CORE_DLLEXPORT void Poly::RTTI::SetCorePropertyValue(void* obj, const RTTI::Property& prop, const rapidjson::Value& value)
{
	switch (prop.CoreType)
	{
	case eCorePropertyType::BOOL:
	case eCorePropertyType::INT8:
	case eCorePropertyType::INT16:
	case eCorePropertyType::INT32:
	case eCorePropertyType::INT64:
	case eCorePropertyType::UINT8:
	case eCorePropertyType::UINT16:
	case eCorePropertyType::UINT32:
	case eCorePropertyType::UINT64:
	case eCorePropertyType::FLOAT:
	case eCorePropertyType::DOUBLE:
		SetFundamentalFromValue(value, prop.CoreType, obj);
		break;
	case eCorePropertyType::STRING:
	{
		*reinterpret_cast<String*>(obj) = String(value.GetString());
		HEAVY_ASSERTE(reinterpret_cast<String*>(obj)->GetLength() == value.GetStringLength(), "String deserialization failed");
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
			ASSERTE(false, "Unhandled value size!");
		break;
	}
	case eCorePropertyType::DYNARRAY:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid enum impl data!");
		const CollectionPropertyImplDataBase* implData = static_cast<const CollectionPropertyImplDataBase*>(prop.ImplData.get());
		implData->Resize(obj, value.GetArray().Size());
		for (size_t i = 0; i < value.GetArray().Size(); ++i)
		{
			SetCorePropertyValue(implData->GetValue(obj, i), implData->PropertyType, value.GetArray()[(rapidjson::SizeType)i]);
		}
	}
	break;
	case eCorePropertyType::ORDERED_MAP:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid ordered map impl data!");
		const DictionaryPropertyImplDataBase* implData = static_cast<const DictionaryPropertyImplDataBase*>(prop.ImplData.get());
		implData->Clear(obj);
		for (size_t i = 0; i < value.GetArray().Size(); ++i)
		{
			const rapidjson::Value& pair = value.GetArray()[(rapidjson::SizeType)i];
			const auto& key = pair.GetObject().FindMember("Key")->value;
			const auto& val = pair.GetObject().FindMember("Value")->value;
			SetCorePropertyValue(implData->GetKeyTemporaryStorage(), implData->KeyPropertyType, key);
			SetCorePropertyValue(implData->GetValueTemporaryStorage(), implData->ValuePropertyType, val);
			implData->SetValue(obj, implData->GetKeyTemporaryStorage(), implData->GetValueTemporaryStorage());
		}
	}
	break;
	case eCorePropertyType::VECTOR:
	case eCorePropertyType::VECTOR_2F:
	case eCorePropertyType::VECTOR_2I:
	case eCorePropertyType::QUATERNION:
	case eCorePropertyType::ANGLE:
	case eCorePropertyType::COLOR:
	case eCorePropertyType::MATRIX:
		//TODO implement
		break;
	case eCorePropertyType::CUSTOM:
		DeserializeObject(reinterpret_cast<RTTIBase*>(obj), value.GetObject());
		break;
	default:
		ASSERTE(false, "Unknown property type!");
	}
}
