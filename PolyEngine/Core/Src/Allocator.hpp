#pragma once

#include "Defines.hpp"


namespace Poly {

	inline void* default_alloc(size_t size) { return malloc(size); }
	inline void* default_realloc(void* ptr, size_t size) { return realloc(ptr, size); }
	inline void default_free(void* ptr) { free(ptr); }



	// Very inefficient pool allocator
	template<typename T>
	class PoolAllocator
	{
	public:
		PoolAllocator(size_t count)
			: Capacity(count)
		{
			Memory = static_cast<T*>(default_alloc(sizeof(T) * count));
			MemoryMap = static_cast<bool*>(default_alloc(sizeof(bool) * count));
			memset(MemoryMap, false, sizeof(bool) * count);
		}

		virtual ~PoolAllocator()
		{
			if (Memory) default_free(Memory);
			if (MemoryMap) default_free(MemoryMap);
		}

		T* Alloc()
		{
			for (size_t i = 0; i < Capacity; ++i)
			{
				if (!MemoryMap[i])
				{
					MemoryMap[i] = true;
					++Size;
					return Memory + i;
				}
			}
			return nullptr;
		}

		void Free(const T* obj)
		{
			size_t offset = obj - Memory;
			HEAVY_ASSERTE(offset >= 0 && offset < Capacity, "Address out of bounds");
			MemoryMap[offset] = false;
			--Size;
#if defined(_DEBUG)
			memset(Memory + offset, 0, sizeof(T)); // zero memory
#endif
		}

		size_t GetSize() const { return Size; }
	private:
		size_t Size = 0;
		const size_t Capacity = 0;
		T* Memory = nullptr;
		bool* MemoryMap = nullptr;
	};
}