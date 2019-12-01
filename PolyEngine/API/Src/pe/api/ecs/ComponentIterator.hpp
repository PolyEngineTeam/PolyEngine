#pragma once

#include <pe/Defines.hpp>

#include <pe/core/memory/IterablePoolAllocator.hpp>
#include <pe/api/ecs/Entity.hpp>

namespace pe::api::ecs {

/// Component iterator.
template<typename PrimaryComponent, typename... SecondaryComponents>
class ComponentIterator final : public ::pe::core::BaseObjectLiteralType<>
{
	using PrimaryCmpIt_t = typename ::pe::core::memory::IterablePoolAllocator<PrimaryComponent>::Iterator;
	using ComponentsTuple_t = std::tuple<typename std::add_pointer<PrimaryComponent>::type,
										 typename std::add_pointer<SecondaryComponents>::type...>;
	
	ITERATOR(std::forward_iterator_tag, ComponentsTuple_t)
public:
	explicit ComponentIterator(PrimaryCmpIt_t it, PrimaryCmpIt_t begin, PrimaryCmpIt_t end) 
		: m_iter(it)
		, m_begin(begin)
		, m_end(end)
	{
		if (shouldIncrement())
			increment();
	}
	
	bool operator==(const ComponentIterator& rhs) const { return m_iter == rhs.m_iter; }
	bool operator!=(const ComponentIterator& rhs) const { return !(*this == rhs); }

	ComponentsTuple_t operator->() const { return **this; }
	ComponentsTuple_t operator*() const
	{
		PrimaryComponent* primary = &*m_iter;
		return std::make_tuple(primary, primary->template GetSibling<SecondaryComponents>()...);
	}

	ComponentIterator& operator++() { increment(); return *this; }
	ComponentIterator operator++(int) { ComponentIterator ret(m_iter); increment(); return ret; }

private:
	template<int zero = 0>
	bool hasComponents(const Entity* entity) const { return true; }

	template<typename Head, typename... Tail>
	bool hasComponents(const Entity* entity) const
	{
		return entity->template HasComponent<Head>() && hasComponents<Tail...>(entity);
	}

	void increment()
	{
		do { ++m_iter; } 
		while (shouldIncrement());
	}

	bool shouldIncrement() const
	{
		return m_iter != m_end && !hasComponents<SecondaryComponents...>(m_iter->GetOwner());
	}

	PrimaryCmpIt_t m_iter;
	PrimaryCmpIt_t m_begin;
	PrimaryCmpIt_t m_end;
};

}
