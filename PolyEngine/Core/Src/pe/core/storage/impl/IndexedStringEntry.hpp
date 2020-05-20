#pragma once

#include <pe/Defines.hpp>
#include <pe/core/storage/String.hpp>

#include <pe/core/utils/Logger.hpp>

namespace pe::core::storage::impl
{

class IndexedStringEntry final : public core::BaseObjectLiteralType<>
{
public:
	IndexedStringEntry(const char* str) : m_str(str) {}
	IndexedStringEntry(core::storage::String&& str) : m_str(std::move(str)) {}

	void incrementRefCount() const { ++m_refCount; }
	void decrementRefCount() const { --m_refCount; }

	const String& get() const { return m_str; }
	size_t getRefCount() const { return m_refCount; }

	std::optional<size_t> getRemovalTimePoint() const { return m_removalTimepoint; }
	void setRemovalTimePoint(size_t timePoint) const { m_removalTimepoint = timePoint; }
	void resetRemovalTimePoint() const { m_removalTimepoint = {}; }
private:
	const String m_str;
	mutable size_t m_refCount = 0;
	mutable std::optional<size_t> m_removalTimepoint;
};

}