#pragma once

#include <pe/Defines.hpp>
#include <pe/core/BaseObject.hpp>
#include <pe/core/rtti/RTTICast.hpp>
#include <pe/core/rtti/RTTIProperty.hpp>
#include <pe/core/UniqueID.hpp>

namespace Poly {
	enum eSerializationType
	{
		JSON,

		_COUNT
	};

	class CORE_DLLEXPORT RTTIBase : public ::pe::core::BaseObject<> {
		RTTI_DECLARE_TYPE(Poly::RTTIBase) 
		{ 
			RTTI_PROPERTY_AUTONAME(UUID, Poly::RTTI::ePropertyFlag::EDITOR_DEBUG_ONLY);
		}

		RTTIBase();
		~RTTIBase() override;

		void SerializeToFile(const ::pe::core::storage::String& fileName, eSerializationType type = eSerializationType::JSON);
		void DeserializeFromFile(const ::pe::core::storage::String& fileName, eSerializationType type = eSerializationType::JSON);

		virtual void BeforeSerializationCallback() {}
		virtual void AfterSerializationCallback() {}

		virtual void BeforeDeserializationCallback() {}
		virtual void AfterDeserializationCallback() {}

		inline const ::pe::core::UniqueID& GetUUID() const { return UUID; }
	private:
		::pe::core::UniqueID UUID;
	};

	class CORE_DLLEXPORT RTTIObjectsManager : public ::pe::core::BaseObject<>
	{
	public:
		static RTTIObjectsManager& Get();

		void Register(RTTIBase* obj);
		void Unregister(const ::pe::core::UniqueID& id);
		void FixMapingAfterDeserialization(RTTIBase* obj, const ::pe::core::UniqueID& oldID)
		{
			Unregister(oldID);
			Register(obj);
		}

		RTTIBase* TryGetObjectByID(const ::pe::core::UniqueID& id);
		RTTIBase* GetObjectByID(const ::pe::core::UniqueID& id);
	private:
		RTTIObjectsManager() = default;
		RTTIObjectsManager(const RTTIObjectsManager&) = delete;
		RTTIObjectsManager(RTTIObjectsManager&&) = delete;
		RTTIObjectsManager operator=(const RTTIObjectsManager&) = delete;
		RTTIObjectsManager operator=(RTTIObjectsManager&&) = delete;

		std::unordered_map<::pe::core::UniqueID, RTTIBase*> DeserializedObjectsById;
	};
}