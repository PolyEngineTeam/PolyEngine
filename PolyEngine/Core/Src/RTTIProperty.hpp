#pragma once

#include "Defines.hpp"
#include "EnumUtils.hpp"
#include "RTTITypeInfo.hpp"

#include <map>
#include <vector>
#include <string>
#include <functional>
#include <initializer_list>

namespace Poly {
	namespace RTTI {

		template<typename T, typename U>
		size_t offsetOfMember(U T::*member) {
			return (char*)&((T*)nullptr->*member) - (char*)nullptr;
		}

		enum ePropertyFlag {
			NONE = 0,
			DONT_SERIALIZE = BIT(1)
		};

		struct Property {
			TypeInfo type;
			size_t offset;
			const char* name;
			ePropertyFlag flags;
		};

		class IPropertyManager {
		public:
			virtual void Serialize() {
				//TODO serialiation
			}
			virtual void Deserialize() {
				//TODO deserialiation
			}

			void AddProperty(const Property& property) { m_properties.push_back(property); }
			const std::vector<Property>& GetPropertyList() { return m_properties; };

		protected:
			std::vector<Property> m_properties;
		};

		template<class T>
		class PropertyManager : public IPropertyManager {
		public:
			PropertyManager() { T::InitProperties(this); }
			~PropertyManager() { }
		};

	}
}


#define RTTI_PROPERTIES_BEGIN(Type)  \
friend class Poly::RTTI::PropertyManager<Type>;\
  Poly::RTTI::IPropertyManager* GetPropertyManager() override {static Poly::RTTI::PropertyManager<Type> instance; return &instance; } \
  template <class T> \
  static void InitProperties(Poly::RTTI::PropertyManager<T>* mgr) {

#define RTTI_PROPERTIES_END }

// standard RTTIBase deriving (or POD type) property
#define RTTI_PROPERTY(Type, variable, var_name, flags) \
  _STATIC_ASSERTE(!std::is_pointer<Type>::value || (flags & Poly::RTTI::DONT_SERIALIZE != 0), "Serializable variable cannot be a pointer."); \
  mgr->AddProperty(Poly::RTTI::Property{Poly::RTTI::TypeInfo::Get<Type>(), Poly::RTTI::offsetOfMember(&T::variable), var_name, flags});

// property with enum type
#define RTTI_PROPERTY_ENUM(Type, variable, var_name, flags) \
  _STATIC_ASSERTE(!std::is_pointer<Type>::value || (flags & Poly::RTTI::DONT_SERIALIZE != 0), "Serializable variable cannot be a pointer."); \
  mgr->AddProperty(Poly::RTTI::Property{RTTI::TypeInfo::Get<int>(), Poly::RTTI::offsetOfMember(&T::variable), var_name, flags});
