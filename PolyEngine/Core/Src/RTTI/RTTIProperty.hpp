#pragma once

#include "Defines.hpp"
#include "Utils/EnumUtils.hpp"
#include "RTTI/RTTITypeInfo.hpp"
#include "Collections/String.hpp"
#include "Collections/Dynarray.hpp"
#include "RTTI/CustomTypeTraits.hpp"

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
			CUSTOM,
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
			DYNARRAY,
			_COUNT
		};

		template <typename T> inline eCorePropertyType GetCorePropertyType() { return RTTI::Impl::HasGetTypeInfoFunc<T>::value ? eCorePropertyType::CUSTOM : eCorePropertyType::UNHANDLED; };
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

		//-----------------------------------------------------------------------------------------------------------------------
		enum class ePropertyFlag {
			NONE = 0,
			DONT_SERIALIZE = BIT(1)
		};

		struct PropertyImplData : public BaseObjectLiteralType<> {};

		struct Property final : public BaseObjectLiteralType<>
		{
			Property() = default;
			Property(TypeInfo typeInfo, size_t offset, const char* name, ePropertyFlag flags, eCorePropertyType coreType, std::shared_ptr<PropertyImplData>&& implData = nullptr)
				: Type(typeInfo), Offset(offset), Name(name), Flags(flags), CoreType(coreType), ImplData(std::move(implData)) {}
			TypeInfo Type;
			size_t Offset;
			String Name;
			EnumFlags<ePropertyFlag> Flags;
			eCorePropertyType CoreType = eCorePropertyType::CUSTOM;
			std::shared_ptr<PropertyImplData> ImplData; // @fixme ugly hack for not working Two-phase lookup in MSVC, should use unique_ptr
		};

		//-----------------------------------------------------------------------------------------------------------------------
		// Declare method first
		template <typename ValueType> Property CreateDynarrayPropertyInfo(size_t offset, const char* name, ePropertyFlag flags);

		//-----------------------------------------------------------------------------------------------------------------------
		// Enum serialization property impl
		struct EnumPropertyImplData final : public PropertyImplData
		{
			const ::Poly::Impl::EnumInfoBase* EnumInfo = nullptr;
		};

		template <typename E> Property CreateEnumPropertyInfo(size_t offset, const char* name, ePropertyFlag flags)
		{
			STATIC_ASSERTE(std::is_enum<E>::value, "Enum type is required");
			using UnderlyingType = typename std::underlying_type<E>::type;
			STATIC_ASSERTE(std::is_integral<UnderlyingType>::value, "Only enums with integral underlying types are supported");
			STATIC_ASSERTE(std::is_signed<UnderlyingType>::value, "Only enums with signed underlying types are supported");
			STATIC_ASSERTE(sizeof(UnderlyingType) <= sizeof(i64), "Only enums with max 64 bit underlying types are supported");
			std::shared_ptr<EnumPropertyImplData> implData = std::make_shared<EnumPropertyImplData>();

			// Register EnumInfo object pointer to property
			implData->EnumInfo = &::Poly::Impl::EnumInfo<E>::Get();
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::ENUM, std::move(implData)};
		}

		//-----------------------------------------------------------------------------------------------------------------------
		struct CollectionPropertyImplDataBase : public PropertyImplData
		{
			Property PropertyType;

			virtual void Resize(void* collection, size_t size) const = 0;
			virtual size_t GetSize(const void* collection) const = 0;
			virtual void* GetValue(void* collection, size_t idx) const = 0;
			virtual const void* GetValue(const void* collection, size_t idx) const = 0;
		};

		//-----------------------------------------------------------------------------------------------------------------------
		// Dynarray serialization property impl
		template <typename ValueType>
		struct DynarrayPropertyImplData final : public CollectionPropertyImplDataBase
		{
			DynarrayPropertyImplData(ePropertyFlag flags) { PropertyType = CreatePropertyInfo<ValueType>(0, "value", flags); }

			void Resize(void* collection, size_t size) const override { reinterpret_cast<Dynarray<ValueType>*>(collection)->Resize(size); }
			size_t GetSize(const void* collection) const override { return reinterpret_cast<const Dynarray<ValueType>*>(collection)->GetSize(); }
			void* GetValue(void* collection, size_t idx) const override { return &((*reinterpret_cast<Dynarray<ValueType>*>(collection))[idx]); }
			const void* GetValue(const void* collection, size_t idx) const override { return &((*reinterpret_cast<const Dynarray<ValueType>*>(collection))[idx]); }
		};

		template <typename ValueType> Property CreateDynarrayPropertyInfo(size_t offset, const char* name, ePropertyFlag flags)
		{
			std::shared_ptr<CollectionPropertyImplDataBase> implData = std::shared_ptr<CollectionPropertyImplDataBase>{ new DynarrayPropertyImplData<ValueType>(flags) };
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::DYNARRAY, std::move(implData) };
		}

		//-----------------------------------------------------------------------------------------------------------------------
		template <typename T> inline Property CreatePropertyInfo(size_t offset, const char* name, ePropertyFlag flags)
		{ 
			return constexpr_match(
				std::is_enum<T>{},			[&](auto lazy) { return CreateEnumPropertyInfo<LAZY_TYPE(T)>(offset, name, flags); },
				Trait::IsDynarray<T>{},		[&](auto lazy) { return CreateDynarrayPropertyInfo<Trait::DynarrayValueType<LAZY_TYPE(T)>::type>(offset, name, flags); },
				std::is_same<String, T>{},	[&](auto) { return Property{ TypeInfo::INVALID, offset, name, flags, GetCorePropertyType<String>() }; },
				/*default*/					[&](auto lazy) { return Property{ TypeInfo::Get<LAZY_TYPE(T)>(), offset, name, flags, GetCorePropertyType<LAZY_TYPE(T)>() }; }
			);
		}

		//-----------------------------------------------------------------------------------------------------------------------
		class CORE_DLLEXPORT PropertyManagerBase : public BaseObject<> {
		public:
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
