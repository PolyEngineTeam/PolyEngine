#include <pe/core/storage/IndexedString.hpp>

#include <pe/core/storage/impl/IndexedStringManager.hpp>


namespace pe::core::storage
{

IndexedString::IndexedString(const char* str)
: m_entry(impl::IndexedStringManager::get().registerString(str))
{
	ASSERTE(m_entry, "Entry is null after string creation!");
}

IndexedString::~IndexedString()
{
	impl::IndexedStringManager::get().unregisterString(m_entry);
}

IndexedString::IndexedString(const IndexedString& str)
: m_entry(str.m_entry)
{
	m_entry->incrementRefCount();
}

IndexedString& IndexedString::operator=(const IndexedString& rhs)
{
	m_entry->decrementRefCount();
	m_entry = rhs.m_entry;
	m_entry->incrementRefCount();
	return *this;
}

bool IndexedString::operator==(const IndexedString& rhs) const { return m_entry == rhs.m_entry; }
bool IndexedString::operator<(const IndexedString& rhs) const { return m_entry < rhs.m_entry; }

const String& IndexedString::get() const
{
	return m_entry->get();
}

}

