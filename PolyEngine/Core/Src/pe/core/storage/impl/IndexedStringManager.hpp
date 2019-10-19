#pragma once

#include <pe/Defines.hpp>
#include <pe/core/storage/impl/IndexedStringEntry.hpp>
#include <pe/core/storage/CString.hpp>

namespace pe::core::storage::impl
{

class CORE_DLLEXPORT IndexedStringManager final : public core::BaseObjectLiteralType<>
{
public:
	static IndexedStringManager& get();

	const IndexedStringEntry* registerString(const char* str);
	void unregisterString(const IndexedStringEntry* entry);

	void setTTLMode(bool enabled);
	void tickTTL(size_t ttlTickCount = 1);
private:
	IndexedStringManager() = default;

	IndexedStringManager(const IndexedStringManager&) = delete;
	IndexedStringManager(IndexedStringManager&&) = delete;

	IndexedStringManager& operator=(const IndexedStringManager&) = delete;
	IndexedStringManager& operator=(IndexedStringManager&&) = delete;

	void erase(const IndexedStringEntry* entry);
	void scheduleErase(const IndexedStringEntry* entry);

	std::unordered_map<CString, std::unique_ptr<IndexedStringEntry>> m_entries;
	bool m_ttlEnabled = false;
};

}