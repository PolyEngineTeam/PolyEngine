#pragma once

#include "Defines.hpp"

#include <type_traits>
#include <map>
#include <vector>

template <typename T>
struct MetaTypeInfo;

namespace Poly {
	namespace RTTI {
		class TypeInfo;

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

			TypeInfo();
			TypeInfo(const TypeInfo& rhs);
			TypeInfo& operator=(const TypeInfo& rhs);
			bool operator<(const TypeInfo& rhs) const;
			bool operator>(const TypeInfo& rhs) const;
			bool operator<=(const TypeInfo& rhs) const;
			bool operator>=(const TypeInfo& rhs) const;
			bool operator==(const TypeInfo& rhs) const;
			bool operator!=(const TypeInfo& rhs) const;
			bool IsValid() const;

			template<typename T>
			static TypeInfo Get() { return GetCheckedTypeInfo<typename std::remove_cv<T>::type>(); }

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