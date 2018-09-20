#include <CorePCH.hpp>

#include <RTTI/RTTISerialization.hpp>
#include <Collections/String.hpp>
#include <Collections/Dynarray.hpp>
#include <UniqueID.hpp>
#include <Math/Quaternion.hpp>
#include <Math/Vector.hpp>
#include <Math/Vector2f.hpp>
#include <Math/Vector2i.hpp>

using namespace Poly;

static const char* JSON_TYPE_ANNOTATION = "@type";

void ResolveUninitializedPointers(const Dynarray<RTTI::UninitializedPointerEntry>& uninitializedPointers)
{
	size_t initCount = 0;
	for (const auto& entry : uninitializedPointers)
	{
		HEAVY_ASSERTE(entry.Property.ImplData.get() != nullptr, "Invalid unique ptr impl data!");
		const Poly::RTTI::RawPtrPropertyImplDataBase* implData = static_cast<const Poly::RTTI::RawPtrPropertyImplDataBase*>(entry.Property.ImplData.get());
		implData->SetPtr(entry.Ptr, RTTIObjectsManager::Get().TryGetObjectByID(entry.UUID));
		if (implData->GetPtr(*entry.Ptr) != nullptr || !entry.UUID.IsValid())
			++initCount;
		else
			gConsole.LogError("Pointer [{}] of RTTIBase object with UUID: {} was not properly deserialized!", entry.Property.Name, entry.UUID);
	}
	ASSERTE(initCount == uninitializedPointers.GetSize(), "Not all raw pointers were initialized!");
}

void Poly::RTTI::SerializeObject(RTTIBase* obj, rapidjson::Document& doc)
{
	TraverseAndCall(obj, [](RTTIBase* obj) {obj->BeforeSerializationCallback(); });
	doc.SetObject();
	RTTI::SerializeObject(obj, doc.GetObject(), doc.GetAllocator());
	TraverseAndCall(obj, [](RTTIBase* obj) {obj->AfterSerializationCallback(); });
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
		rapidjson::Value GetValueFromFundamental(eCorePropertyType type, const void* value)
		{
			rapidjson::Value currentValue;

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

			return currentValue;
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

		template<typename T>
		rapidjson::Value GetVectorValueHelper(const RTTI::Property& prop, rapidjson::Document::AllocatorType& alloc, size_t count, const T* data)
		{
			HEAVY_ASSERTE(count >= 2, "Invalid vector size!");
			HEAVY_ASSERTE(data, "Invalid data pointer!");
			rapidjson::Value currentValue;
			currentValue.SetArray();
			
			eCorePropertyType type = GetCorePropertyType<T>();
			currentValue.GetArray().Reserve((rapidjson::SizeType)count, alloc);
			for(size_t i=0; i<count; ++i)
				currentValue.GetArray().PushBack(GetValueFromFundamental(type, data + i), alloc);
			
			return currentValue;
		}

		template<typename T>
		void SetVectorValueHelper(const RTTI::Property& prop, const rapidjson::Value& value, size_t count, T* data)
		{
			HEAVY_ASSERTE(count >= 2, "Invalid vector size!");
			HEAVY_ASSERTE(value.IsArray(), "Invalid value type");
			HEAVY_ASSERTE(value.GetArray().Size() >= 2, "Invalid array size");
			HEAVY_ASSERTE(value.GetArray().Size() == count, "Invalid array size compared to count");
			HEAVY_ASSERTE(data, "Invalid data pointer");
			
			eCorePropertyType type = GetCorePropertyType<T>();
			for (size_t i = 0; i < count && i < value.GetArray().Size(); ++i)
				SetFundamentalFromValue(value.GetArray()[(rapidjson::SizeType)i], type, data + i);
		}
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
		currentValue = GetValueFromFundamental(prop.CoreType, value);
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
	case eCorePropertyType::ENUM_ARRAY:
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
	{
		const Vector* vec = reinterpret_cast<const Vector*>(value);
		currentValue = GetVectorValueHelper(prop, alloc, vec->Data.size(), vec->Data.data());
	}
	break;
	case eCorePropertyType::VECTOR_2F:
	{
		const Vector2f* vec = reinterpret_cast<const Vector2f*>(value);
		currentValue = GetVectorValueHelper(prop, alloc, vec->Data.size(), vec->Data.data());
	}
	break;
	case eCorePropertyType::VECTOR_2I:
	{
		const Vector2i* vec = reinterpret_cast<const Vector2i*>(value);
		currentValue = GetVectorValueHelper(prop, alloc, vec->Data.size(), vec->Data.data());
	}
	break;
	case eCorePropertyType::QUATERNION:
	{
		const Quaternion* quat = reinterpret_cast<const Quaternion*>(value);
		currentValue = GetVectorValueHelper(prop, alloc, quat->Data.size(), quat->Data.data());
	}
	break;
	case eCorePropertyType::ANGLE:
	{
		const Angle* angle = reinterpret_cast<const Angle*>(value);
		auto tmp = angle->AsDegrees();
		currentValue = GetValueFromFundamental(GetCorePropertyType<decltype(tmp)>(), &tmp);
	}
	break;
	case eCorePropertyType::COLOR:
	{
		const Color* color = reinterpret_cast<const Color*>(value);
		currentValue = GetVectorValueHelper(prop, alloc, color->Data.size(), color->Data.data());
	}
	break;
	case eCorePropertyType::MATRIX:
	{
		const Matrix* mat = reinterpret_cast<const Matrix*>(value);
		currentValue = GetVectorValueHelper(prop, alloc, mat->Data.size(), mat->Data.data());
	}
	break;
	case eCorePropertyType::ENUM_FLAGS:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid ordered map impl data!");
		const EnumFlagsPropertyImplDataBase* implData = static_cast<const EnumFlagsPropertyImplDataBase*>(prop.ImplData.get());

		i64 val = implData->GetValue(value);
		currentValue.SetInt64(val);
		break;
	}
	break;
	case eCorePropertyType::UUID:
		currentValue.SetString(reinterpret_cast<const Poly::UniqueID*>(value)->ToString().GetCStr(), alloc);
		break;
	case eCorePropertyType::UNIQUE_PTR:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid unique ptr impl data!");
		const UniquePtrPropertyImplDataBase* implData = static_cast<const UniquePtrPropertyImplDataBase*>(prop.ImplData.get());
		if (implData->Get(value))
			currentValue = GetCorePropertyValue(implData->Get(value), implData->PropertyType, alloc);
		else
			currentValue.SetNull();
		break;
	}
	case eCorePropertyType::RAW_PTR:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid raw ptr impl data!");
		const RTTIBase* ptr = *reinterpret_cast<RTTIBase* const*>(value);

		if (ptr)
		{
			const UniqueID& uuid = ptr->GetUUID();
			currentValue.SetString(uuid.ToString().GetCStr(), alloc);
		}
		else
			currentValue.SetNull();
		
		break;
	}
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
	TraverseAndCall(obj, [](RTTIBase* obj) { obj->BeforeDeserializationCallback(); });
	Dynarray<UninitializedPointerEntry> uninitializedPointers;
	RTTI::DeserializeObject(obj, doc.GetObject(), uninitializedPointers);
	ResolveUninitializedPointers(uninitializedPointers);
	TraverseAndCall(obj, [](RTTIBase* obj) { obj->AfterDeserializationCallback(); });
}

CORE_DLLEXPORT void Poly::RTTI::DeserializeObject(RTTIBase* obj, 
	const rapidjson::GenericObject<true, rapidjson::Value> currentValue, 
	Dynarray<RTTI::UninitializedPointerEntry>& uninitializedPointers)
{
	UniqueID oldId = obj->GetUUID();

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
			SetCorePropertyValue(ptr, child, it->value, uninitializedPointers);
		}
	}

	RTTIObjectsManager::Get().FixMapingAfterDeserialization(obj, oldId);
}

CORE_DLLEXPORT void Poly::RTTI::SetCorePropertyValue(void* obj, 
	const RTTI::Property& prop, 
	const rapidjson::Value& value, 
	Dynarray<RTTI::UninitializedPointerEntry>& uninitializedPointers)
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
			SetCorePropertyValue(implData->GetValue(obj, i), implData->PropertyType, value.GetArray()[(rapidjson::SizeType)i], uninitializedPointers);
		}
	}
	break;
	case eCorePropertyType::ENUM_ARRAY:
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
			SetCorePropertyValue(implData->GetKeyTemporaryStorage(), implData->KeyPropertyType, key, uninitializedPointers);
			SetCorePropertyValue(implData->GetValueTemporaryStorage(), implData->ValuePropertyType, val, uninitializedPointers);
			implData->SetValue(obj, implData->GetKeyTemporaryStorage(), implData->GetValueTemporaryStorage());
		}
	}
	break;
	case eCorePropertyType::VECTOR:
	{
		Vector* vec = reinterpret_cast<Vector*>(obj);
		SetVectorValueHelper(prop, value, vec->Data.size(), vec->Data.data());
	}
	break;
	case eCorePropertyType::VECTOR_2F:
	{
		Vector2f* vec = reinterpret_cast<Vector2f*>(obj);
		SetVectorValueHelper(prop, value, vec->Data.size(), vec->Data.data());
	}
	break;
	case eCorePropertyType::VECTOR_2I:
	{
		Vector2i* vec = reinterpret_cast<Vector2i*>(obj);
		SetVectorValueHelper(prop, value, vec->Data.size(), vec->Data.data());
	}
	break;
	case eCorePropertyType::QUATERNION:
	{
		Quaternion* quat = reinterpret_cast<Quaternion*>(obj);
		SetVectorValueHelper(prop, value, quat->Data.size(), quat->Data.data());
	}
	break;
	case eCorePropertyType::ANGLE:
	{
		Angle* angle = reinterpret_cast<Angle*>(obj);
		using ValueType = decltype(angle->AsDegrees());
		ValueType degrees;
		SetFundamentalFromValue(value, GetCorePropertyType<ValueType>(), &degrees);
		*angle = Angle::FromDegrees(degrees);
	}
	break;
	case eCorePropertyType::COLOR:
	{
		Color* color = reinterpret_cast<Color*>(obj);
		SetVectorValueHelper(prop, value, color->Data.size(), color->Data.data());
	}
	break;
	case eCorePropertyType::MATRIX:
	{
		Matrix* mat = reinterpret_cast<Matrix*>(obj);
		SetVectorValueHelper(prop, value, mat->Data.size(), mat->Data.data());
	}
	break;
	case eCorePropertyType::ENUM_FLAGS:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid enum impl data!");
		const EnumFlagsPropertyImplDataBase* implData = static_cast<const EnumFlagsPropertyImplDataBase*>(prop.ImplData.get());
		const i64 val = value.GetInt64();
		implData->SetValue(obj, val);
	}
	break;
	case eCorePropertyType::UUID:
	{
		Poly::UniqueID& uuid = *reinterpret_cast<Poly::UniqueID*>(obj);
		uuid = UniqueID::FromString(String(value.GetString())).Value();
		HEAVY_ASSERTE(uuid.IsValid(), "UniqueID deserialization failed");
	}
	break;
	case eCorePropertyType::UNIQUE_PTR:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid unique ptr impl data!");
		const UniquePtrPropertyImplDataBase* implData = static_cast<const UniquePtrPropertyImplDataBase*>(prop.ImplData.get());
		
		if (value.IsNull())
		{
			implData->Clear(obj);
			break;
		}

		if (implData->PropertyType.Type.IsValid()) // RTTI type
		{
			String typeName;
			if (implData->PropertyType.CoreType == eCorePropertyType::CUSTOM)
				typeName = value.GetObject().FindMember(JSON_TYPE_ANNOTATION)->value.GetString();
			else
				typeName = implData->PropertyType.Type.GetTypeName();
			implData->CreatePolymorphic(obj, typeName.GetCStr());
		}
		else // Fundamental
			implData->Create(obj);
		
		SetCorePropertyValue(implData->Get(obj), implData->PropertyType, value, uninitializedPointers);
	}
	break;
	case eCorePropertyType::RAW_PTR:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid unique ptr impl data!");
		UninitializedPointerEntry entry;
		entry.UUID = value.IsNull() ? UniqueID::INVALID : UniqueID::FromString(String(value.GetString())).Value();
		HEAVY_ASSERTE(value.IsNull() || entry.UUID, "UniqueID deserialization failed");
		entry.Ptr = (RTTIBase**)obj;
		entry.Property = prop;
		uninitializedPointers.PushBack(std::move(entry));
	}
	break;
	case eCorePropertyType::CUSTOM:
		DeserializeObject(reinterpret_cast<RTTIBase*>(obj), value.GetObject(), uninitializedPointers);
		break;
	default:
		ASSERTE(false, "Unknown property type!");
	}
}

CORE_DLLEXPORT void Poly::RTTI::TraverseAndCall(RTTIBase* obj, const std::function<void(RTTIBase*)>& func)
{
	const PropertyManagerBase* propMgr = obj->GetPropertyManager();

	for (auto& child : propMgr->GetPropertyList())
	{
		ASSERTE(child.CoreType != eCorePropertyType::UNHANDLED, "Invalid type in property declaration!");
		if (child.Flags.IsSet(ePropertyFlag::DONT_SERIALIZE) || child.CoreType != eCorePropertyType::CUSTOM)
			continue;
		RTTIBase* ptr = reinterpret_cast<RTTIBase*>(((char*)obj) + child.Offset);
		TraverseAndCall(ptr, func);
	}
	func(obj);
}
