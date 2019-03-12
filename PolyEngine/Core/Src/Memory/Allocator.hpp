#pragma once

#include <Defines.hpp>

namespace Poly
{
	namespace Impl
	{
		constexpr size_t MEM_ALIGNMENT = 16;
	}

	template<typename T>
	T* Allocate(size_t count)
	{
		using AlignedT = typename std::aligned_storage<sizeof(T), Impl::MEM_ALIGNMENT>::type;
		auto fresh = new(std::nothrow) AlignedT[count];
		return reinterpret_cast<T*>(fresh);
	}

	inline char* AllocateSlab(size_t size) { return Allocate<char>(size); }


	template<typename T> void Deallocate(T* memory)
	{
		using AlignedT = typename std::aligned_storage<sizeof(T), Impl::MEM_ALIGNMENT>::type;
		delete[] reinterpret_cast<AlignedT*>(memory);
	}

	inline void Deallocate(void* memory) { Deallocate(static_cast<char*>(memory)); }
} //namespace Poly
