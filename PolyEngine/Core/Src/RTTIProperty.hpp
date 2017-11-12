#pragma once

#include "Defines.hpp"
#include "EnumUtils.hpp"
#include "RTTITypeInfo.hpp"
#include "String.hpp"

#include <functional>
#include <initializer_list>
#include <unordered_map>

namespace Poly {
	namespace RTTI {

		template<typename T, typename U>
		constexpr size_t OffsetOfMember(U T::*member) { return (char*)&((T*)nullptr->*member) - (char*)nullptr; }

		enum class eCorePropertyType
		{
			UNHANDLED,
			NONE,
			BOOL,
			INT8,
			INT16,
			INT32,
			INT64,
			UINT8,
			UINT16,
			UINT32,
			UINT64,
			FLOAT,
			DOUBLE,
			ENUM,
			STRING,
			_COUNT
		};

		template <typename T> inline eCorePropertyType GetCorePropertyType() { return RTTI::Impl::HasGetTypeInfoFunc<T>::value ? eCorePropertyType::NONE : eCorePropertyType::UNHANDLED; };
		// specializations
		template <> inline eCorePropertyType GetCorePropertyType<bool>() { return eCorePropertyType::BOOL; };
		template <> inline eCorePropertyType GetCorePropertyType<i8>() { return eCorePropertyType::INT8; };
		template <> inline eCorePropertyType GetCorePropertyType<i16>() { return eCorePropertyType::INT16; };
		template <> inline eCorePropertyType GetCorePropertyType<i32>() { return eCorePropertyType::INT32; };
		template <> inline eCorePropertyType GetCorePropertyType<i64>() { return eCorePropertyType::INT64; };
		template <> inline eCorePropertyType GetCorePropertyType<u8>() { return eCorePropertyType::UINT8; };
		template <> inline eCorePropertyType GetCorePropertyType<u16>() { return eCorePropertyType::UINT16; };
		template <> inline eCorePropertyType GetCorePropertyType<u32>() { return eCorePropertyType::UINT32; };
		template <> inline eCorePropertyType GetCorePropertyType<u64>() { return eCorePropertyType::UINT64; };
		template <> inline eCorePropertyType GetCorePropertyType<float>() { return eCorePropertyType::FLOAT; };
		template <> inline eCorePropertyType GetCorePropertyType<double>() { return eCorePropertyType::DOUBLE; };
		template <> inline eCorePropertyType GetCorePropertyType<String>() { return eCorePropertyType::STRING; };

		enum class ePropertyFlag {
			NONE = 0,
			DONT_SERIALIZE = BIT(1)
		};

		struct PropertyImplData : public BaseObjectLiteralType<> {};

		struct Property : public BaseObjectLiteralType<>
		{
			Property(TypeInfo typeInfo, size_t offset, const char* name, ePropertyFlag flags, eCorePropertyType coreType, std::shared_ptr<PropertyImplData>&& implData = nullptr)
				: Type(typeInfo), Offset(offset), Name(name), Flags(flags), CoreType(coreType), ImplData(std::move(implData)) {}
			TypeInfo Type;
			size_t Offset;
			String Name;
			EnumFlags<ePropertyFlag> Flags;
			eCorePropertyType CoreType;
			std::shared_ptr<PropertyImplData> ImplData; // @fixme ugly hack for not working Two-phase lookup in MSVC, should use unique_ptr
		};

		struct EnumPropertyImplData : public PropertyImplData
		{
			std::unordered_map<i64, String> ValueToNameMap;
			std::map<String, i64> NameToValueMap;
			size_t ValueSize = 0;
		};

		template <typename E> Property CreateEnumPropertyInfo(size_t offset, const char* name, ePropertyFlag flags)
		{
			STATIC_ASSERTE(std::is_enum<E>::value, "Enum type is required");
			STATIC_ASSERTE(std::is_integral<std::underlying_type<E>::type>::value, "Enum with integral underlying type is required");
			std::shared_ptr<EnumPropertyImplData> implData = std::make_shared<EnumPropertyImplData>();

			for (E val : IterateEnum<E>())
			{
				String name = GetEnumName(val);
				implData->ValueToNameMap.insert(std::make_pair((i64)val, name));
				implData->NameToValueMap.insert(std::make_pair(name, (i64)val));
			}
			implData->ValueSize = sizeof(std::underlying_type<E>::type);
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::ENUM, std::move(implData)};
		}

		template <typename T> inline Property CreatePropertyInfo(size_t offset, const char* name, ePropertyFlag flags)
		{ 
			return Property{ TypeInfo::Get<T>(), offset, name, flags, GetCorePropertyType<T>()}; 
		};

		// specializations
		template <> inline Property CreatePropertyInfo<String>(size_t offset, const char* name, ePropertyFlag flags)
		{
			return Property{ TypeInfo::INVALID, offset, name, flags, GetCorePropertyType<String>() };
		}

		class CORE_DLLEXPORT PropertyManagerBase : public BaseObject<> {
		public:
			//TODO serialiation

			void AddProperty(Property&& property) { Properties.PushBack(std::move(property)); }
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
#define RTTI_PROPERTY(variable, var_name, flags) \
	STATIC_ASSERTE(!std::is_pointer<decltype(variable)>::value || EnumFlags<Poly::RTTI::ePropertyFlag>(flags).IsSet(Poly::RTTI::ePropertyFlag::DONT_SERIALIZE), "Serializable variable cannot be a pointer."); \
	mgr->AddProperty(Poly::RTTI::CreatePropertyInfo<decltype(variable)>(Poly::RTTI::OffsetOfMember(&T::variable), var_name, flags))

#define RTTI_PROPERTY_AUTONAME(variable, flags) \
	STATIC_ASSERTE(!std::is_pointer<decltype(variable)>::value || EnumFlags<Poly::RTTI::ePropertyFlag>(flags).IsSet(Poly::RTTI::ePropertyFlag::DONT_SERIALIZE), "Serializable variable cannot be a pointer."); \
	mgr->AddProperty(Poly::RTTI::CreatePropertyInfo<decltype(variable)>(Poly::RTTI::OffsetOfMember(&T::variable), #variable, flags))	

// property with enum type
#define RTTI_PROPERTY_ENUM(variable, var_name, flags) \
	STATIC_ASSERTE(!std::is_pointer<decltype(variable)>::value || EnumFlags<Poly::RTTI::ePropertyFlag>(flags).IsSet(Poly::RTTI::ePropertyFlag::DONT_SERIALIZE), "Serializable variable cannot be a pointer."); \
	mgr->AddProperty(Poly::RTTI::CreateEnumPropertyInfo<decltype(variable)>(Poly::RTTI::OffsetOfMember(&T::variable), var_name, flags))
