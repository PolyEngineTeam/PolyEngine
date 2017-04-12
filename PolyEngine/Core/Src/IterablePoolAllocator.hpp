#pragma once

#include "Defines.hpp"
#include "Allocator.hpp"

namespace Poly {
	//------------------------------------------------------------------------------
	// Fast pool allocator, that enables iteration
	template<typename T>
	class IterablePoolAllocator : public BaseObject<>
	{
		struct Cell
		{
			char Data[sizeof(T)];
			Cell* Prev = nullptr;
			Cell* Next = nullptr;
		};

		STATIC_ASSERTE(sizeof(Cell) >= sizeof(size_t), "Type size is too small for allocator");
		STATIC_ASSERTE(OFFSET_OF(Cell, Data) == 0, "Data has to be the first member of Cell struct. It must have no offset.");
	public:
		//------------------------------------------------------------------------------
		class Iterator
		{
		public:
			bool operator==(const Iterator& rhs) const { return CurrentCell == rhs.CurrentCell; }
			bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }

			T& operator*() const { return *((T*)CurrentCell->Data); }
			T& operator->() const { return *((T*)CurrentCell->Data); }

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
		class ConstIterator
		{
		public:
			bool operator==(const ConstIterator& rhs) const { return CurrentCell == rhs.CurrentCell; }
			bool operator!=(const ConstIterator& rhs) const { return !(*this == rhs); }

			const T& operator*() const { return *((T*)CurrentCell->Data); }
			const T& operator->() const { return *((T*)CurrentCell->Data); }

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

		//------------------------------------------------------------------------------
		explicit IterablePoolAllocator(size_t count)
			: Capacity(count), FreeBlockCount(count)
		{
			ASSERTE(count > 0, "Cell count cannot be lower than 1.");
			Data = (Cell*)default_alloc(sizeof(Cell) * (Capacity + 1));
			Next = Data;
			Head = Tail = Data + Capacity;
			Head->Next = nullptr;
			Tail->Prev = nullptr;
		}

		//------------------------------------------------------------------------------
		virtual ~IterablePoolAllocator()
		{
			ASSERTE(Data, "Allocator is invalid");
			default_free(Data);
			Data = nullptr;
		}

		//------------------------------------------------------------------------------
		T* Alloc()
		{
			// initialize new block
			if (InitializedBlockCount < Capacity)
			{
				size_t* p = (size_t*)AddrFromIndex(InitializedBlockCount);
				*p = InitializedBlockCount + 1;
				InitializedBlockCount++;
			}

			// find first suitable place for data
			if (FreeBlockCount > 0)
			{
				Cell* ret = Next;

				// setup linked list
				if (GetSize() > 0) {
					Cell* prev = ret - 1;
					ret->Prev = prev;
					ret->Next = prev->Next;
					prev->Next->Prev = ret;
					prev->Next = ret;
				}
				else
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
					Next = AddrFromIndex(*((size_t*)Next));
				}
				else
				{
					Next = nullptr;
				}

				return (T*)ret->Data;
			}
			return nullptr;
		}

		//------------------------------------------------------------------------------
		void Free(const T* p)
		{
			// get cell addr
			Cell* cell = (Cell*)p;

			// keep the linked list valid
			if (Head == cell)
				Head = Tail;
			else
			{
				Cell* prev = cell->Prev;
				cell->Prev->Next = cell->Next;
				cell->Next->Prev = prev;
			}

			// Calculate the value of Next
			if (Next != nullptr)
			{
				(*(size_t*)p) = IndexFromAddr(Next);
				Next = (Cell*)p;
			}
			else
			{
				*((size_t*)p) = Capacity;
				Next = (Cell*)p;
			}
			++FreeBlockCount;
		}

		//------------------------------------------------------------------------------
		size_t GetSize() const { return Capacity - FreeBlockCount; }

	private:
		Cell* AddrFromIndex(size_t i) const { return Data + i; }
		size_t IndexFromAddr(const Cell* p) const { return p - Data; }

		const size_t Capacity = 0;
		size_t FreeBlockCount = 0;
		size_t InitializedBlockCount = 0;
		Cell* Data = nullptr;
		Cell* Next = nullptr;
		Cell* Head = nullptr;
		Cell* Tail = nullptr;
	};

	// std library for each enablers
	template <typename T> typename Poly::IterablePoolAllocator<T>::Iterator begin(Poly::IterablePoolAllocator<T>& rhs) { return rhs.Begin(); }
	template <typename T> typename Poly::IterablePoolAllocator<T>::Iterator end(Poly::IterablePoolAllocator<T>& rhs) { return rhs.End(); }
	template <typename T> typename Poly::IterablePoolAllocator<T>::ConstIterator begin(const Poly::IterablePoolAllocator<T>& rhs) { return rhs.Begin(); }
	template <typename T> typename Poly::IterablePoolAllocator<T>::ConstIterator end(const Poly::IterablePoolAllocator<T>& rhs) { return rhs.End(); }
}