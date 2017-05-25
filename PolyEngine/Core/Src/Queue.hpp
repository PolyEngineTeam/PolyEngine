#pragma once

#include "ObjectLifetimeHelpers.hpp"
#include "Defines.hpp"
#include "Allocator.hpp"

namespace Poly
{
	template<typename T>
	class Queue : public BaseObject<>
	{
	public:
		//------------------------------------------------------------------------------
		Queue() { }
		Queue(size_t capacity) { Reserve(capacity); }
		//------------------------------------------------------------------------------
		Queue(const Queue<T>& rhs)
		{
			Copy(rhs);
		}

		//------------------------------------------------------------------------------
		Queue(Queue<T>&& rhs)
		{
			Move(std::forward<Queue<T>>(rhs));
		}

		//------------------------------------------------------------------------------
		Queue(const std::initializer_list<T>& list)
		{
			PopulateFromInitializerList(list);
		}

		//------------------------------------------------------------------------------
		~Queue()
		{
			Clear();
			Free();
		}

		//------------------------------------------------------------------------------
		Queue<T>& operator=(const Queue<T>& rhs)
		{
			Clear();
			Copy(rhs);
			return *this;
		}

		//------------------------------------------------------------------------------
		Queue<T>& operator=(Queue<T>&& rhs)
		{
			Clear();
			Free();
			Move(std::forward<Queue<T>>(rhs));
			return *this;
		}

		//------------------------------------------------------------------------------
		Queue<T>& operator=(const std::initializer_list<T>& list)
		{
			Clear();
			PopulateFromInitializerList(list);
			return *this;
		}

		//------------------------------------------------------------------------------
		bool operator==(const Queue<T>& rhs) const
		{
			if (GetSize() != rhs.GetSize())
				return false;

			for (size_t n = 0; n < GetSize(); ++n)
				if (Data[GetNthIdx(n)] != rhs.Data[rhs.GetNthIdx(n)])
					return false;

			return true;
		}

		bool operator!=(const Queue<T>& rhs) const { return !(*this == rhs); };

		//------------------------------------------------------------------------------
		bool Empty() const { return GetSize() == 0; }
		size_t GetSize() const { return Size; }
		size_t GetCapacity() const { return Capacity; }

		//------------------------------------------------------------------------------
		void Clear()
		{
			for (size_t idx = Head; idx != Tail; AdvanceIdx(idx))
				ObjectLifetimeHelper::Destroy(Data + idx);
			Size = 0;
			Head = 0;
			Tail = 0;
		}

		//------------------------------------------------------------------------------
		void PushBack(const T& obj)
		{
			if (Capacity == 0 || Head == GetNextIdx(Tail))
				Enlarge();
			ObjectLifetimeHelper::CopyCreate(Data + Tail, obj);
			AdvanceIdx(Tail);
			Size++;
		}

		void PushFront(const T& obj)
		{
			if (Capacity == 0 || GetPrevIdx(Head) == Tail) // equivalent to (Head == GetNextIdx(Tail)) but it's more readable this way
				Enlarge();
			DecreaseIdx(Head);
			ObjectLifetimeHelper::CopyCreate(Data + Head, obj);
			Size++;
		}

		void PopBack()
		{
			HEAVY_ASSERTE(!Empty(), "Trying to access empty queue!");
			DecreaseIdx(Tail);
			ObjectLifetimeHelper::Destroy(Data + Tail);
			--Size;
		}

		void PopFront()
		{
			HEAVY_ASSERTE(!Empty(), "Trying to access empty queue!");
			ObjectLifetimeHelper::Destroy(Data + Head);
			AdvanceIdx(Head);
			--Size;
		}


		//------------------------------------------------------------------------------
		T& Front() const { HEAVY_ASSERTE(!Empty(), "Trying to access empty queue!"); return Data[Head]; };
		T& Back() const { HEAVY_ASSERTE(!Empty(), "Trying to access empty queue!"); return Data[GetPrevIdx(Tail)]; };

		//------------------------------------------------------------------------------
		void Reserve(size_t capacity)
		{
			HEAVY_ASSERTE(capacity >= Capacity, "Capacity too small!");
			Realloc(capacity);
		}

		//------------------------------------------------------------------------------
		bool Contains(const T& rhs) const { return FindIdx(rhs) < GetSize(); }

	private:
		//------------------------------------------------------------------------------
		size_t GetPrevIdx(size_t idx) const { return (idx == 0) ? Capacity - 1 : idx - 1; }
		size_t GetNextIdx(size_t idx) const { return (idx + 1) % Capacity; }
		void DecreaseIdx(size_t& idx) const { idx = GetPrevIdx(idx); }
		void AdvanceIdx(size_t& idx) const { idx = GetNextIdx(idx); }
		size_t GetNthIdx(size_t& n)	const { return (Head + n) % Capacity; }

		//------------------------------------------------------------------------------
		size_t FindIdx(const T& rhs) const
		{
			for (size_t idx = Head; idx != Tail; AdvanceIdx(idx))
				if (Data[idx] == rhs)
					return idx;

			return GetSize();
		}

		//------------------------------------------------------------------------------
		void Realloc(size_t capacity)
		{
			HEAVY_ASSERTE(Size <= capacity, "Invalid resize capacity!");
			T* newData = static_cast<T*>(DefaultAlloc(capacity * sizeof(T)));

			// move all elements
			for (size_t i = 0; i < Size; ++i)
			{
				size_t oldIdx = GetNthIdx(i);
				ObjectLifetimeHelper::MoveCreate(newData + i, std::move(Data[oldIdx]));
				ObjectLifetimeHelper::Destroy(Data + oldIdx);
			}

			DefaultFree(Data);
			Data = newData;
			Capacity = capacity;
			Head = 0;
			Tail = Size;
		}

		void Free() { if (Data) DefaultFree(Data); }

		//------------------------------------------------------------------------------
		void Copy(const Queue<T>& rhs)
		{
			Reserve(rhs.GetSize() + 1);
			for (size_t idx = rhs.Head; idx != rhs.Tail; rhs.AdvanceIdx(idx))
				PushBack(rhs.Data[idx]);
		}

		//------------------------------------------------------------------------------
		void Move(Queue<T>&& rhs)
		{
			Size = rhs.Size;
			Capacity = rhs.Capacity;
			Data = rhs.Data;
			Head = rhs.Head;
			Tail = rhs.Tail;
			rhs.Data = nullptr;
			rhs.Size = 0;
			rhs.Capacity = 0;
			rhs.Head = 0;
			rhs.Tail = 0;
		}

		//------------------------------------------------------------------------------
		void PopulateFromInitializerList(const std::initializer_list<T>& list)
		{
			Reserve(list.size());
			for (const T& obj : list)
				PushBack(obj);
		}

		void Enlarge() { Reserve((Capacity + 1) * 2); }

		size_t Head = 0;
		size_t Tail = 0;
		size_t Size = 0;
		size_t Capacity = 0;
		T* Data = nullptr;
	};
}