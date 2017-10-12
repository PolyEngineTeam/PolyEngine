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
			TypeInfo Type;
			size_t Offset;
			const char* Name;
			ePropertyFlag Flags;
		};

		class IPropertyManager {
		public:
			virtual void Serialize() {
				//TODO serialiation
			}
			virtual void Deserialize() {
				//TODO deserialiation
			}

			void AddProperty(const Property& property) { Properties.PushBack(property); }
			const Dynarray<Property>& GetPropertyList() { return Properties; };

		protected:
			Dynarray<Property> Properties;
		};

		template<class T>
		class PropertyManager : public IPropertyManager {
		public:
			PropertyManager() { T::InitProperties(this); }
			~PropertyManager() { }
		};

	}
}

#define RTTI_GENERATE_PROPERTY_LIST(Type)\
friend class Poly::RTTI::PropertyManager<Type>; \
virtual Poly::RTTI::IPropertyManager* GetPropertyManager() { static Poly::RTTI::PropertyManager<Type> instance; return &instance; } \
template <class T> \
static void InitProperties(Poly::RTTI::PropertyManager<T>* mgr)

// standard RTTIBase deriving (or POD type) property
#define RTTI_PROPERTY(Type, variable, var_name, flags) \
  STATIC_ASSERTE(!std::is_pointer<Type>::value || ((flags & Poly::RTTI::DONT_SERIALIZE) != 0), "Serializable variable cannot be a pointer."); \
  mgr->AddProperty(Poly::RTTI::Property{Poly::RTTI::TypeInfo::Get<Type>(), Poly::RTTI::offsetOfMember(&T::variable), var_name, flags})

// property with enum type
#define RTTI_PROPERTY_ENUM(Type, variable, var_name, flags) \
  STATIC_ASSERTE(std::is_enum<Type>::value, "Enum type is required"); \
  STATIC_ASSERTE(std::is_integral<std::underlying_type<Type>::type>::value, "Enum with integral underlying type is required"); \
  STATIC_ASSERTE(!std::is_pointer<Type>::value || ((flags & Poly::RTTI::DONT_SERIALIZE) != 0), "Serializable variable cannot be a pointer."); \
  mgr->AddProperty(Poly::RTTI::Property{RTTI::TypeInfo::Get<i64>(), Poly::RTTI::offsetOfMember(&T::variable), var_name, flags})
