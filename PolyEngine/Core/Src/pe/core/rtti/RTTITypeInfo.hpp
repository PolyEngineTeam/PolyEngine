#pragma once

#include <pe/Defines.hpp>

#include <pe/core/memory/ObjectLifetimeHelpers.hpp>

namespace Poly 
{
	namespace RTTI {
		namespace Impl
		{
			class TypeManager;
		}

		  //--------------------------------------------------------------------------
		class CORE_DLLEXPORT TypeInfo final : public BaseObjectLiteralType<> {
		public:
			static const TypeInfo INVALID;

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
			constexpr static TypeInfo Get() { return T::MetaTypeInfo::GetTypeInfo(); }

			template<typename T>
			static TypeInfo Get(T* object) { return object->GetTypeInfo(); }

			template<typename T>
			static TypeInfo Get(T& object) { return object.GetTypeInfo(); }

			template<typename T>
			inline bool isTypeDerivedFrom() const;

			const char* GetTypeName() const;

			void* CreateInstance() const;
			void* CreateInstanceInPlace(void* ptr) const;

			CORE_DLLEXPORT friend std::ostream& operator<<(std::ostream& stream, const TypeInfo& typeInfo);

		private:
			TypeInfo(TypeId id);

			friend Impl::TypeManager;

		private:
			TypeId ID = 0;
		};

		namespace Impl {

			//--------------------------------------------------------------------------
			class CORE_DLLEXPORT TypeManager final : public BaseObjectLiteralType<>{
			public:
				static TypeManager& Get();

				template <typename T>
				TypeInfo RegisterOrGetType(const char* name, const std::vector<TypeInfo>& baseClassList)
				{
					if (NameToTypeMap.find(name) != NameToTypeMap.end())
						return NameToTypeMap[name];
					else {
						TypeInfo ti(++Counter);
						NameToTypeMap.insert(std::make_pair(name, ti));
						TypeToNameMap.insert(std::make_pair(ti, name));

						ConstructorsMap.insert(std::make_pair(ti, [](void* memory)
						{
							return (void*)ObjectLifetimeHelper::DefaultAllocateAndCreate<T>((T*)memory);
						}));
						InheritanceListMap[ti] = baseClassList;
						return ti;
					}
				}
				bool IsTypeDerivedFrom(const TypeInfo& checked, const TypeInfo& from) const;

				const char* GetTypeName(const TypeInfo& typeInfo) const;
				const std::function<void*(void*)>& GetConstructor(const TypeInfo& typeInfo) const;
				TypeInfo GetTypeByName(const char* name) const;

				void Clear();
			private:
				TypeManager() = default;
				TypeManager(const TypeManager& rhs) = delete;
				TypeManager& operator=(const TypeManager& rhs) = delete;

				long long Counter = 0;
				std::map<std::string, TypeInfo> NameToTypeMap;
				std::map<TypeInfo, const char*> TypeToNameMap;
				std::map<TypeInfo, std::vector<TypeInfo>> InheritanceListMap;
				std::map<TypeInfo, std::function<void*(void*)>> ConstructorsMap;
			};

		} // namespace Impl

		template<typename T>
		inline bool TypeInfo::isTypeDerivedFrom() const
		{
			return Impl::TypeManager::Get().IsTypeDerivedFrom(*this, Get<T>());
		}

	} // namespace RTTI
} // namespace Poly

#include <pe/core/rtti/RTTITypeInfoImpl.hpp>