#include <CorePCH.hpp>

#include <RTTI/RTTITypeInfo.hpp>

namespace Poly {
	namespace RTTI {
		const TypeInfo TypeInfo::INVALID = TypeInfo();

		namespace Impl {

			TypeManager& TypeManager::Get() {
				static TypeManager instance;
				return instance;
			}

			bool TypeManager::IsTypeDerivedFrom(const TypeInfo& checked, const TypeInfo& from) const {
				ASSERTE(checked.IsValid(), "Checked type is not a valid TypeInfo");
				ASSERTE(from.IsValid(), "From type is not a valid TypeInfo");

				if (checked == from)
					return true;

				for (auto& base : InheritanceListMap.at(checked)) {
					ASSERTE(base.IsValid(), "Base type is not a valid TypeInfo");
					if (from == base)
						return true;
				}

				return false;
			}

			const char* TypeManager::GetTypeName(const TypeInfo& typeInfo) const
			{
				const auto& it = TypeToNameMap.find(typeInfo);
				ASSERTE(it != TypeToNameMap.end(), "Type has no name! Not registered?");
				return it->second;
			}

			const std::function<void*(void*)>& TypeManager::GetConstructor(const TypeInfo & typeInfo) const
			{
				const auto& it = ConstructorsMap.find(typeInfo);
				ASSERTE(it != ConstructorsMap.end(), "Type has no name! Not registered?");
				return it->second;
			}

			TypeInfo TypeManager::GetTypeByName(const char* name) const
			{
				const auto& it = NameToTypeMap.find(name);
				if (it == NameToTypeMap.end())
				{
					return TypeInfo::INVALID;
					HEAVY_ASSERTE(false, "Type has no name! Not registered?");
				}
				return it->second;
			}

		} // namespace Impl

		TypeInfo::TypeInfo(long long id) : ID(id) {}

		const char* TypeInfo::GetTypeName() const { return Impl::TypeManager::Get().GetTypeName(*this); }

		void * TypeInfo::CreateInstance() const
		{
			return Impl::TypeManager::Get().GetConstructor(*this)(nullptr);
		}

		void * TypeInfo::CreateInstanceInPlace(void * ptr) const
		{
			return Impl::TypeManager::Get().GetConstructor(*this)(ptr);
		}

		//------------------------------------------------------------------------------
		std::ostream& operator<< (std::ostream& stream, const TypeInfo& typeInfo) {
			return stream << "TypeInfo["<< typeInfo.ID << ", " << typeInfo.GetTypeName() << "]";
		}
	} // namespace RTTI
} // namespace Poly