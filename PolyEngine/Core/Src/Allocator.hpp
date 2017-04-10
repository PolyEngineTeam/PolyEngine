#pragma once

#include "Defines.hpp"


namespace Poly {

	inline void* default_alloc(size_t size) { return malloc(size); }
	inline void* default_realloc(void* ptr, size_t size) { return realloc(ptr, size); }
	inline void default_free(void* ptr) { free(ptr); }



	// Fast pool allocator, https://www.thinkmind.org/download.php?articleid=computation_tools_2012_1_10_80006
	template<typename T>
	class PoolAllocator
	{
	public:
		PoolAllocator(size_t count)
			: Capacity(count), FreeBlockCount(count)
		{
			Data = (T*)default_alloc(sizeof(T) * Capacity);
			Next = Data;
		}

		virtual ~PoolAllocator()
		{
			if (Data)
			{
				default_free(Data);
				Data = nullptr;
			}
		}

		T* Alloc()
		{
			if (InitializedBlockCount < Capacity)
			{
				size_t* p = (size_t*)AddrFromIndex(InitializedBlockCount);
				*p = InitializedBlockCount + 1;
				InitializedBlockCount++;
			}
			T* ret = nullptr;
			if (FreeBlockCount > 0)
			{
				ret = Next;
				--FreeBlockCount;
				if (FreeBlockCount != 0)
				{
					Next = AddrFromIndex(*((size_t*)Next));
				}
				else
				{
					Next = nullptr;
				}
			}
			return ret;
		}

		void Free(const T* p)
		{
			if (Next != nullptr)
			{
				(*(size_t*)p) = IndexFromAddr(Next);
				Next = (T*)p;
			}
			else
			{
				*((size_t*)p) = Capacity;
				Next = (T*)p;
			}
			++FreeBlockCount;
		}

		size_t GetSize() const { return Capacity - FreeBlockCount; }
	private:
		T* AddrFromIndex(size_t i) const { return Data + i; }
		size_t IndexFromAddr(const T* p) const { return p - Data; }

		const size_t Capacity = 0;
		size_t FreeBlockCount = 0;
		size_t InitializedBlockCount = 0;
		T* Data = nullptr;		T* Next = nullptr;
	};
}