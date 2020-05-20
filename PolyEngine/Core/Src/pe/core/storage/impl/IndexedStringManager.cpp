#include <pe/core/storage/impl/IndexedStringManager.hpp>

namespace pe::core::storage::impl
{

constexpr size_t TIME_TO_LIVE = 100;

IndexedStringManager& IndexedStringManager::get()
{
	static IndexedStringManager instance;
	return instance;
}

const IndexedStringEntry* IndexedStringManager::registerString(std::string_view str)
{
	auto it = m_entries.find(str);
	if (it == m_entries.end())
	{
		return shareEntry(createEntry(String(str)));
	}
	else
	{
		return shareEntry(it->second.get());
	}
}

const IndexedStringEntry* IndexedStringManager::registerString(core::storage::String&& str)
{
	auto it = m_entries.find(str);
	if (it == m_entries.end())
	{
		return shareEntry(createEntry(std::move(str)));
	}
	else
	{
		return shareEntry(it->second.get());
	}
}

const IndexedStringEntry* IndexedStringManager::createEntry(core::storage::String&& str)
{
	IndexedStringEntry* ret = nullptr;
	auto entry = std::make_unique<IndexedStringEntry>(std::move(str));
	ret = entry.get();
	// We need to create a new string_view, which points to the string with the same lifetime as entry.
	// Otherwise we could have some memory issues.
	std::string_view strView(entry.get()->get().GetCStr());
	m_entries.emplace(strView, std::move(entry));

	return ret;
}

const IndexedStringEntry* IndexedStringManager::shareEntry(const IndexedStringEntry* entry)
{
	entry->incrementRefCount();
	entry->resetRemovalTimePoint();
	return entry;
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
	size_t removalTimePoint = m_tickCount + TIME_TO_LIVE;
	HEAVY_ASSERTE(entry->getRemovalTimePoint().value_or(0) <= removalTimePoint,
		"Cannot set removal time point to a previous one than already set!");
	entry->setRemovalTimePoint(removalTimePoint);
	m_ttlEntries.emplace(TTLEntry{removalTimePoint, entry});
}

void IndexedStringManager::erase(const IndexedStringEntry* entry)
{
	auto it = m_entries.find(entry->get().GetCStr());
	ASSERTE(it != m_entries.end(), "Couldn't find indexed string entry for given string!");
	m_entries.erase(it);
}

void IndexedStringManager::setTTLMode(bool enabled)
{
	if(m_ttlEnabled && !enabled)
	{
		m_ttlEntries.clear();
	}
	m_ttlEnabled = enabled;
}

bool IndexedStringManager::isRegistered(std::string_view str) const
{
	auto it = m_entries.find(str);
	return it != m_entries.end();
}

void IndexedStringManager::tickTTL(size_t ttlTickCount)
{
	if (!m_ttlEnabled)
		return;

	m_tickCount += ttlTickCount;

	while(m_ttlEntries.size() > 0 && m_ttlEntries.top().m_scheduledTimePoint <= m_tickCount)
	{
		auto entry = m_ttlEntries.top().m_entry;
		m_ttlEntries.pop();
		auto realRemovalTimePoint = entry->getRemovalTimePoint();
		if (realRemovalTimePoint.value_or(m_tickCount+1) <= m_tickCount)
		{
			erase(entry);
		}
	}
}

}