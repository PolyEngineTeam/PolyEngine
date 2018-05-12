#pragma once

#include "Defines.hpp"
#include "BaseObject.hpp"

#include "RTTICast.hpp"
#include "RTTITypeInfo.hpp"
#include "RTTIProperty.hpp"

namespace Poly {
	enum eSerializationType
	{
		JSON,

		_COUNT
	};

	class CORE_DLLEXPORT RTTIBase : public BaseObject<> {
		RTTI_DECLARE_TYPE(Poly::RTTIBase) { NO_RTTI_PROPERTY(); }

		void SerializeToFile(const String& fileName, eSerializationType type = eSerializationType::JSON);
		void DeserializeFromFile(const String& fileName, eSerializationType type = eSerializationType::JSON);
	};
}