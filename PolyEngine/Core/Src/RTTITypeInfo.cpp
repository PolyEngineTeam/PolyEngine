#include "CorePCH.hpp"
#include "RTTITypeInfo.hpp"

namespace Poly {
	namespace RTTI {
		namespace Impl {

			TypeManager& TypeManager::Get() {
				static TypeManager instance;
				return instance;
			}

			TypeInfo TypeManager::RegisterOrGetType(const char* name, const TypeInfo& rawTypeInfo, const Dynarray<TypeInfo>& baseClassList) {
				UNUSED(rawTypeInfo);
				if (NameToTypeMap.find(name) != NameToTypeMap.end())
					return NameToTypeMap[name];
				else {
					TypeInfo ti(++Counter);
					NameToTypeMap.insert(std::make_pair(name, ti));
					TypeToNameMap.insert(std::make_pair(ti, name));
					InheritanceListMap[ti] = baseClassList;
					return ti;
				}
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

		} // namespace Impl

		TypeInfo::TypeInfo(long long id) : m_id(id) {}
	}
}