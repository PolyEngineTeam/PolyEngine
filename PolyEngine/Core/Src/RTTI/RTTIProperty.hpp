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

	// Fwd declarations
	class Vector;
	class Vector2f;
	class Vector2i;
	class Quaternion;
	class Angle;
	class Color;
	class Matrix;
	class UniqueID;

	namespace RTTI {

		template<typename T, typename U>
		constexpr size_t OffsetOfMember(U T::*member) { return (char*)&((T*)nullptr->*member) - (char*)nullptr; }

		enum class eCorePropertyType
		{
			UNHANDLED, // for fast failing
			CUSTOM,	// custom rtti property (user defined class)
			
			// fundamental
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
			
			// other
			ENUM,
			ENUM_FLAGS,
			STRING,
			UUID,
			ITERABLE_POOL_ALLOCATOR,
			UNIQUE_PTR,
			
			// collections
			DYNARRAY,
			ORDERED_MAP,
			ENUM_ARRAY,

			// math
			VECTOR,
			VECTOR_2F,
			VECTOR_2I,
			QUATERNION,
			ANGLE,
			COLOR,
			MATRIX,

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

		template <> inline eCorePropertyType GetCorePropertyType<Vector>() { return eCorePropertyType::VECTOR; };
		template <> inline eCorePropertyType GetCorePropertyType<Vector2f>() { return eCorePropertyType::VECTOR_2F; };
		template <> inline eCorePropertyType GetCorePropertyType<Vector2i>() { return eCorePropertyType::VECTOR_2I; };
		template <> inline eCorePropertyType GetCorePropertyType<Quaternion>() { return eCorePropertyType::QUATERNION; };
		template <> inline eCorePropertyType GetCorePropertyType<Angle>() { return eCorePropertyType::ANGLE; };
		template <> inline eCorePropertyType GetCorePropertyType<Color>() { return eCorePropertyType::COLOR; };
		template <> inline eCorePropertyType GetCorePropertyType<Matrix>() { return eCorePropertyType::MATRIX; };
		template <> inline eCorePropertyType GetCorePropertyType<UniqueID>() { return eCorePropertyType::UUID; };

		//-----------------------------------------------------------------------------------------------------------------------
		enum class ePropertyFlag {
			NONE = 0,
			DONT_SERIALIZE = BIT(1)
		};

		struct PropertyImplData : public BaseObjectLiteralType<> {};

		using FactoryFunc_t = std::function<void*(TypeInfo)>;

		struct Property final : public BaseObjectLiteralType<>
		{
			Property() = default;
			Property(TypeInfo typeInfo, size_t offset, const char* name, ePropertyFlag flags, eCorePropertyType coreType, std::shared_ptr<PropertyImplData>&& implData = nullptr, FactoryFunc_t&& factory_func = nullptr)
				: Type(typeInfo), Offset(offset), Name(name), Flags(flags), CoreType(coreType), ImplData(std::move(implData)), FactoryFunc(std::move(factory_func))
			{
				HEAVY_ASSERTE(CoreType != eCorePropertyType::UNHANDLED, "Unhandled property type!");
			}
			TypeInfo Type;
			size_t Offset;
			String Name;
			EnumFlags<ePropertyFlag> Flags;
			eCorePropertyType CoreType = eCorePropertyType::CUSTOM;
			std::shared_ptr<PropertyImplData> ImplData; // @fixme ugly hack for not working Two-phase lookup in MSVC, should use unique_ptr
			FactoryFunc_t FactoryFunc;
		};

		//-----------------------------------------------------------------------------------------------------------------------
		// Declare method first
		template <typename T> inline Property CreatePropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t factory_func);

		//-----------------------------------------------------------------------------------------------------------------------
		// Enum serialization property impl
		struct EnumPropertyImplData final : public PropertyImplData
		{
			const ::Poly::Impl::EnumInfoBase* EnumInfo = nullptr;
		};

		template <typename E> Property CreateEnumPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t&& factory_func)
		{
			STATIC_ASSERTE(std::is_enum<E>::value, "Enum type is required");
			using UnderlyingType = typename std::underlying_type<E>::type;
			STATIC_ASSERTE(std::is_integral<UnderlyingType>::value, "Only enums with integral underlying types are supported");
			STATIC_ASSERTE(std::is_signed<UnderlyingType>::value, "Only enums with signed underlying types are supported");
			STATIC_ASSERTE(sizeof(UnderlyingType) <= sizeof(i64), "Only enums with max 64 bit underlying types are supported");
			std::shared_ptr<EnumPropertyImplData> implData = std::make_shared<EnumPropertyImplData>();

			// Register EnumInfo object pointer to property
			implData->EnumInfo = &::Poly::Impl::EnumInfo<E>::Get();
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::ENUM, std::move(implData), std::move(factory_func)};
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
			DynarrayPropertyImplData(ePropertyFlag flags, FactoryFunc_t&& factory_func) { PropertyType = CreatePropertyInfo<ValueType>(0, "value", flags, std::move(factory_func)); }

			void Resize(void* collection, size_t size) const override { reinterpret_cast<Dynarray<ValueType>*>(collection)->Resize(size); }
			size_t GetSize(const void* collection) const override { return reinterpret_cast<const Dynarray<ValueType>*>(collection)->GetSize(); }
			void* GetValue(void* collection, size_t idx) const override { return &((*reinterpret_cast<Dynarray<ValueType>*>(collection))[idx]); }
			const void* GetValue(const void* collection, size_t idx) const override { return &((*reinterpret_cast<const Dynarray<ValueType>*>(collection))[idx]); }
		};

		template <typename ValueType> Property CreateDynarrayPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t&& factory_func)
		{
			std::shared_ptr<CollectionPropertyImplDataBase> implData = std::shared_ptr<CollectionPropertyImplDataBase>{ new DynarrayPropertyImplData<ValueType>(flags, std::move(factory_func)) };
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::DYNARRAY, std::move(implData) };
		}

		//-----------------------------------------------------------------------------------------------------------------------
		struct DictionaryPropertyImplDataBase : public PropertyImplData
		{
			Property KeyPropertyType;
			Property ValuePropertyType;

			virtual void* GetKeyTemporaryStorage() const = 0;
			virtual void* GetValueTemporaryStorage() const = 0;
			virtual Dynarray<const void*> GetKeys(const void* collection) const = 0;
			virtual void Clear(void* collection) const = 0;
			virtual void SetValue(void* collection, void* key, void* value) const = 0;
			virtual const void* GetValue(const void* collection, const void* key) const = 0;
		};

		//-----------------------------------------------------------------------------------------------------------------------
		// OrderedMap serialization property impl
		template <typename KeyType, typename ValueType, size_t Bfactor>
		struct OrderedMapPropertyImplData final : public DictionaryPropertyImplDataBase
		{
			mutable KeyType TempKey;
			mutable ValueType TempValue;

			using MapType = OrderedMap<KeyType, ValueType, Bfactor>;

			OrderedMapPropertyImplData(ePropertyFlag flags, FactoryFunc_t&& factory_func)
			{
				KeyPropertyType = CreatePropertyInfo<KeyType>(0, "key", flags);
				ValuePropertyType = CreatePropertyInfo<ValueType>(0, "value", flags, std::move(factory_func));
			}

			void Clear(void* collection) const override { reinterpret_cast<MapType*>(collection)->Clear(); }
			void* GetKeyTemporaryStorage() const override { return reinterpret_cast<void*>(&TempKey); }
			void* GetValueTemporaryStorage() const override { return reinterpret_cast<void*>(&TempValue); }

			Dynarray<const void*> GetKeys(const void* collection) const override
			{
				Dynarray<const void*> ret;
				const MapType& map = *reinterpret_cast<const MapType*>(collection);
				for (const KeyType& key : map.Keys())
					ret.PushBack((const void*)&key);
				return ret;
			}

			void SetValue(void* collection, void* key, void* value) const override
			{
				KeyType& keyRef = *reinterpret_cast<KeyType*>(key);
				ValueType& valueRef = *reinterpret_cast<ValueType*>(value);
				reinterpret_cast<MapType*>(collection)->MustInsert(std::move(keyRef), std::move(valueRef));
			}

			const void* GetValue(const void* collection, const void* key) const override
			{
				const KeyType& keyRef = *reinterpret_cast<const KeyType*>(key);
				const ValueType& valueRef = reinterpret_cast<const MapType*>(collection)->Get(keyRef).Value();
				return reinterpret_cast<const void*>(&valueRef);
			}
		};

		template <typename KeyType, typename ValueType, size_t Bfactor> Property CreateOrderedMapPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t&& factory_func)
		{
			std::shared_ptr<DictionaryPropertyImplDataBase> implData = std::shared_ptr<DictionaryPropertyImplDataBase>{ new OrderedMapPropertyImplData<KeyType, ValueType, Bfactor>(flags, std::move(factory_func)) };
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::ORDERED_MAP, std::move(implData) };
		}

		//-----------------------------------------------------------------------------------------------------------------------
		// OrderedMap serialization property impl
		template <typename EnumType, typename ValueType>
		struct EnumArrayPropertyImplData final : public DictionaryPropertyImplDataBase
		{
			mutable EnumType TempKey;
			mutable ValueType TempValue;
			mutable Dynarray<EnumType> EnumValues;

			using EnumArrayType = EnumArray<ValueType, EnumType>;

			EnumArrayPropertyImplData(ePropertyFlag flags, FactoryFunc_t&& factory_func)
			{
				KeyPropertyType = CreatePropertyInfo<EnumType>(0, "key", flags);
				ValuePropertyType = CreatePropertyInfo<ValueType>(0, "value", flags, std::move(factory_func));
			}

			void Clear(void* collection) const override 
			{ 
				EnumArrayType& col = *reinterpret_cast<EnumArrayType*>(collection);
				for (EnumType e : IterateEnum<EnumType>())
					col[e] = ValueType{};
			}
			void* GetKeyTemporaryStorage() const override { return reinterpret_cast<void*>(&TempKey); }
			void* GetValueTemporaryStorage() const override { return reinterpret_cast<void*>(&TempValue); }

			Dynarray<const void*> GetKeys(const void* collection) const override
			{
				UNUSED(collection);
				if (EnumValues.GetSize() == 0)
				{
					for (EnumType e : IterateEnum<EnumType>())
						EnumValues.PushBack(e);
				}

				Dynarray<const void*> ret;
				for (const EnumType& key : EnumValues)
					ret.PushBack((const void*)&key);
				return ret;
			}

			void SetValue(void* collection, void* key, void* value) const override
			{
				const EnumType& keyRef = *reinterpret_cast<const EnumType*>(key);
				ValueType& valueRef = *reinterpret_cast<ValueType*>(value);

				EnumArrayType& col = *reinterpret_cast<EnumArrayType*>(collection);
				col[keyRef] = std::move(valueRef);
			}

			const void* GetValue(const void* collection, const void* key) const override
			{
				const EnumType& keyRef = *reinterpret_cast<const EnumType*>(key);

				const EnumArrayType& col = *reinterpret_cast<const EnumArrayType*>(collection);
				return reinterpret_cast<const void*>(&col[keyRef]);
			}
		};

		template <typename EnumType, typename ValueType> Property CreateEnumArrayPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t&& factory_func)
		{
			std::shared_ptr<DictionaryPropertyImplDataBase> implData = std::shared_ptr<DictionaryPropertyImplDataBase>{ new EnumArrayPropertyImplData<EnumType, ValueType>(flags, std::move(factory_func)) };
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::ENUM_ARRAY, std::move(implData) };
		}

		//-----------------------------------------------------------------------------------------------------------------------
		// Enum flags serialization property impl
		struct EnumFlagsPropertyImplDataBase : public PropertyImplData
		{
			virtual i64 GetValue(const void* collection) const = 0;
			virtual void SetValue(void* collection, i64 value) const = 0;
		};

		template<typename E>
		struct EnumFlagsPropertyImplData final : public EnumFlagsPropertyImplDataBase
		{
			i64 GetValue(const void* collection) const override { return (typename EnumFlags<E>::FlagType)*reinterpret_cast<const EnumFlags<E>*>(collection); }
			void SetValue(void* collection, i64 value) const override { *reinterpret_cast<EnumFlags<E>*>(collection) = EnumFlags<E>((E)value); }
		};

		template <typename E> Property CreateEnumFlagsPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t&& factory_func)
		{
			STATIC_ASSERTE(std::is_enum<E>::value, "Enum type is required");
			using UnderlyingType = typename std::underlying_type<E>::type;
			STATIC_ASSERTE(std::is_integral<UnderlyingType>::value, "Only enums with integral underlying types are supported");
			STATIC_ASSERTE(std::is_signed<UnderlyingType>::value, "Only enums with signed underlying types are supported");
			STATIC_ASSERTE(sizeof(UnderlyingType) <= sizeof(i64), "Only enums with max 64 bit underlying types are supported");
			ASSERTE(factory_func == nullptr, "Cannot create enums with factory!");
			std::shared_ptr<EnumFlagsPropertyImplData<E>> implData = std::make_shared<EnumFlagsPropertyImplData<E>>();

			// Register EnumInfo object pointer to property
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::ENUM_FLAGS, std::move(implData) };
		}

		//-----------------------------------------------------------------------------------------------------------------------
		// Iterable Pool Allocator serialization property impl
		struct IterablePoolAllocatorPropertyImplDataBase : public PropertyImplData
		{
			virtual const void* GetNextValue(const void* collection, const void* obj) const = 0;
			virtual const void* Begin(const void* collection) const = 0;
			virtual void* Alloc(void* collection) const = 0;
		};

		template<typename T>
		struct IterablePoolAllocatorPropertyImplData final : public IterablePoolAllocatorPropertyImplDataBase
		{
			const void* GetNextValue(const void* collection, const void* obj) const override
			{
				const IterablePoolAllocator<T>& allocator = *reinterpret_cast<const IterablePoolAllocator<T>*>(collection);
				//@todo(muniu) implement
				return nullptr;
			}

			const void* Begin(const void* collection) const override
			{
				const IterablePoolAllocator<T>& allocator = *reinterpret_cast<const IterablePoolAllocator<T>*>(collection);
				//@todo(muniu) implement
				return nullptr;// allocator.Begin();
			}

			void* Alloc(void* collection) const override
			{
				IterablePoolAllocator<T>& allocator = *reinterpret_cast<IterablePoolAllocator<T>*>(collection);
				return allocator.Alloc();
			}
		};

		template <typename E> Property CreateIterablePoolAllocatorPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t&& factory_func)
		{
			std::shared_ptr<IterablePoolAllocatorPropertyImplData<E>> implData = std::make_shared<IterablePoolAllocatorPropertyImplData<E>>();

			// Register EnumInfo object pointer to property
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::ITERABLE_POOL_ALLOCATOR, std::move(implData), std::move(factory_func) };
		}

		//-----------------------------------------------------------------------------------------------------------------------
		// Unique ptr serialization property impl
		struct UniquePtrPropertyImplDataBase : public PropertyImplData
		{
			Property PropertyType;

			virtual void Create(void* ptr) const = 0;
			virtual void* Get(void* ptr) const = 0;
			virtual const void* Get(const void* ptr) const = 0;
		};

		template<typename T>
		struct UniquePtrPropertyImplData final : public UniquePtrPropertyImplDataBase
		{
			UniquePtrPropertyImplData(ePropertyFlag flags, FactoryFunc_t&& factory_func)
			{
				// Stop propagation of factory function
				PropertyType = CreatePropertyInfo<T>(0, "value", flags, nullptr);
				PropertyType.FactoryFunc = std::move(factory_func);
			}

			void Create(void* ptr) const override { 
				std::unique_ptr<T>& uptr = *reinterpret_cast<std::unique_ptr<T>*>(ptr);
				if (PropertyType.FactoryFunc)
					uptr.reset((T*)PropertyType.FactoryFunc(PropertyType.Type));
				else
					uptr = std::make_unique<T>();
			}
			void* Get(void* ptr) const override { return reinterpret_cast<std::unique_ptr<T>*>(ptr)->get(); }
			const void* Get(const void* ptr) const override { return reinterpret_cast<const std::unique_ptr<T>*>(ptr)->get(); }
		};

		template <typename T> Property CreateUniquePtrPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t&& factory_func)
		{
			std::shared_ptr<UniquePtrPropertyImplData<T>> implData = std::make_shared<UniquePtrPropertyImplData<T>>(flags, std::move(factory_func));

			// Register unique ptr object pointer to property
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::UNIQUE_PTR, std::move(implData)};
		}

		//-----------------------------------------------------------------------------------------------------------------------
		template <typename T> inline Property CreatePropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t factory_func = nullptr)
		{ 
			return constexpr_match(
				std::is_enum<T>{},			[&](auto lazy) { return CreateEnumPropertyInfo<LAZY_TYPE(T)>(offset, name, flags, std::move(factory_func)); },
				Trait::IsDynarray<T>{},		[&](auto lazy) { return CreateDynarrayPropertyInfo<typename Trait::DynarrayValueType<LAZY_TYPE(T)>::type>(offset, name, flags, std::move(factory_func)); },
				Trait::IsOrderedMap<T>{},	[&](auto lazy) { return CreateOrderedMapPropertyInfo<typename Trait::OrderedMapType<LAZY_TYPE(T)>::keyType, typename Trait::OrderedMapType<LAZY_TYPE(T)>::valueType, Trait::OrderedMapType<LAZY_TYPE(T)>::bFactor>(offset, name, flags, std::move(factory_func)); },
				Trait::IsEnumArray<T>{},	[&](auto lazy) { return CreateEnumArrayPropertyInfo<typename Trait::EnumArrayType<LAZY_TYPE(T)>::enumType, typename Trait::EnumArrayType<LAZY_TYPE(T)>::valueType>(offset, name, flags, std::move(factory_func)); },
				Trait::IsEnumFlags<T>{},	[&](auto lazy) { return CreateEnumFlagsPropertyInfo<typename Trait::EnumFlagsType<LAZY_TYPE(T)>::type>(offset, name, flags, std::move(factory_func)); },
				Trait::IsIterablePoolAllocator<T>{}, [&](auto lazy) { return CreateIterablePoolAllocatorPropertyInfo<typename Trait::IterablePoolAllocatorType<LAZY_TYPE(T)>::type>(offset, name, flags, std::move(factory_func)); },
				Trait::IsUniquePtr<T>{},	[&](auto lazy) { return CreateUniquePtrPropertyInfo<typename Trait::UniquePtrType<LAZY_TYPE(T)>::type>(offset, name, flags, std::move(factory_func)); },
				std::is_base_of<RTTIBase, T>{}, [&](auto lazy) { return Property{ TypeInfo::Get<LAZY_TYPE(T)>(), offset, name, flags, GetCorePropertyType<LAZY_TYPE(T)>(), nullptr, std::move(factory_func) }; },
				//TODO add RTTIBase specialization!
				/*default*/					[&](auto lazy) { return Property{ TypeInfo::INVALID, offset, name, flags, GetCorePropertyType<LAZY_TYPE(T)>(), nullptr, std::move(factory_func) }; }
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


namespace Poly::RTTI::Impl {
	template<typename T>
	void RegisterProperty(PropertyManagerBase* mgr, size_t offset, const char* var_name, FactoryFunc_t factory_func, ePropertyFlag flags)
	{
		STATIC_ASSERTE(!std::is_pointer<T>::value
			|| EnumFlags<Poly::RTTI::ePropertyFlag>(flags).IsSet(Poly::RTTI::ePropertyFlag::DONT_SERIALIZE),
			"Serializable variable cannot be a pointer.");
		mgr->AddProperty(Poly::RTTI::CreatePropertyInfo<T>(offset, var_name, flags, factory_func));
	}
}

// Standard RTTIBase deriving (or POD type) property
#define RTTI_PROPERTY(variable, var_name, flags) ::Poly::RTTI::Impl::RegisterProperty<decltype(variable)>(mgr, ::Poly::RTTI::OffsetOfMember(&T::variable), var_name, nullptr, flags)
#define RTTI_PROPERTY_AUTONAME(variable, flags) RTTI_PROPERTY(variable, #variable, flags)

// Factory supported rtti property
#define RTTI_PROPERTY_FACTORY(variable, var_name, factory_func, flags) ::Poly::RTTI::Impl::RegisterProperty<decltype(variable)>(mgr, ::Poly::RTTI::OffsetOfMember(&T::variable), var_name, factory_func, flags)
#define RTTI_PROPERTY_FACTORY_AUTONAME(variable, factory_func, flags) RTTI_PROPERTY_FACTORY(variable, #variable, factory_func, flags)


