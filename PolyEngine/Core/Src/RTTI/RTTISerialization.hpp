#pragma once

#include "RTTI/RTTI.hpp"
#include "RTTIProperty.hpp"
#include <rapidjson/document.h>

namespace Poly
{
	namespace RTTI
	{
		CORE_DLLEXPORT void SerializeObject(const RTTIBase* obj, rapidjson::Document& doc);
		CORE_DLLEXPORT void SerializeObject(const RTTIBase* obj, rapidjson::GenericObject<false, rapidjson::Value> currentValue, rapidjson::Document::AllocatorType& alloc);
		CORE_DLLEXPORT rapidjson::Value GetCorePropertyValue(const void* value, const RTTI::Property& prop, rapidjson::Document::AllocatorType& alloc);

		CORE_DLLEXPORT void DeserializeObject(RTTIBase* obj,  const rapidjson::Document& doc);
		CORE_DLLEXPORT void DeserializeObject(RTTIBase* obj, const rapidjson::GenericObject<true, rapidjson::Value> currentValue);
		CORE_DLLEXPORT void SetCorePropertyValue(void* obj, const RTTI::Property& prop, const rapidjson::Value& value);
	}
}