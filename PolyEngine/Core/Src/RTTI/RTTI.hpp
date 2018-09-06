#pragma once

#include "Defines.hpp"
#include "BaseObject.hpp"
#include "RTTICast.hpp"
#include "RTTIProperty.hpp"
#include "UniqueID.hpp"

namespace Poly {
	enum eSerializationType
	{
		JSON,

		_COUNT
	};

	class CORE_DLLEXPORT RTTIBase : public BaseObject<> {
		RTTI_DECLARE_TYPE(Poly::RTTIBase) 
		{ 
			RTTI_PROPERTY_AUTONAME(UUID, Poly::RTTI::ePropertyFlag::NONE);
		}

		RTTIBase();
		~RTTIBase() override;

		void SerializeToFile(const String& fileName, eSerializationType type = eSerializationType::JSON);
		void DeserializeFromFile(const String& fileName, eSerializationType type = eSerializationType::JSON);

		virtual void BeforeSerializationCallback() {}
		virtual void AfterSerializationCallback() {}

		virtual void BeforeDeserializationCallback() {}
		virtual void AfterDeserializationCallback() {}

		inline const UniqueID& GetUUID() const { return UUID; }
	private:
		UniqueID UUID;
	};

	class CORE_DLLEXPORT RTTIObjectsManager : public BaseObject<>
	{
	public:
		static RTTIObjectsManager& Get();

		void Register(RTTIBase* obj);
		void Unregister(const UniqueID& id);
		void FixMapingAfterDeserialization(RTTIBase* obj, const UniqueID& oldID)
		{
			Unregister(oldID);
			Register(obj);
		}

		RTTIBase* TryGetObjectByID(const UniqueID& id);
		RTTIBase* GetObjectByID(const UniqueID& id);
	private:
		RTTIObjectsManager() = default;
		RTTIObjectsManager(const RTTIObjectsManager&) = delete;
		RTTIObjectsManager(RTTIObjectsManager&&) = delete;
		RTTIObjectsManager operator=(const RTTIObjectsManager&) = delete;
		RTTIObjectsManager operator=(RTTIObjectsManager&&) = delete;

		std::unordered_map<UniqueID, RTTIBase*> DeserializedObjectsById;
	};
}