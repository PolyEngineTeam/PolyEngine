#pragma once

#include "Defines.hpp"
#include "Memory/ObjectLifetimeHelpers.hpp"

namespace Poly {

	template<typename T, size_t N>
	class UnsafeStorage final : public BaseObjectLiteralType<>
	{
		using value_type      = T;
		using pointer         = T*;
		using const_pointer   = const T*;
		using reference       = value_type&;
		using const_reference = const value_type&;
		using iterator        = value_type*;
		using const_iterator  = const value_type*;
		using size_type       = size_t;
		using difference_type = intptr_t;
	public:
		iterator begin() noexcept { return iterator(data()); }
		const_iterator begin() const noexcept { return const_iterator(data()); }
		const_iterator cbegin() const noexcept { return const_iterator(data()); }
		constexpr size_type max_size() const noexcept { return N; }
		reference operator[](size_type idx) { ASSERTE(idx < N, "Index out of bounds"); return static_cast<T&>(data()[idx]); }
		constexpr const_reference operator[](size_type idx) const { ASSERTE(idx < N, "Index out of bounds"); return static_cast<const T&>(data()[idx]); }
		pointer data() noexcept { return reinterpret_cast<T*>(internal_array); }
		const_pointer data() const noexcept { return reinterpret_cast<const T*>(internal_array); }
		void Destruct(const size_t len) { ASSERTE(len <= N, "Attempting to destruct more values than the storage can possibly hold"); this->DestructRange(0, len); }
		void DestructRange(const size_t from, const size_t until)
		{
			ASSERTE(from <= until, "Lower bound is greater than the higher bound");
			ASSERTE(from <= N, "Lower bound is greater than maximum. Possible negative integer overflow.");
			ASSERTE(until <= N, "Higher bound is greater than maximum");
			if (!std::is_trivially_destructible<T>::value)
			{
				for (iterator it = begin() + from; it != begin() + until; ++it)
				{
					ObjectLifetimeHelper::Destroy(&*it);
				}
			}
		}
		void DestructAt(const size_t idx) { ASSERTE(idx < N, "Index out of bounds"); this->DestructRange(idx, idx + 1u); }
	private:
		alignas(alignof(T)) char internal_array[sizeof(T) * N];
	};
}
