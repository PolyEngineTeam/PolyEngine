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
	IndexedStringEntry* ret = nullptr;

	auto it = m_entries.find(str);
	if (it == m_entries.end())
	{
		// @todo(muniu) There are two allocations happening here.
		// Consider fixing it if the performace is affected by this.
		auto entry = std::make_unique<IndexedStringEntry>(str);
		ret = entry.get();
		// We need to create a new cstring, which points to the string with the same lifetime as entry.
		// Otherwise we could have some memory issues.
		std::string_view strView(entry.get()->get().GetCStr());
		m_entries.emplace(strView, std::move(entry));
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
	auto it = m_entries.find(entry->get().GetCStr());
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