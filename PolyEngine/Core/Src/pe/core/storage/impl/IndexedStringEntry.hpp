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

	void incrementRefCount() const { ++m_refCount; }
	void decrementRefCount() const { --m_refCount; }

	const String& get() const { return m_str; }
	size_t getRefCount() const { return m_refCount; }
private:
	const String m_str;
	mutable size_t m_refCount = 0;
	std::optional<size_t> m_ttl;
};

}