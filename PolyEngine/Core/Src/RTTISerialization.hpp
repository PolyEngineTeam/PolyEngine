#pragma once

#include "RTTI.hpp"
#include "RTTIProperty.hpp"
#include <rapidjson/document.h>

namespace Poly
{
	namespace RTTI
	{
		CORE_DLLEXPORT void SerializeObject(const RTTIBase* obj, const String& propertyName, rapidjson::Value& currentValue, rapidjson::Document::AllocatorType& alloc);
		CORE_DLLEXPORT void SerializeObject(const RTTIBase* obj, const String& propertyName, rapidjson::Document& doc);
		CORE_DLLEXPORT bool SerializeCoreProperty(const void* value, const RTTI::Property& prop, rapidjson::Value& currentValue, rapidjson::Document::AllocatorType& alloc);
		CORE_DLLEXPORT rapidjson::Value GetCorePropertyValue(const void* value, const RTTI::Property& prop, rapidjson::Document::AllocatorType& alloc);
	}
}