#pragma once

#include "Defines.hpp"
#include "Allocator.hpp"

namespace Poly {

	class IterablePoolAllocatorBase : public BaseObject<>
	{
	public:
		virtual void Free(void* ptr) = 0;
	};

	/// <summary>Fast pool allocator, that enables iteration.</summary>
	template<typename T>
	class IterablePoolAllocator : public IterablePoolAllocatorBase
	{
		struct Cell
		{
			char Data[sizeof(T)];
			Cell* Prev = nullptr;
			Cell* Next = nullptr;
		};

		STATIC_ASSERTE(sizeof(T) >= sizeof(size_t), "Type size is too small for allocator");
		STATIC_ASSERTE(offsetof(Cell, Data) == 0, "Data has to be the first member of Cell struct. It must have no offset.");
	public:
		//------------------------------------------------------------------------------
		class Iterator : public BaseObject<>, public std::iterator<std::bidirectional_iterator_tag, T>
		{
		public:
			bool operator==(const Iterator& rhs) const { return CurrentCell == rhs.CurrentCell; }
			bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }

			T& operator*() const { return *reinterpret_cast<T*>(CurrentCell->Data); }
			T& operator->() const { return *reinterpret_cast<T*>(CurrentCell->Data); }

			Iterator& operator++() { CurrentCell = CurrentCell->Next; return *this; }
			Iterator operator++(int) { Iterator ret(CurrentCell); CurrentCell = CurrentCell->Next; return ret; }
			Iterator& operator--() { CurrentCell = CurrentCell->Prev; return *this; }
			Iterator operator--(int) { Iterator ret(CurrentCell); CurrentCell = CurrentCell->Prev; return ret; }

		private:
			explicit Iterator(Cell* cell) : CurrentCell(cell) {}

			Cell* CurrentCell = nullptr;
			friend class IterablePoolAllocator;
		};

		//------------------------------------------------------------------------------
		class ConstIterator : public BaseObject<>, public std::iterator<std::bidirectional_iterator_tag, T>
		{
		public:
			bool operator==(const ConstIterator& rhs) const { return CurrentCell == rhs.CurrentCell; }
			bool operator!=(const ConstIterator& rhs) const { return !(*this == rhs); }

			const T& operator*() const { return *reinterpret_cast<T*>(CurrentCell->Data); }
			const T& operator->() const { return *reinterpret_cast<T*>(CurrentCell->Data); }

			ConstIterator& operator++() { CurrentCell = CurrentCell->Next; return *this; }
			ConstIterator operator++(int) { ConstIterator ret(CurrentCell); CurrentCell = CurrentCell->Next; return ret; }
			ConstIterator& operator--() { CurrentCell = CurrentCell->Prev; return *this; }
			ConstIterator operator--(int) { ConstIterator ret(CurrentCell); CurrentCell = CurrentCell->Prev; return ret; }

		private:
			explicit ConstIterator(Cell* cell) : CurrentCell(cell) {}

			Cell* CurrentCell = nullptr;
			friend class IterablePoolAllocator;
		};

		//------------------------------------------------------------------------------
		Iterator Begin() { return Iterator(Head); }
		Iterator End() { return Iterator(Tail); }
		ConstIterator Begin() const { return ConstIterator(Head); }
		ConstIterator End() const { return ConstIterator(Tail); }

		/// <summary>Constuctor that allocates memory for provided amount of objects. </summary>
		/// <param name="count"></param>
		explicit IterablePoolAllocator(size_t count)
			: Capacity(count), FreeBlockCount(count)
		{
			ASSERTE(count > 0, "Cell count cannot be lower than 1.");
			Data = reinterpret_cast<Cell*>(DefaultAlloc(sizeof(Cell) * (Capacity + 1)));
			NextFree = Data;
			Head = Tail = Data + Capacity; // Head and tail point to the last element from pool which is reserved for linked list.
			Head->Next = nullptr; // Reset next and prev pointers
			Tail->Prev = nullptr;
			NextFree->Next = nullptr;
			NextFree->Prev = nullptr;
		}

		//------------------------------------------------------------------------------
		virtual ~IterablePoolAllocator()
		{
			ASSERTE(Data, "Allocator is invalid");
			DefaultFree(Data);
			Data = nullptr;
		}

		/// <summary>Allocation method</summary>
		/// <returns>Pointer to uninitialized memory for object of type T.</returns>
		T* Alloc()
		{
			// initialize new block
			if (InitializedBlockCount < Capacity)
			{
				size_t* p = reinterpret_cast<size_t*>(AddrFromIndex(InitializedBlockCount));
				*p = ++InitializedBlockCount;
			}

			// find first suitable place for data
			if (FreeBlockCount > 0)
			{
				Cell* ret = NextFree;

				// setup linked list
				if (Tail->Prev != nullptr) { // list is not empty
					if (ret > Head) {

						Cell* prev = Head;
						while (prev->Next && prev->Next < ret)
							prev = prev->Next;

						//Cell* prev = ret - 1;
						HEAVY_ASSERTE(prev->Next != nullptr, "prev is null");
						
						ret->Prev = prev;
						ret->Next = prev->Next;
						prev->Next->Prev = ret;
						prev->Next = ret;
					}
					else // Corner case when we're placing new element at first position
					{
						ret->Prev = nullptr;
						ret->Next = Head;
						Head->Prev = ret;
						Head = ret;
					}
				}
				else // list is empty
				{
					ret->Prev = nullptr;
					ret->Next = Tail;
					Tail->Prev = ret;
					Head = ret;
				}

				// Update the Next node value.
				--FreeBlockCount;
				if (FreeBlockCount != 0)
				{
					NextFree = AddrFromIndex(*reinterpret_cast<size_t*>(NextFree));
					NextFree->Next = nullptr;
					NextFree->Prev = nullptr;
				}
				else
				{
					NextFree = nullptr;
				}

				return reinterpret_cast<T*>(ret->Data);
			}
			return nullptr;
		}

		//------------------------------------------------------------------------------
		void Free(void* p) override { Free(reinterpret_cast<T*>(p)); }

		/// <summary>Method for freeing allocated memory. Allocator does not call any object destructors!</summary>
		/// <param name="p">Pointer to memory to free.</param>
		void Free(T* p)
		{
			// get cell addr
			Cell* cell = reinterpret_cast<Cell*>(p);

			// keep the linked list valid
			if (Head == cell)
			{
				Head = cell->Next;
				Head->Prev = nullptr;
			}
			else
			{
				Cell* prev = cell->Prev;
				prev->Next = cell->Next;
				cell->Next->Prev = prev;
			}

			cell->Next = nullptr;
			cell->Prev = nullptr;

			// Calculate the value of Next
			if (NextFree != nullptr)
			{
				*reinterpret_cast<size_t*>(p) = IndexFromAddr(NextFree);
				NextFree = cell;
				NextFree->Next = nullptr;
				NextFree->Prev = nullptr;
			}
			else
			{
				*reinterpret_cast<size_t*>(p) = Capacity;
				NextFree = cell;
				NextFree->Next = nullptr;
				NextFree->Prev = nullptr;
			}
			++FreeBlockCount;
		}

		/// <summary>Gets current size of the allocator.</summary>
		/// <returns>Count of allocated objects.</returns>
		size_t GetSize() const { return Capacity - FreeBlockCount; }

	private:
		Cell* AddrFromIndex(size_t i) const { return Data + i; }
		size_t IndexFromAddr(const Cell* p) const { return p - Data; }

		const size_t Capacity = 0;
		size_t FreeBlockCount = 0;
		size_t InitializedBlockCount = 0;
		Cell* Data = nullptr;
		Cell* NextFree = nullptr;
		Cell* Head = nullptr;
		Cell* Tail = nullptr;
	};

	// std library for each enablers
	template <typename T> typename Poly::IterablePoolAllocator<T>::Iterator begin(Poly::IterablePoolAllocator<T>& rhs) { return rhs.Begin(); }
	template <typename T> typename Poly::IterablePoolAllocator<T>::Iterator end(Poly::IterablePoolAllocator<T>& rhs) { return rhs.End(); }
	template <typename T> typename Poly::IterablePoolAllocator<T>::ConstIterator begin(const Poly::IterablePoolAllocator<T>& rhs) { return rhs.Begin(); }
	template <typename T> typename Poly::IterablePoolAllocator<T>::ConstIterator end(const Poly::IterablePoolAllocator<T>& rhs) { return rhs.End(); }
}
