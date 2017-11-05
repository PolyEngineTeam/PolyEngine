#pragma once

#include "Defines.hpp"
#include "EnumUtils.hpp"
#include "RTTITypeInfo.hpp"
#include "String.hpp"

#include <functional>
#include <initializer_list>

namespace Poly {
	namespace RTTI {

		template<typename T, typename U>
		constexpr size_t OffsetOfMember(U T::*member) { return (char*)&((T*)nullptr->*member) - (char*)nullptr; }

		enum class eCorePropertyType
		{
			NONE,
			BOOL,
			ENUM,
			_COUNT
		};

		template <typename T> eCorePropertyType GetCorePropertyType() { return eCorePropertyType::NONE; };
		// specializations
		template <> CORE_DLLEXPORT eCorePropertyType GetCorePropertyType<bool>() { return eCorePropertyType::BOOL; };

		enum class ePropertyFlag {
			NONE = 0,
			DONT_SERIALIZE = BIT(1)
		};

		struct Property : public BaseObjectLiteralType<> {
			Property(TypeInfo typeInfo, size_t offset, const char* name, ePropertyFlag flags, eCorePropertyType coreType)
				: Type(typeInfo), Offset(offset), Name(name), Flags(flags), CoreType(coreType) {}
			TypeInfo Type;
			size_t Offset;
			String Name;
			EnumFlags<ePropertyFlag> Flags;
			eCorePropertyType CoreType;
		};

		class CORE_DLLEXPORT PropertyManagerBase : public BaseObject<> {
		public:
			//TODO serialiation

			void AddProperty(const Property& property) { Properties.PushBack(property); }
			const Dynarray<Property>& GetPropertyList() const { return Properties; };

		protected:
			Dynarray<Property> Properties;
		};

		template<class T>
		class PropertyManager : public PropertyManagerBase {
		public:
			PropertyManager() { T::InitProperties(this); }
			~PropertyManager() { }
		};

	} // namespace RTTI
} // namespace Poly

#define RTTI_GENERATE_PROPERTY_LIST_BASE(Type)\
	friend class Poly::RTTI::PropertyManager<Type>; \
	virtual Poly::RTTI::PropertyManagerBase* GetPropertyManager() const; \
	template <class T> \
	static void InitProperties(Poly::RTTI::PropertyManager<T>* mgr)

#define RTTI_GENERATE_PROPERTY_LIST(Type)\
	friend class Poly::RTTI::PropertyManager<Type>; \
	Poly::RTTI::PropertyManagerBase* GetPropertyManager() const override; \
	template <class T> \
	static void InitProperties(Poly::RTTI::PropertyManager<T>* mgr)

#define RTTI_PROPERTY_MANAGER_IMPL(Type)\
	Poly::RTTI::PropertyManagerBase* Type::GetPropertyManager() const { static Poly::RTTI::PropertyManager<Type> instance; return &instance; }

#define NO_RTTI_PROPERTY() UNUSED(mgr)

// standard RTTIBase deriving (or POD type) property
#define RTTI_PROPERTY(Type, variable, var_name, flags) \
	STATIC_ASSERTE(!std::is_pointer<Type>::value || EnumFlags<Poly::RTTI::ePropertyFlag>(flags).IsSet(Poly::RTTI::ePropertyFlag::DONT_SERIALIZE), "Serializable variable cannot be a pointer."); \
	mgr->AddProperty(Poly::RTTI::Property{Poly::RTTI::TypeInfo::Get<Type>(), Poly::RTTI::OffsetOfMember(&T::variable), var_name, flags, Poly::RTTI::GetCorePropertyType<Type>()})

// property with enum type
#define RTTI_PROPERTY_ENUM(Type, variable, var_name, flags) \
	STATIC_ASSERTE(std::is_enum<Type>::value, "Enum type is required"); \
	STATIC_ASSERTE(std::is_integral<std::underlying_type<Type>::type>::value, "Enum with integral underlying type is required"); \
	STATIC_ASSERTE(!std::is_pointer<Type>::value || EnumFlags<Poly::RTTI::ePropertyFlag>(flags).IsSet(Poly::RTTI::ePropertyFlag::DONT_SERIALIZE), "Serializable variable cannot be a pointer."); \
	mgr->AddProperty(Poly::RTTI::Property{Poly::RTTI::TypeInfo::Get<Type>(), Poly::RTTI::OffsetOfMember(&T::variable), var_name, flags, Poly::RTTI::eCorePropertyType::ENUM})
