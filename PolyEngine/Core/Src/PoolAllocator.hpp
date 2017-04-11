#pragma once

#include "Defines.hpp"
#include "Allocator.hpp"


namespace Poly {
	//------------------------------------------------------------------------------
	// Fast pool allocator, https://www.thinkmind.org/download.php?articleid=computation_tools_2012_1_10_80006
	template<typename T>
	class PoolAllocator : public BaseObject<>
	{
		STATIC_ASSERTE(sizeof(T) >= sizeof(size_t), "Pool allocator is invalid for types that are smaller than size_t");
	public:
		//------------------------------------------------------------------------------
		explicit PoolAllocator(size_t count)
			: Capacity(count), FreeBlockCount(count)
		{
			ASSERTE(count > 0, "Cell count cannot be lower than 1.");
			Data = (T*)default_alloc(sizeof(T) * Capacity);
			Next = Data;
		}

		//------------------------------------------------------------------------------
		virtual ~PoolAllocator()
		{
			ASSERTE(Data, "Allocator is invalid");
			default_free(Data);
			Data = nullptr;
		}

		//------------------------------------------------------------------------------
		T* Alloc()
		{
			if (InitializedBlockCount < Capacity)
			{
				size_t* p = (size_t*)AddrFromIndex(InitializedBlockCount);
				*p = InitializedBlockCount + 1;
				InitializedBlockCount++;
			}

			if (FreeBlockCount > 0)
			{
				T* ret = Next;
				--FreeBlockCount;
				if (FreeBlockCount != 0)
				{
					Next = AddrFromIndex(*((size_t*)Next));
				}
				else
				{
					Next = nullptr;
				}
				return ret;
			}
			return nullptr;
		}

		//------------------------------------------------------------------------------
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

		//------------------------------------------------------------------------------
		size_t GetSize() const { return Capacity - FreeBlockCount; }
	private:
		T* AddrFromIndex(size_t i) const { return Data + i; }
		size_t IndexFromAddr(const T* p) const { return p - Data; }

		const size_t Capacity = 0;
		size_t FreeBlockCount = 0;
		size_t InitializedBlockCount = 0;
		T* Data = nullptr;
		T* Next = nullptr;
	};
}