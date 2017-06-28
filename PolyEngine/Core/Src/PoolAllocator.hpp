#pragma once

#include "Defines.hpp"
#include "Allocator.hpp"


namespace Poly {
	/// <summary>Fast pool allocator, based on: https://www.thinkmind.org/download.php?articleid=computation_tools_2012_1_10_80006 </summary>
	template<typename T>
	class PoolAllocator : public BaseObject<>
	{
		STATIC_ASSERTE(sizeof(T) >= sizeof(size_t), "Pool allocator is invalid for types that are smaller than size_t");
	public:
		/// <summary>Constuctor that allocates memory for provided amount of objects. </summary>
		/// <param name="count"></param>
		explicit PoolAllocator(size_t count)
			: Capacity(count), FreeBlockCount(count)
		{
			ASSERTE(count > 0, "Cell count cannot be lower than 1.");
			Data = reinterpret_cast<T*>(DefaultAlloc(sizeof(T) * Capacity));
			Next = Data;
		}

		//------------------------------------------------------------------------------
		virtual ~PoolAllocator()
		{
			ASSERTE(Data, "Allocator is invalid");
			DefaultFree(Data);
			Data = nullptr;
		}

		/// <summary>Allocation method</summary>
		/// <returns>Pointer to uninitialized memory for object of type T.</returns>
		T* Alloc()
		{
			if (InitializedBlockCount < Capacity)
			{
				size_t* p = reinterpret_cast<size_t*>(AddrFromIndex(InitializedBlockCount));
				*p = ++InitializedBlockCount;
			}

			if (FreeBlockCount > 0)
			{
				T* ret = Next;
				--FreeBlockCount;
				if (FreeBlockCount != 0)
				{
					Next = AddrFromIndex(*reinterpret_cast<size_t*>(Next));
				}
				else
				{
					Next = nullptr;
				}
				return ret;
			}
			return nullptr;
		}

		/// <summary>Method for freeing allocated memory. Allocator does not call any object destructors!</summary>
		/// <param name="p">Pointer to memory to free.</param>
		void Free(T* p)
		{
			if (Next != nullptr)
			{
				*reinterpret_cast<size_t*>(p) = IndexFromAddr(Next);
				Next = p;
			}
			else
			{
				*reinterpret_cast<size_t*>(p) = Capacity;
				Next = p;
			}
			++FreeBlockCount;
		}

		/// <summary>Gets current size of the allocator.</summary>
		/// <returns>Count of allocated objects.</returns>
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