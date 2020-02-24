#pragma once

#include <pe/Defines.hpp>
#include <pe/core/storage/impl/IndexedStringEntry.hpp>
#include <pe/core/storage/PriorityQueue.hpp>

namespace pe::core::storage::impl
{

struct TTLEntry
{
	size_t m_scheduledTimePoint;
	const IndexedStringEntry* m_entry;

	bool operator<(const TTLEntry& rhs) const { return m_scheduledTimePoint < rhs.m_scheduledTimePoint; }
	bool operator>(const TTLEntry& rhs) const { return m_scheduledTimePoint > rhs.m_scheduledTimePoint; }
};

class CORE_DLLEXPORT IndexedStringManager final : public core::BaseObjectLiteralType<>
{
public:
	static IndexedStringManager& get();

	const IndexedStringEntry* registerString(const char* str);
	void unregisterString(const IndexedStringEntry* entry);

	void setTTLMode(bool enabled);
	void tickTTL(size_t ttlTickCount = 1);

	bool isRegistered(const char* str) const;
private:
	IndexedStringManager() = default;

	IndexedStringManager(const IndexedStringManager&) = delete;
	IndexedStringManager(IndexedStringManager&&) = delete;

	IndexedStringManager& operator=(const IndexedStringManager&) = delete;
	IndexedStringManager& operator=(IndexedStringManager&&) = delete;

	void erase(const IndexedStringEntry* entry);
	void scheduleErase(const IndexedStringEntry* entry);

	std::unordered_map<std::string_view, std::unique_ptr<IndexedStringEntry>> m_entries;
	PriorityQueue<TTLEntry> m_ttlEntries;
	bool m_ttlEnabled = false;
	size_t m_tickCount = 0;
};

}