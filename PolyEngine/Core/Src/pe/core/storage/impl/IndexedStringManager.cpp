#include <pe/core/storage/impl/IndexedStringManager.hpp>

namespace pe::core::storage::impl
{

IndexedStringManager& IndexedStringManager::get()
{
	static IndexedStringManager instance;
	return instance;
}

const IndexedStringEntry* IndexedStringManager::registerString(const char* str)
{
	CString cstr(str);
	IndexedStringEntry* ret = nullptr;

	auto it = m_entries.find(cstr);
	if (it == m_entries.end())
	{
		auto entry = std::make_unique<IndexedStringEntry>(str);
		ret = entry.get();
		m_entries.emplace(cstr, std::move(entry));
	}
	else
	{
		ret = it->second.get();
	}
	
	ret->incrementRefCount();
	return ret;
}

void IndexedStringManager::unregisterString(const IndexedStringEntry* entry)
{
	entry->decrementRefCount();

	if (entry->getRefCount() == 0)
	{
		if (m_ttlEnabled)
			scheduleErase(entry);
		else
			erase(entry);
	}
}

void IndexedStringManager::scheduleErase(const IndexedStringEntry* entry)
{
	ASSERTE(false, "TTL not yet implemented!");
	// @todo(muniu) implement
}

void IndexedStringManager::erase(const IndexedStringEntry* entry)
{
	CString cstr(entry->get().GetCStr());

	auto it = m_entries.find(cstr);
	ASSERTE(it != m_entries.end(), "Couldn't find indexed string entry for given string!");
	m_entries.erase(it);
}

void IndexedStringManager::setTTLMode(bool enabled)
{
	m_ttlEnabled = enabled;
}

void IndexedStringManager::tickTTL(size_t ttlTickCount)
{
	if (!m_ttlEnabled)
		return;

	ASSERTE(false, "TTL not yet implemented!");
	// @todo(muniu) implement
}

}