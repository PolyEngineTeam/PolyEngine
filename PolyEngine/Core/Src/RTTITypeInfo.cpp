#include "CorePCH.hpp"
#include "RTTITypeInfo.hpp"

namespace Poly {
	namespace RTTI {
		namespace Impl {

			TypeManager& TypeManager::Get() {
				static TypeManager instance;
				return instance;
			}

			TypeInfo TypeManager::RegisterOrGetType(const char* name, const TypeInfo& rawTypeInfo, const std::vector<TypeInfo>& baseClassList) {
				UNUSED(rawTypeInfo);
				if (s_nameToTypeMap.find(name) != s_nameToTypeMap.end())
					return s_nameToTypeMap[name];
				else {
					TypeInfo ti(++s_counter);
					s_nameToTypeMap.insert(std::make_pair(name, ti));
					s_typeToNameMap.insert(std::make_pair(ti, name));
					s_inheritanceListMap[ti] = baseClassList;
					return ti;
				}
			}

			bool TypeManager::IsTypeDerivedFrom(const TypeInfo& checked, const TypeInfo& from) const {
				ASSERTE(checked.IsValid(), "Checked type is not a valid TypeInfo");
				ASSERTE(from.IsValid(), "From type is not a valid TypeInfo");

				if (checked == from)
					return true;

				for (auto& base : s_inheritanceListMap.at(checked)) {
					ASSERTE(base.IsValid(), "Base type is not a valid TypeInfo");
					if (from == base)
						return true;
				}

				return false;
			}

		} // namespace Impl

		  // TYPE INFO
		TypeInfo::TypeInfo() : m_id(0) {}
		TypeInfo::TypeInfo(long long id) : m_id(id) {}
		TypeInfo::TypeInfo(const TypeInfo& rhs) : m_id(rhs.m_id) {}
		TypeInfo& TypeInfo::operator=(const TypeInfo& rhs) { m_id = rhs.m_id; return *this; }
		bool TypeInfo::operator<(const TypeInfo& rhs) const { return m_id < rhs.m_id; }
		bool TypeInfo::operator>(const TypeInfo& rhs) const { return m_id > rhs.m_id; }
		bool TypeInfo::operator<=(const TypeInfo& rhs) const { return m_id <= rhs.m_id; }
		bool TypeInfo::operator>=(const TypeInfo& rhs) const { return m_id >= rhs.m_id; }
		bool TypeInfo::operator==(const TypeInfo& rhs) const { return m_id == rhs.m_id; }
		bool TypeInfo::operator!=(const TypeInfo& rhs) const { return m_id != rhs.m_id; }
		bool TypeInfo::IsValid() const { return m_id != 0; }

	}
}