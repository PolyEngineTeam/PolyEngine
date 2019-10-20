#pragma once

#include <pe/Defines.hpp>

namespace pe::core::storage
{

///	@brief Wrapper class for c-style strings (char*).
/// This class implements a zero overhead abstraction over c-style strings.
/// This class is guaranteed to be allocation free. It should be preferred to usual const char*.
/// @warning This is a non-owning abstraction. The lifetime of the c-string char array 
/// should always be the same or longer than the instance of this class.
/// Having shorter lifetime will result in an undefined behaviour.
class CORE_DLLEXPORT CString final : public BaseObjectLiteralType<>
{
public:
	/// @brief Implicit CString constructor, takes c-style string and stores the pointer.
	/// @note It does not change the ownership of the charater array.
	constexpr CString(const char* str) : m_str(str) { HEAVY_ASSERTE(m_str, "CString cannot hold nullptr!"); }

	/// @brief Lexicographical equality comparison operator.
	/// @param[in] rhs Right hand side of the comparison.
	/// @return True if strings are equal, false otherwise.
	bool operator==(const CString& rhs) const { return strcmp(m_str, rhs.m_str) == 0; }

	/// @brief Lexicographical inequality comparison operator.
	/// @param[in] rhs Right hand side of the comparison.
	/// @return True if strings are not equal, false otherwise.
	bool operator!=(const CString& rhs) const { return !(*this == rhs); }
	
	/// @brief Lexicographical less ordering comparison operator.
	/// @param[in] rhs Right hand side of the ordering comparison.
	/// @return True if the lhs comes before rhs in lexicographical order, false otherwise.
	bool operator<(const CString& rhs) const { return strcmp(m_str, rhs.m_str) < 0; }

	/// @brief Lexicographical greater ordering comparison operator.
	/// @param[in] rhs Right hand side of the ordering comparison.
	/// @return True if the lhs comes after rhs in lexicographical order, false otherwise.
	bool operator>(const CString& rhs) const { return strcmp(m_str, rhs.m_str) > 0; }

	/// @brief Lexicographical less or equal ordering comparison operator.
	/// @param[in] rhs Right hand side of the ordering comparison.
	/// @return True if the lhs comes before rhs in lexicographical order or is equal, false otherwise.
	bool operator<=(const CString& rhs) const { return strcmp(m_str, rhs.m_str) <= 0; }

	/// @brief Lexicographical greater or equal ordering comparison operator.
	/// @param[in] rhs Right hand side of the ordering comparison.
	/// @return True if the lhs comes after rhs in lexicographical order or is equal, false otherwise.
	bool operator>=(const CString& rhs) const { return strcmp(m_str, rhs.m_str) >= 0; }

	/// @brief String indexing operator
	/// @param[in] idx Index of the character.
	/// @return Character from string at position pointed by idx.
	constexpr char operator[](size_t idx) const { return m_str[idx]; }

	/// @brief Implicit cast to c-style string pointer.
	/// @note Does not modify the ownership.
	/// @return C-style string pointer represented by this class.
	constexpr operator const char*() const { return m_str; }

	/// @brief Getter for stored c-style string pointer.
	/// @note Does not modify the ownership.
	/// @return C-style string pointer represented by this class.
	constexpr const char* get() const { return m_str; }

	/// @brief Calculates the length of the string.
	/// @return Length of the string.
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