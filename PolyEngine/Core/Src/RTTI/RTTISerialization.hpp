#pragma once

#include "RTTI/RTTI.hpp"
#include "RTTIProperty.hpp"
#include <rapidjson/document.h>

namespace Poly
{
	namespace RTTI
	{
		struct CORE_DLLEXPORT UninitializedPointerEntry : public BaseObjectLiteralType<>
		{
			UniqueID UUID;
			RTTIBase** Ptr = nullptr;
			RTTI::Property Property;
		};


		CORE_DLLEXPORT void SerializeObject(const RTTIBase* obj, 
			rapidjson::Document& doc);
		CORE_DLLEXPORT void SerializeObject(const RTTIBase* obj, 
			rapidjson::GenericObject<false, rapidjson::Value> currentValue, 
			rapidjson::Document::AllocatorType& alloc);
		CORE_DLLEXPORT rapidjson::Value GetCorePropertyValue(const void* value, 
			const RTTI::Property& prop, 
			rapidjson::Document::AllocatorType& alloc);

		CORE_DLLEXPORT void DeserializeObject(RTTIBase* obj,  
			const rapidjson::Document& doc);
		CORE_DLLEXPORT void DeserializeObject(RTTIBase* obj, 
			const rapidjson::GenericObject<true, rapidjson::Value> currentValue,
			Dynarray<UninitializedPointerEntry>& uninitializedPointers);
		CORE_DLLEXPORT void SetCorePropertyValue(void* obj, 
			const RTTI::Property& prop, 
			const rapidjson::Value& value,
			Dynarray<UninitializedPointerEntry>& uninitializedPointers);

		CORE_DLLEXPORT void TraverseAndCall(RTTIBase* obj, const std::function<void(RTTIBase*)>& func);
	}
}