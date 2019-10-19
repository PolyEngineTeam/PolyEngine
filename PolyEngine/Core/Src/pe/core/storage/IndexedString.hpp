#pragma once

#include <pe/Defines.hpp>
#include <pe/core/storage/String.hpp>

namespace pe::core::storage
{

namespace impl 
{
	class IndexedStringEntry;
}

class CORE_DLLEXPORT IndexedString final : public core::BaseObjectLiteralType<>
{
public:
	IndexedString(const char* str);
	~IndexedString();

	IndexedString(const IndexedString& str);
	IndexedString& operator=(const IndexedString& rhs);

	bool operator==(const IndexedString& str) const;
	bool operator!=(const IndexedString& rhs) const { return !(*this == rhs); }

	bool operator<(const IndexedString& rhs) const;

	const String& get() const;

	friend std::ostream& operator<< (std::ostream& stream, const IndexedString& rhs) { return stream << rhs.get(); }
private:

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