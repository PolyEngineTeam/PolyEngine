#pragma once

#pragma once

#include <pe/Defines.hpp>

namespace pe::core::storage
{

class CORE_DLLEXPORT CString final : public BaseObjectLiteralType<>
{
public:
	CString(const char* str) : m_str(str) { HEAVY_ASSERTE(m_str, "CString cannot hold nullptr!"); }

	bool operator==(const CString& rhs) const { return strcmp(m_str, rhs.m_str) == 0; }
	bool operator!=(const CString& rhs) const { return !(*this == rhs); }
	
	bool operator<(const CString& rhs) const { return strcmp(m_str, rhs.m_str) < 0; }
	bool operator>(const CString& rhs) const { return strcmp(m_str, rhs.m_str) > 0; }
	bool operator<=(const CString& rhs) const { return strcmp(m_str, rhs.m_str) <= 0; }
	bool operator>=(const CString& rhs) const { return strcmp(m_str, rhs.m_str) >= 0; }

	char operator[](size_t idx) const { return m_str[idx]; }
	operator const char*() const { return m_str; }

	const char* get() const { return m_str; }

	size_t length() const { return strlen(m_str); }

	friend std::ostream& operator<< (std::ostream& stream, const CString& rhs) { return stream << rhs.m_str; }

private:
	const char* const m_str;
};

}

namespace std
{
	template<> struct hash<::pe::core::storage::CString>
	{
		std::size_t operator()(::pe::core::storage::CString const& s) const noexcept
		{	
			unsigned long hash = 5381;
			size_t i = 0;
			while (s[i] != 0)
			{
				hash = ((hash << 5) + hash) + s[i];
				++i;
			}

			return hash;
		}
	};
}