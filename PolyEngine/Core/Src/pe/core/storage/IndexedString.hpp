#pragma once

#include <pe/Defines.hpp>
#include <pe/core/storage/String.hpp>

namespace pe::core::storage
{

namespace impl 
{
	class IndexedStringEntry;
}

/// @brief String class with indexing for optimization (memory wise and computation wise).
/// This string class ensures there is only one instance of a particullar string in the memory.
/// The string data is shared across many instances of IndexedString.
/// There are two major improvements related to that:
/// 	- less memory is being allocated to the same strings
///		- all comparison operations are O(1) complexity.
class CORE_DLLEXPORT IndexedString final : public core::BaseObjectLiteralType<>
{
public:
	/// @brief IndexedString constructor. Registers the string in the IndexedStringManager.
	/// @param[in] str String to be represented by the IndexedString instance.
	explicit IndexedString(std::string_view str);

	/// @brief IndexedString factory function. Registers the string in the IndexedStringManager.
	/// @note If the string is not registered yet, the memory is reused.
	/// @param[in] str String to be represented by the IndexedString instance.
	static IndexedString FromRString(core::storage::String&& str);

	/// @brief IndexedString destructor. Unregisters the string from the IndexedStringManager.
	~IndexedString();

	/// @brief Copy constructor for IndexedString. It handles ref count updating.
	/// @param[in] str IndexedString to copy.
	IndexedString(const IndexedString& str);

	/// @brief Copy assignment operator for IndexedString. It handles ref count updating.
	/// @param[in] str IndexedString to copy.
	/// @return Mutable reference to this.
	IndexedString& operator=(const IndexedString& rhs);

	/// @brief Equal comparison operator. This operation has O(1) complexity.
	/// @param[in] rhs IndexedString to compare with.
	/// @return True if strings are equal, false otherwise.
	bool operator==(const IndexedString& rhs) const;

	/// @brief Unequal comparison operator. This operation has O(1) complexity.
	/// @param[in] rhs IndexedString to compare with.
	/// @return True if strings are not equal, false otherwise.
	bool operator!=(const IndexedString& rhs) const { return !(*this == rhs); }

	/// @brief Less ordering comparison operator. This operation has O(1) complexity.
	/// @warning This operation does not apply lexicographical ordering.
	/// The only guarantee is that the ordering is deterministic within a single execution.
	/// @param[in] rhs IndexedString to compare with.
	/// @return True if lhs is earlier in the ordering space than rhs, false otherwise.
	bool operator<(const IndexedString& rhs) const;

	/// @brief Gets underlying String representation.
	/// @return Const reference to stored String object.
	const String& get() const;

	friend std::ostream& operator<< (std::ostream& stream, const IndexedString& rhs) { return stream << rhs.get(); }
private:
	explicit IndexedString(const impl::IndexedStringEntry* entry);

	const impl::IndexedStringEntry* m_entry = nullptr;

	friend struct std::hash<::pe::core::storage::IndexedString>;
};

}

namespace std
{
	template<> struct hash<::pe::core::storage::IndexedString>
	{
		std::size_t operator()(::pe::core::storage::IndexedString const& s) const noexcept
		{	
			return (size_t)s.m_entry;
		}
	};
}