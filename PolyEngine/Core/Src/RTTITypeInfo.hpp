#pragma once

#include "Defines.hpp"

#include <type_traits>
#include <map>
#include <vector>

namespace Poly {
    namespace RTTI {
        class TypeInfo;
    }
}

template <typename T>
struct MetaTypeInfo;

template<typename T> constexpr Poly::RTTI::TypeInfo GetCheckedTypeInfo(typename std::enable_if<std::is_fundamental<T>::value>::type* = 0);
template<typename T> constexpr Poly::RTTI::TypeInfo GetCheckedTypeInfo(typename std::enable_if<!std::is_fundamental<T>::value>::type* = 0);


namespace Poly {
	namespace RTTI {

		namespace Impl {

			//--------------------------------------------------------------------------
			template<typename T, bool>
			struct RawTypeInfo;

			//--------------------------------------------------------------------------
			class CORE_DLLEXPORT TypeManager {
			public:
				static TypeManager& Get();
				TypeInfo RegisterOrGetType(const char* name, const TypeInfo& rawTypeInfo, const Dynarray<TypeInfo>& baseClassList);
				bool IsTypeDerivedFrom(const TypeInfo& checked, const TypeInfo& from) const;

			private:
				TypeManager() = default;
				TypeManager(const TypeManager& rhs) = delete;
				TypeManager& operator=(const TypeManager& rhs) = delete;

				long long Counter = 0;
				std::map<const char*, TypeInfo> NameToTypeMap;
				std::map<TypeInfo, const char*> TypeToNameMap;
				std::map<TypeInfo, Dynarray<TypeInfo>> InheritanceListMap;
			};

		} // namespace Impl

		  //--------------------------------------------------------------------------
		class CORE_DLLEXPORT TypeInfo {
		public:
			typedef long long TypeId;

			constexpr TypeInfo() : m_id(0) {}
            constexpr TypeInfo(const TypeInfo& rhs) : m_id(rhs.m_id) {}
            TypeInfo& operator=(const TypeInfo& rhs) { m_id = rhs.m_id; return *this; }
            constexpr bool operator<(const TypeInfo& rhs) const { return m_id < rhs.m_id; }
            constexpr bool operator>(const TypeInfo& rhs) const { return m_id > rhs.m_id; }
            constexpr bool operator<=(const TypeInfo& rhs) const { return m_id <= rhs.m_id; }
            constexpr bool operator>=(const TypeInfo& rhs) const { return m_id >= rhs.m_id; }
            constexpr bool operator==(const TypeInfo& rhs) const { return m_id == rhs.m_id; }
            constexpr bool operator!=(const TypeInfo& rhs) const { return m_id != rhs.m_id; }
            constexpr bool IsValid() const  { return m_id != 0; }

			template<typename T>
			constexpr static TypeInfo Get() { return GetCheckedTypeInfo<typename std::remove_cv<T>::type>(); }

			template<typename T>
			static TypeInfo Get(T* object) { UNUSED(object); return Get<typename std::remove_pointer<T>::type>(); }

			template<typename T>
			static TypeInfo Get(T& object) { UNUSED(object); return Get<typename std::remove_reference<T>::type>(); }

			template<typename T>
			inline bool isTypeDerivedFrom() const {
				return Impl::TypeManager::Get().IsTypeDerivedFrom(*this, GetCheckedTypeInfo<T>());
			}

		private:
			TypeInfo(TypeId id);

			friend class Impl::TypeManager;

			template<typename T, bool>
			friend struct Impl::RawTypeInfo;
		private:
			TypeId m_id = 0;
		};

	} // namespace RTTI
} // namespace Poly

#include "RTTITypeInfoImpl.hpp"