#pragma once

#include <pe/Defines.hpp>
#include <pe/core/utils/EnumUtils.hpp>
#include <pe/core/rtti/RTTITypeInfo.hpp>
#include <pe/core/rtti/RTTICast.hpp>
#include <pe/core/storage/String.hpp>
#include <pe/core/rtti/CustomTypeTraits.hpp>
#include <pe/core/utils/Logger.hpp>
#include <pe/core/utils/MakeFunction.hpp>

// Fwd declarations
namespace pe::core
{
	class UniqueID;

	namespace math
	{
		class Vector;
		class Vector2f;
		class Vector2i;
		class Quaternion;
		class Angle;
		class Color;
		class Matrix;
	}
}

namespace Poly {
	class RTTIBase;

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
			RAW_PTR, // Only for RTTIBase objects
			
			// collections
			LIST,
			MAP,
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

		template <> inline eCorePropertyType GetCorePropertyType<::pe::core::UniqueID>() { return eCorePropertyType::UUID; };
		
		template <> inline eCorePropertyType GetCorePropertyType<::pe::core::storage::String>() { return eCorePropertyType::STRING; };

		template <> inline eCorePropertyType GetCorePropertyType<::pe::core::math::Vector>() { return eCorePropertyType::VECTOR; };
		template <> inline eCorePropertyType GetCorePropertyType<::pe::core::math::Vector2f>() { return eCorePropertyType::VECTOR_2F; };
		template <> inline eCorePropertyType GetCorePropertyType<::pe::core::math::Vector2i>() { return eCorePropertyType::VECTOR_2I; };
		template <> inline eCorePropertyType GetCorePropertyType<::pe::core::math::Quaternion>() { return eCorePropertyType::QUATERNION; };
		template <> inline eCorePropertyType GetCorePropertyType<::pe::core::math::Angle>() { return eCorePropertyType::ANGLE; };
		template <> inline eCorePropertyType GetCorePropertyType<::pe::core::math::Color>() { return eCorePropertyType::COLOR; };
		template <> inline eCorePropertyType GetCorePropertyType<::pe::core::math::Matrix>() { return eCorePropertyType::MATRIX; };
		

		//-----------------------------------------------------------------------------------------------------------------------
		enum class ePropertyFlag {
			NONE = 0,
			EDITOR_DEBUG_ONLY,
			DONT_SERIALIZE = BIT(1)
		};

		struct PropertyImplData : public ::pe::core::BaseObjectLiteralType<> {};

		template<typename T>
		using FactoryFunc_t = std::function<T(RTTIBase*, TypeInfo)>;

		struct FactoryStorageBase : public ::pe::core::BaseObject<> {};
		template<typename T>
		struct FactoryStorage : public FactoryStorageBase
		{
			FactoryStorage(FactoryFunc_t<T>&& f) : foo(std::move(f)) {}

			FactoryFunc_t<T> foo;
		};

		template<typename Foo>
		std::shared_ptr<FactoryStorageBase> CreateFactoryStorage(Foo&& foo)
		{
			return std::shared_ptr<FactoryStorageBase>(new FactoryStorage<typename Foo::result_type>(std::forward<Foo>(foo)));
		}


		class Property final : public ::pe::core::BaseObjectLiteralType<>
		{
		public:
			Property() = default;

			Property(TypeInfo typeInfo, size_t offset, const char* name, ePropertyFlag flags, eCorePropertyType coreType, std::shared_ptr<PropertyImplData>&& implData = nullptr, std::shared_ptr<FactoryStorageBase> factory_storage=nullptr)
				: Type(typeInfo), Offset(offset), Name(name), Flags(flags), CoreType(coreType), ImplData(std::move(implData)), m_factoryStorage(factory_storage)
			{
				HEAVY_ASSERTE(CoreType != eCorePropertyType::UNHANDLED || ::pe::core::utils::EnumFlags<ePropertyFlag>(Flags).IsSet(ePropertyFlag::DONT_SERIALIZE), "Unhandled property type!");
			}
			TypeInfo Type;
			size_t Offset;
			::pe::core::storage::String Name;
			::pe::core::utils::EnumFlags<ePropertyFlag> Flags;
			eCorePropertyType CoreType = eCorePropertyType::CUSTOM;
			std::shared_ptr<PropertyImplData> ImplData; // @fixme ugly hack for not working Two-phase lookup in MSVC, should use unique_ptr
			
			bool HasFactory() const { return m_factoryStorage != nullptr; }
			template<typename T>
			FactoryFunc_t<T> GetFactory() const { return static_cast<FactoryStorage<T>*>(m_factoryStorage.get())->foo; }
			
		private:
			std::shared_ptr<FactoryStorageBase> m_factoryStorage;
		};

		//-----------------------------------------------------------------------------------------------------------------------
		// Declare method first
		template <typename T, typename R> inline Property CreatePropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t<R>&& factory_func);

		//-----------------------------------------------------------------------------------------------------------------------
		// Enum serialization property impl
		struct EnumPropertyImplData final : public PropertyImplData
		{
			const ::pe::core::utils::impl::EnumInfoBase* EnumInfo = nullptr;
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
			implData->EnumInfo = &::pe::core::utils::impl::EnumInfo<E>::Get();
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::ENUM, std::move(implData)};
		}

		//-----------------------------------------------------------------------------------------------------------------------
		struct CollectionPropertyImplDataBase : public PropertyImplData
		{
			Property PropertyType;

			virtual void Resize(RTTIBase* parent, void* collection, size_t size) const = 0;
			virtual size_t GetSize(const void* collection) const = 0;
			virtual void* GetValue(void* collection, size_t idx) const = 0;
			virtual const void* GetValue(const void* collection, size_t idx) const = 0;
		};

		//-----------------------------------------------------------------------------------------------------------------------
		// std::vector serialization property impl

		template <typename ValueType>
		struct StdVectorPropertyImplData final : public CollectionPropertyImplDataBase
		{

			StdVectorPropertyImplData(ePropertyFlag flags) { PropertyType = CreatePropertyInfo<ValueType, void>(0, "value", flags, nullptr); }
			StdVectorPropertyImplData(ePropertyFlag flags, FactoryFunc_t<ValueType>&& factory_func)
			{ 
				PropertyType = CreatePropertyInfo<ValueType, ValueType>(0, "value", flags, std::move(factory_func));
			}

			void Resize(RTTIBase* parent, void* collection, size_t size) const override 
			{
				UNUSED(parent);
				auto col = reinterpret_cast<std::vector<ValueType>*>(collection);
				size_t oldSize = col->size();
				if(oldSize < size)
				{
					if constexpr (std::is_default_constructible_v<ValueType>)
					{
						col->resize(size);
					}
					else
					{
						ASSERTE(PropertyType.HasFactory(), "non default constructible object needs factory function.");
						col->reserve(size);
						FactoryFunc_t<ValueType> factory = PropertyType.GetFactory<ValueType>();
						for (size_t i=oldSize; i<size; ++i)
						{
							col->push_back(factory(parent, PropertyType.Type));
						}
					}
				}
				else if (oldSize > size)
				{
					for (size_t i=oldSize; i>size; --i)
					{
						col->pop_back();
					}
				}
			}
			size_t GetSize(const void* collection) const override { return reinterpret_cast<const std::vector<ValueType>*>(collection)->size(); }
			void* GetValue(void* collection, size_t idx) const override { return &((*reinterpret_cast<std::vector<ValueType>*>(collection))[idx]); }
			const void* GetValue(const void* collection, size_t idx) const override { return &((*reinterpret_cast<const std::vector<ValueType>*>(collection))[idx]); }
		};

		template <typename ValueType> Property CreateStdVectorPropertyInfo(size_t offset, const char* name, ePropertyFlag flags)
		{
			std::shared_ptr<CollectionPropertyImplDataBase> implData =
				std::shared_ptr<CollectionPropertyImplDataBase>{ new StdVectorPropertyImplData<ValueType>(flags) };
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::LIST, std::move(implData), nullptr };
		}

		template <typename ValueType> Property CreateStdVectorPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t<ValueType>&& factory_func)
		{
			std::shared_ptr<CollectionPropertyImplDataBase> implData =
				std::shared_ptr<CollectionPropertyImplDataBase>{ new StdVectorPropertyImplData<ValueType>(flags, std::move(factory_func)) };
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::LIST, std::move(implData), nullptr };
		}

		//-----------------------------------------------------------------------------------------------------------------------
		struct DictionaryPropertyImplDataBase : public PropertyImplData
		{
			Property KeyPropertyType;
			Property ValuePropertyType;

			virtual void* GetKeyTemporaryStorage() const = 0;
			virtual void* GetValueTemporaryStorage() const = 0;
			virtual std::vector<const void*> GetKeys(const void* collection) const = 0;
			virtual void Clear(void* collection) const = 0;
			virtual void SetValue(void* collection, void* key, void* value) const = 0;
			virtual const void* GetValue(const void* collection, const void* key) const = 0;
		};

		//-----------------------------------------------------------------------------------------------------------------------
		// std::map serialization property impl
		template <typename KeyType, typename ValueType>
		struct StdMapPropertyImplData final : public DictionaryPropertyImplDataBase
		{
			mutable KeyType TempKey;
			mutable ValueType TempValue;

			using MapType = std::map<KeyType, ValueType>;

			StdMapPropertyImplData(ePropertyFlag flags)
			{
				KeyPropertyType = CreatePropertyInfo<KeyType, void>(0, "key", flags, nullptr);
				ValuePropertyType = CreatePropertyInfo<ValueType, void>(0, "value", flags, nullptr);
			}

			StdMapPropertyImplData(ePropertyFlag flags, FactoryFunc_t<ValueType>&& factory_func)
			{
				KeyPropertyType = CreatePropertyInfo<KeyType, void>(0, "key", flags, nullptr);
				ValuePropertyType = CreatePropertyInfo<ValueType, ValueType>(0, "value", flags, std::move(factory_func));
			}

			void Clear(void* collection) const override { reinterpret_cast<MapType*>(collection)->clear(); }
			void* GetKeyTemporaryStorage() const override { return reinterpret_cast<void*>(&TempKey); }
			void* GetValueTemporaryStorage() const override { return reinterpret_cast<void*>(&TempValue); }

			std::vector<const void*> GetKeys(const void* collection) const override
			{
				std::vector<const void*> ret;
				const MapType& map = *reinterpret_cast<const MapType*>(collection);
				for (const auto& kv : map)
					ret.push_back((const void*)&(kv.first));
				return ret;
			}

			void SetValue(void* collection, void* key, void* value) const override
			{
				KeyType& keyRef = *reinterpret_cast<KeyType*>(key);
				ValueType& valueRef = *reinterpret_cast<ValueType*>(value);
				(*reinterpret_cast<MapType*>(collection))[std::move(keyRef)] = std::move(valueRef);
			}

			const void* GetValue(const void* collection, const void* key) const override
			{
				const KeyType& keyRef = *reinterpret_cast<const KeyType*>(key);
				const ValueType& valueRef = reinterpret_cast<const MapType*>(collection)->at(keyRef);
				return reinterpret_cast<const void*>(&valueRef);
			}
		};

		template <typename KeyType, typename ValueType> Property CreateStdMapPropertyInfo(size_t offset, const char* name, ePropertyFlag flags)
		{
			std::shared_ptr<DictionaryPropertyImplDataBase> implData = std::shared_ptr<DictionaryPropertyImplDataBase>{ new StdMapPropertyImplData<KeyType, ValueType>(flags) };
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::MAP, std::move(implData) };
		}

		template <typename KeyType, typename ValueType> Property CreateStdMapPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t<ValueType>&& factory_func)
		{
			std::shared_ptr<DictionaryPropertyImplDataBase> implData = std::shared_ptr<DictionaryPropertyImplDataBase>{ new StdMapPropertyImplData<KeyType, ValueType>(flags, std::move(factory_func)) };
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::MAP, std::move(implData) };
		}

				//-----------------------------------------------------------------------------------------------------------------------
		// std::map serialization property impl
		template <typename KeyType, typename ValueType>
		struct StdUnorderedMapPropertyImplData final : public DictionaryPropertyImplDataBase
		{
			mutable KeyType TempKey;
			mutable ValueType TempValue;

			using MapType = std::unordered_map<KeyType, ValueType>;

			StdUnorderedMapPropertyImplData(ePropertyFlag flags)
			{
				KeyPropertyType = CreatePropertyInfo<KeyType, void>(0, "key", flags, nullptr);
				ValuePropertyType = CreatePropertyInfo<ValueType, void>(0, "value", flags, nullptr);
			}

			StdUnorderedMapPropertyImplData(ePropertyFlag flags, FactoryFunc_t<ValueType>&& factory_func)
			{
				KeyPropertyType = CreatePropertyInfo<KeyType, void>(0, "key", flags, nullptr);
				ValuePropertyType = CreatePropertyInfo<ValueType, ValueType>(0, "value", flags, std::move(factory_func));
			}

			void Clear(void* collection) const override { reinterpret_cast<MapType*>(collection)->clear(); }
			void* GetKeyTemporaryStorage() const override { return reinterpret_cast<void*>(&TempKey); }
			void* GetValueTemporaryStorage() const override { return reinterpret_cast<void*>(&TempValue); }

			std::vector<const void*> GetKeys(const void* collection) const override
			{
				std::vector<const void*> ret;
				const MapType& map = *reinterpret_cast<const MapType*>(collection);
				for (const auto& kv : map)
					ret.push_back((const void*)&(kv.first));
				return ret;
			}

			void SetValue(void* collection, void* key, void* value) const override
			{
				KeyType& keyRef = *reinterpret_cast<KeyType*>(key);
				ValueType& valueRef = *reinterpret_cast<ValueType*>(value);
				(*reinterpret_cast<MapType*>(collection))[std::move(keyRef)] = std::move(valueRef);
			}

			const void* GetValue(const void* collection, const void* key) const override
			{
				const KeyType& keyRef = *reinterpret_cast<const KeyType*>(key);
				const ValueType& valueRef = reinterpret_cast<const MapType*>(collection)->at(keyRef);
				return reinterpret_cast<const void*>(&valueRef);
			}
		};

		template <typename KeyType, typename ValueType> Property CreateStdUnorderedMapPropertyInfo(size_t offset, const char* name, ePropertyFlag flags)
		{
			std::shared_ptr<DictionaryPropertyImplDataBase> implData = std::shared_ptr<DictionaryPropertyImplDataBase>{ new StdUnorderedMapPropertyImplData<KeyType, ValueType>(flags) };
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::MAP, std::move(implData) };
		}

		template <typename KeyType, typename ValueType> Property CreateStdUnorderedMapPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t<ValueType>&& factory_func)
		{
			std::shared_ptr<DictionaryPropertyImplDataBase> implData = std::shared_ptr<DictionaryPropertyImplDataBase>{ new StdUnorderedMapPropertyImplData<KeyType, ValueType>(flags, std::move(factory_func)) };
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::MAP, std::move(implData) };
		}

		//-----------------------------------------------------------------------------------------------------------------------
		// OrderedMap serialization property impl
		template <typename EnumType, typename ValueType>
		struct EnumArrayPropertyImplData final : public DictionaryPropertyImplDataBase
		{
			mutable EnumType TempKey;
			mutable ValueType TempValue;
			mutable std::vector<EnumType> EnumValues;

			using EnumArrayType = ::pe::core::utils::EnumArray<ValueType, EnumType>;

			EnumArrayPropertyImplData(ePropertyFlag flags)
			{
				KeyPropertyType = CreatePropertyInfo<EnumType, void>(0, "key", flags, nullptr);
				ValuePropertyType = CreatePropertyInfo<ValueType, void>(0, "value", flags, nullptr);
			}

			EnumArrayPropertyImplData(ePropertyFlag flags, FactoryFunc_t<ValueType>&& factory_func)
			{
				KeyPropertyType = CreatePropertyInfo<EnumType, void>(0, "key", flags, nullptr);
				ValuePropertyType = CreatePropertyInfo<ValueType, ValueType>(0, "value", flags, std::move(factory_func));
			}

			void Clear(void* collection) const override 
			{ 
				EnumArrayType& col = *reinterpret_cast<EnumArrayType*>(collection);
				for (EnumType e : ::pe::core::utils::IterateEnum<EnumType>())
					col[e] = ValueType{};
			}
			void* GetKeyTemporaryStorage() const override { return reinterpret_cast<void*>(&TempKey); }
			void* GetValueTemporaryStorage() const override { return reinterpret_cast<void*>(&TempValue); }

			std::vector<const void*> GetKeys(const void* collection) const override
			{
				UNUSED(collection);
				if (EnumValues.size() == 0)
				{
					for (EnumType e : ::pe::core::utils::IterateEnum<EnumType>())
						EnumValues.push_back(e);
				}

				std::vector<const void*> ret;
				for (const EnumType& key : EnumValues)
					ret.push_back((const void*)&key);
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

		template <typename EnumType, typename ValueType> Property CreateEnumArrayPropertyInfo(size_t offset, const char* name, ePropertyFlag flags)
		{
			std::shared_ptr<DictionaryPropertyImplDataBase> implData =
				std::shared_ptr<DictionaryPropertyImplDataBase>{ new EnumArrayPropertyImplData<EnumType, ValueType>(flags) };
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::ENUM_ARRAY, std::move(implData) };
		}

		template <typename EnumType, typename ValueType> Property CreateEnumArrayPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t<ValueType>&& factory_func)
		{
			std::shared_ptr<DictionaryPropertyImplDataBase> implData =
				std::shared_ptr<DictionaryPropertyImplDataBase>{ new EnumArrayPropertyImplData<EnumType, ValueType>(flags, std::move(factory_func)) };
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
			i64 GetValue(const void* collection) const override { return (typename ::pe::core::utils::EnumFlags<E>::FlagType)*reinterpret_cast<const ::pe::core::utils::EnumFlags<E>*>(collection); }
			void SetValue(void* collection, i64 value) const override { *reinterpret_cast<::pe::core::utils::EnumFlags<E>*>(collection) = ::pe::core::utils::EnumFlags<E>((E)value); }
		};

		template <typename E> Property CreateEnumFlagsPropertyInfo(size_t offset, const char* name, ePropertyFlag flags)
		{
			STATIC_ASSERTE(std::is_enum<E>::value, "Enum type is required");
			using UnderlyingType = typename std::underlying_type<E>::type;
			STATIC_ASSERTE(std::is_integral<UnderlyingType>::value, "Only enums with integral underlying types are supported");
			STATIC_ASSERTE(std::is_signed<UnderlyingType>::value, "Only enums with signed underlying types are supported");
			STATIC_ASSERTE(sizeof(UnderlyingType) <= sizeof(i64), "Only enums with max 64 bit underlying types are supported");

			std::shared_ptr<EnumFlagsPropertyImplData<E>> implData = std::make_shared<EnumFlagsPropertyImplData<E>>();

			// Register EnumInfo object pointer to property
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::ENUM_FLAGS, std::move(implData) };
		}

		//-----------------------------------------------------------------------------------------------------------------------
		// Iterable Pool Allocator serialization property impl
/*		struct IterablePoolAllocatorPropertyImplDataBase : public PropertyImplData
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
				const ::pe::core::memory::IterablePoolAllocator<T>& allocator = *reinterpret_cast<const ::pe::core::memory::IterablePoolAllocator<T>*>(collection);
				//@todo(muniu) implement
				return nullptr;
			}

			const void* Begin(const void* collection) const override
			{
				const ::pe::core::memory::IterablePoolAllocator<T>& allocator = *reinterpret_cast<const ::pe::core::memory::IterablePoolAllocator<T>*>(collection);
				//@todo(muniu) implement
				return nullptr;// allocator.Begin();
			}

			void* Alloc(void* collection) const override
			{
				::pe::core::memory::IterablePoolAllocator<T>& allocator = *reinterpret_cast<::pe::core::memory::IterablePoolAllocator<T>*>(collection);
				return allocator.Alloc();
			}
		};

		template <typename E> Property CreateIterablePoolAllocatorPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t<E> factory_func)
		{
			std::shared_ptr<IterablePoolAllocatorPropertyImplData<E>> implData = std::make_shared<IterablePoolAllocatorPropertyImplData<E>>();

			// Register EnumInfo object pointer to property
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::ITERABLE_POOL_ALLOCATOR, std::move(implData), factory_func };
		}*/

		//-----------------------------------------------------------------------------------------------------------------------
		// Unique ptr serialization property impl
		struct UniquePtrPropertyImplDataBase : public PropertyImplData
		{
			Property PropertyType;

			virtual void Create(RTTIBase* parent, void* ptr) const = 0;
			virtual void CreatePolymorphic(RTTIBase* parent, void* ptr, const char* typeName) const = 0;
			virtual void Clear(void* ptr) const = 0;
			virtual void* Get(void* ptr) const = 0;
			virtual const void* Get(const void* ptr) const = 0;
		};

		template<typename T, typename D>
		struct UniquePtrPropertyImplData final : public UniquePtrPropertyImplDataBase
		{
			UniquePtrPropertyImplData(ePropertyFlag flags)
			{
				PropertyType = CreatePropertyInfo<T, void>(0, "value", flags, nullptr);
			}

			UniquePtrPropertyImplData(ePropertyFlag flags, FactoryFunc_t<std::unique_ptr<T, D>>&& factory_func)
			{
				m_factory = std::move(factory_func);
				PropertyType = CreatePropertyInfo<T, void>(0, "value", flags, nullptr);
			}

			void Create(RTTIBase* parent, void* ptr) const override { 
				std::unique_ptr<T,D>& uptr = *reinterpret_cast<std::unique_ptr<T,D>*>(ptr);

				if (m_factory)
					uptr = m_factory(parent, PropertyType.Type);
				else
				{
					if constexpr (std::is_default_constructible_v<T>)
					{
						uptr.reset(new T());
					}
					else
						ASSERTE(false, "Invalid construction method!");
				}
			}

			void CreatePolymorphic(RTTIBase* parent, void* ptr, const char* typeName) const override {
				std::unique_ptr<T, D>& uptr = *reinterpret_cast<std::unique_ptr<T, D>*>(ptr);

				TypeInfo type = RTTI::Impl::TypeManager::Get().GetTypeByName(typeName);
				if (type.IsValid())
				{
					if (m_factory)
						uptr = m_factory(parent, PropertyType.Type);
					else
						uptr.reset((T*)type.CreateInstance());
				}
				else
					::pe::core::utils::gConsole.LogError("Failed to create instance of type {}. Probably not registered!", typeName);

			}

			void Clear(void* ptr) const override { return (*reinterpret_cast<std::unique_ptr<T, D>*>(ptr)).reset(); }
			void* Get(void* ptr) const override { return (*reinterpret_cast<std::unique_ptr<T,D>*>(ptr)).get(); }
			const void* Get(const void* ptr) const override { return (*reinterpret_cast<const std::unique_ptr<T,D>*>(ptr)).get(); }

			FactoryFunc_t<std::unique_ptr<T, D>> m_factory = nullptr;
		};

		template <typename T, typename D> Property CreateUniquePtrPropertyInfo(size_t offset, const char* name, ePropertyFlag flags)
		{
			std::shared_ptr<UniquePtrPropertyImplData<T, D>> implData = std::make_shared<UniquePtrPropertyImplData<T, D>>(flags);

			// Register unique ptr object pointer to property
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::UNIQUE_PTR, std::move(implData), nullptr };
		}

		template <typename T, typename D> Property CreateUniquePtrPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t<std::unique_ptr<T,D>>&& factory_func)
		{
			FactoryFunc_t<std::unique_ptr<T, D>> cpy = factory_func;
			std::shared_ptr<UniquePtrPropertyImplData<T, D>> implData = std::make_shared<UniquePtrPropertyImplData<T, D>>(flags, std::move(cpy));

			// Register unique ptr object pointer to property
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::UNIQUE_PTR, std::move(implData), CreateFactoryStorage(std::move(factory_func)) };
		}

		//-----------------------------------------------------------------------------------------------------------------------
		// Raw ptr serialization property impl
		struct RawPtrPropertyImplDataBase : public PropertyImplData
		{
			Property PropertyType;

			virtual void* GetPtr(RTTIBase* ptr) const = 0;
			virtual void SetPtr(RTTIBase** ptr_dest, RTTIBase* ptr_src) const = 0;
		};

		template<typename T>
		struct RawPtrPropertyImplData final : public RawPtrPropertyImplDataBase
		{
			RawPtrPropertyImplData(ePropertyFlag flags)
			{
				// Stop propagation of factory function
				PropertyType = CreatePropertyInfo<T, void>(0, "value", flags, nullptr);
			}

			void* GetPtr(RTTIBase* ptr) const override
			{
				return rtti_cast<T*>(ptr);
			}
			
			void SetPtr(RTTIBase** ptr_dest, RTTIBase* ptr_src) const override
			{
				T** p = reinterpret_cast<T**>(ptr_dest);
				*p = rtti_cast<T*>(ptr_src);
			}
		};

		template <typename T> Property CreateRawPtrPropertyInfo(size_t offset, const char* name, ePropertyFlag flags)
		{
			std::shared_ptr<RawPtrPropertyImplData<T>> implData = std::make_shared<RawPtrPropertyImplData<T>>(flags);

			// Register unique ptr object pointer to property
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::RAW_PTR, std::move(implData), nullptr };
		}

		template <typename T> Property CreateRawPtrPropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t<T*>&& factory_func)
		{
			std::shared_ptr<RawPtrPropertyImplData<T>> implData = std::make_shared<RawPtrPropertyImplData<T>>(flags);

			// Register unique ptr object pointer to property
			return Property{ TypeInfo::INVALID, offset, name, flags, eCorePropertyType::RAW_PTR, std::move(implData), CreateFactoryStorage(std::move(factory_func)) };
		}

		//-----------------------------------------------------------------------------------------------------------------------
		template <typename T, typename R> inline Property CreatePropertyInfo(size_t offset, const char* name, ePropertyFlag flags, FactoryFunc_t<R>&& factory_func)
		{ 
			if constexpr (std::is_same<R, void>::value)
			{
				if constexpr (std::is_enum<T>::value)
					return CreateEnumPropertyInfo<T>(offset, name, flags);
				else if constexpr (Trait::IsStdVector<T>::value)
					return CreateStdVectorPropertyInfo<typename Trait::StdVectorValueType<T>::type>(offset, name, flags);
				else if constexpr (Trait::IsStdMap<T>::value)
					return CreateStdMapPropertyInfo<typename Trait::StdMapType<T>::keyType, typename Trait::StdMapType<T>::valueType>(offset, name, flags);
				else if constexpr (Trait::IsStdUnorderedMap<T>::value)
					return CreateStdUnorderedMapPropertyInfo<typename Trait::StdUnorderedMapType<T>::keyType, typename Trait::StdUnorderedMapType<T>::valueType>(offset, name, flags);
				else if constexpr (Trait::IsEnumArray<T>::value)
					return CreateEnumArrayPropertyInfo<typename Trait::EnumArrayType<T>::enumType, typename Trait::EnumArrayType<T>::valueType>(offset, name, flags);
				else if constexpr (Trait::IsEnumFlags<T>::value)
					return CreateEnumFlagsPropertyInfo<typename Trait::EnumFlagsType<T>::type>(offset, name, flags);
				else if constexpr (Trait::IsUniquePtr<T>::value)
					return CreateUniquePtrPropertyInfo<typename Trait::UniquePtrType<T>::type, typename Trait::UniquePtrType<T>::deleter>(offset, name, flags);
				else if constexpr (std::is_pointer<T>::value)
					return CreateRawPtrPropertyInfo<typename std::remove_pointer<T>::type>(offset, name, flags);
				else if constexpr (std::is_base_of<RTTIBase, T>::value)
					return Property{ TypeInfo::Get<T>(), offset, name, flags, GetCorePropertyType<T>(), nullptr, nullptr };
				else
					return Property{ TypeInfo::INVALID, offset, name, flags, GetCorePropertyType<T>(), nullptr, nullptr };
			}
			else
			{
				if constexpr (std::is_enum<T>::value)
					return CreateEnumPropertyInfo<T>(offset, name, flags);
				else if constexpr (Trait::IsStdVector<T>::value)
					return CreateStdVectorPropertyInfo<typename Trait::StdVectorValueType<T>::type>(offset, name, flags, std::move(factory_func));
				else if constexpr (Trait::IsStdMap<T>::value)
					return CreateStdMapPropertyInfo<typename Trait::StdMapType<T>::keyType, typename Trait::StdMapType<T>::valueType>(offset, name, flags, std::move(factory_func));
				else if constexpr (Trait::IsStdUnorderedMap<T>::value)
					return CreateStdUnorderedMapPropertyInfo<typename Trait::StdUnorderedMapType<T>::keyType, typename Trait::StdUnorderedMapType<T>::valueType>(offset, name, flags, std::move(factory_func));
				else if constexpr (Trait::IsEnumArray<T>::value)
					return CreateEnumArrayPropertyInfo<typename Trait::EnumArrayType<T>::enumType, typename Trait::EnumArrayType<T>::valueType>(offset, name, flags, std::move(factory_func));
				else if constexpr (Trait::IsEnumFlags<T>::value)
					return CreateEnumFlagsPropertyInfo<typename Trait::EnumFlagsType<T>::type>(offset, name, flags);
				else if constexpr (Trait::IsUniquePtr<T>::value)
					return CreateUniquePtrPropertyInfo<typename Trait::UniquePtrType<T>::type, typename Trait::UniquePtrType<T>::deleter>(offset, name, flags, std::move(factory_func));
				else if constexpr (std::is_pointer<T>::value)
					return CreateRawPtrPropertyInfo<typename std::remove_pointer<T>::type>(offset, name, flags, std::move(factory_func));
				else if constexpr (std::is_base_of<RTTIBase, T>::value)
					return Property{ TypeInfo::Get<T>(), offset, name, flags, GetCorePropertyType<T>(), nullptr, CreateFactoryStorage(std::move(factory_func)) };
				else
					return Property{ TypeInfo::INVALID, offset, name, flags, GetCorePropertyType<T>(), nullptr, CreateFactoryStorage(std::move(factory_func)) };
			}
		}

		//-----------------------------------------------------------------------------------------------------------------------
		class CORE_DLLEXPORT PropertyManagerBase : public ::pe::core::BaseObject<> {
		public:
			void AddProperty(Property&& property) { Properties.push_back(std::move(property)); }
			const std::vector<Property>& GetPropertyList() const { return Properties; };

		protected:
			std::vector<Property> Properties;
		};

		template<class T>
		class PropertyManager : public PropertyManagerBase {
		public:
			PropertyManager() { T::InitPropertiesBase(this); T::InitProperties(this); }
			~PropertyManager() { }
		};

	} // namespace RTTI
} // namespace Poly

  // Declares type with no base class
#define RTTI_DECLARE_TYPE(T) \
	public: \
	RTTI_GENERATE_TYPE_INFO(T)\
	virtual Poly::RTTI::TypeInfo GetTypeInfo() const { return Poly::RTTI::Impl::GetTypeInfoFromInstance(this); } \
	typedef TYPE_LIST() baseClassList;\
	RTTI_GENERATE_PROPERTY_LIST_BASE(T)


  // Declares type with one base class
#define RTTI_DECLARE_TYPE_DERIVED(T,A) \
	public: \
	RTTI_GENERATE_TYPE_INFO(T)\
	Poly::RTTI::TypeInfo GetTypeInfo() const override { return Poly::RTTI::Impl::GetTypeInfoFromInstance(this); } \
	typedef TYPE_LIST_1(A) baseClassList;\
	RTTI_GENERATE_PROPERTY_LIST(T,A)

  // Declares type with two base classes. Disabled for now.
  /*#define RTTI_DECLARE_TYPE_DERIVED2(T,A,B) \
  public: \
  RTTI_GENERATE_TYPE_INFO(T)\
  Poly::RTTI::TypeInfo GetTypeInfo() const override { return Poly::RTTI::Impl::GetTypeInfoFromInstance(this); } \
  typedef TYPE_LIST_2(A,B) baseClassList;\
  RTTI_GENERATE_PROPERTY_LIST(T)*/


#define RTTI_GENERATE_PROPERTY_LIST_BASE(Type)\
	friend class ::Poly::RTTI::PropertyManager<Type>; \
	virtual ::Poly::RTTI::PropertyManagerBase* GetPropertyManager() const; \
	template <class T> \
	static void InitPropertiesBase(Poly::RTTI::PropertyManager<T>*) {} \
	template <class T> \
	static void InitProperties(Poly::RTTI::PropertyManager<T>* mgr)

#define RTTI_GENERATE_PROPERTY_LIST(Type, BaseType)\
	friend class Poly::RTTI::PropertyManager<Type>; \
	Poly::RTTI::PropertyManagerBase* GetPropertyManager() const override; \
	template <class T> \
	static void InitPropertiesBase(Poly::RTTI::PropertyManager<T>* mgr) { BaseType::InitPropertiesBase(mgr); BaseType::InitProperties(mgr); } \
	template <class T> \
	static void InitProperties(Poly::RTTI::PropertyManager<T>* mgr)

#define RTTI_PROPERTY_MANAGER_IMPL(Type)\
	::Poly::RTTI::PropertyManagerBase* Type::GetPropertyManager() const { static ::Poly::RTTI::PropertyManager<Type> instance; return &instance; }

#define NO_RTTI_PROPERTY() UNUSED(mgr)


namespace Poly::RTTI::Impl {
	template<typename T>
	void RegisterProperty(PropertyManagerBase* mgr, size_t offset, const char* var_name, ePropertyFlag flags)
	{
		ASSERTE((!std::is_pointer<T>::value 
			|| ::pe::core::utils::EnumFlags<Poly::RTTI::ePropertyFlag>(flags).IsSet(Poly::RTTI::ePropertyFlag::DONT_SERIALIZE) 
			|| std::is_base_of<RTTIBase, typename std::remove_pointer<T>::type>::value), "Serializable variable cannot be a pointer of not RTTIBase deriving type.");

		mgr->AddProperty(Poly::RTTI::CreatePropertyInfo<T, void>(offset, var_name, flags, nullptr));
	}

	template<typename T, typename Foo>
	void RegisterPropertyFactory(PropertyManagerBase* mgr, size_t offset, const char* var_name, Foo&& factory_func, ePropertyFlag flags)
	{
		ASSERTE((!std::is_pointer<T>::value
			|| ::pe::core::utils::EnumFlags<Poly::RTTI::ePropertyFlag>(flags).IsSet(Poly::RTTI::ePropertyFlag::DONT_SERIALIZE)
			|| std::is_base_of<RTTIBase, typename std::remove_pointer<T>::type>::value), "Serializable variable cannot be a pointer of not RTTIBase deriving type.");



		mgr->AddProperty(Poly::RTTI::CreatePropertyInfo<T, typename Foo::result_type>(offset, var_name, flags, std::move(factory_func)));
	}
}

// Standard RTTIBase deriving (or POD type) property
#define RTTI_PROPERTY(variable, var_name, flags) ::Poly::RTTI::Impl::RegisterProperty<decltype(variable)>(mgr, ::Poly::RTTI::OffsetOfMember(&T::variable), var_name, flags)
#define RTTI_PROPERTY_AUTONAME(variable, flags) RTTI_PROPERTY(variable, #variable, flags)

// Factory supported rtti property
#define RTTI_PROPERTY_FACTORY(variable, var_name, factory_func, flags) \
	::Poly::RTTI::Impl::RegisterPropertyFactory<decltype(variable)>\
						(mgr, ::Poly::RTTI::OffsetOfMember(&T::variable), var_name, ::pe::core::utils::make_function(factory_func), flags)
#define RTTI_PROPERTY_FACTORY_AUTONAME(variable, factory_func, flags) RTTI_PROPERTY_FACTORY(variable, #variable, factory_func, flags)
