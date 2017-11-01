#pragma once

#include "Defines.hpp"

#include <type_traits>
#include <map>
#include <vector>

namespace Poly 
{
	namespace RTTI {
		class TypeInfo;

		template<typename T, typename std::enable_if<std::is_fundamental<T>::value || std::is_enum<T>::value>::type* = nullptr>
		constexpr TypeInfo GetUnifiedTypeInfo();
		template<typename T, typename std::enable_if<!std::is_fundamental<T>::value && !std::is_enum<T>::value>::type* = nullptr>
		constexpr TypeInfo GetUnifiedTypeInfo();


		namespace Impl {

			//--------------------------------------------------------------------------
			class CORE_DLLEXPORT TypeManager : public BaseObjectLiteralType<> {
			public:
				static TypeManager& Get();
				TypeInfo RegisterOrGetType(const char* name, const Dynarray<TypeInfo>& baseClassList);
				bool IsTypeDerivedFrom(const TypeInfo& checked, const TypeInfo& from) const;

				const char* GetTypeName(const TypeInfo& typeInfo) const;
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
		class CORE_DLLEXPORT TypeInfo : public BaseObjectLiteralType<> {
		public:
			typedef long long TypeId;

			constexpr TypeInfo() : ID(0) {}
            constexpr TypeInfo(const TypeInfo& rhs) : ID(rhs.ID) {}
            TypeInfo& operator=(const TypeInfo& rhs) { ID = rhs.ID; return *this; }
            constexpr bool operator<(const TypeInfo& rhs) const { return ID < rhs.ID; }
            constexpr bool operator>(const TypeInfo& rhs) const { return ID > rhs.ID; }
            constexpr bool operator<=(const TypeInfo& rhs) const { return ID <= rhs.ID; }
            constexpr bool operator>=(const TypeInfo& rhs) const { return ID >= rhs.ID; }
            constexpr bool operator==(const TypeInfo& rhs) const { return ID == rhs.ID; }
            constexpr bool operator!=(const TypeInfo& rhs) const { return ID != rhs.ID; }
            constexpr bool IsValid() const  { return ID != 0; }

			template<typename T>
			constexpr static TypeInfo Get() { return GetUnifiedTypeInfo<typename std::remove_cv<T>::type>(); }

			template<typename T>
			static TypeInfo Get(T* object) { UNUSED(object); return Get<typename std::remove_pointer<T>::type>(); }

			template<typename T>
			static TypeInfo Get(T& object) { UNUSED(object); return Get<typename std::remove_reference<T>::type>(); }

			template<typename T>
			inline bool isTypeDerivedFrom() const {
				return Impl::TypeManager::Get().IsTypeDerivedFrom(*this, GetUnifiedTypeInfo<T>());
			}

			const char* GetTypeName() const;

			CORE_DLLEXPORT friend std::ostream& operator<<(std::ostream& stream, const TypeInfo& typeInfo);

		private:
			TypeInfo(TypeId id);

			friend class Impl::TypeManager;

		private:
			TypeId ID = 0;
		};

	} // namespace RTTI
} // namespace Poly

#include "RTTITypeInfoImpl.hpp"