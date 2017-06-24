#pragma once

#include "ObjectLifetimeHelpers.hpp"
#include "Defines.hpp"
#include "Allocator.hpp"

namespace Poly
{
	/// Queue is a vector based container thet allocates its memory in one, continous block and provides
	/// API that is very fast when instering/popping from back/front
	/// This should be the goto container for any queueing purposes, like FIFO or LIFO. 
	/// It does not provide API for insertion in any position.
	template<typename T>
	class Queue : public BaseObject<>
	{
	public:
		/// Base queue constructor that creates empty object with capacity == 0.
		Queue() = default;

		/// Creates queue instance with provided capacity.
		/// @param[in] size_t capcity
		Queue(size_t capacity) { Reserve(capacity); }
		
		/// Basic copy constructor
		/// @param[in] const Queue<T>& rhs Reference to Queue instance which state should be copied.
		Queue(const Queue<T>& rhs) { Copy(rhs); }

		/// Basic move constructor
		/// @param[in] Queue<T>&& rhs R-value reference to Queue instance which state should be moved.
		Queue(Queue<T>&& rhs) { Move(std::forward<Queue<T>>(rhs)); }

		/// Creates queue instance from initializer list.
		/// @param[in] const std::initializer_list<T>& list
		Queue(const std::initializer_list<T>& list) { PopulateFromInitializerList(list); }

		/// Basic destructor.
		~Queue() override
		{
			Clear();
			Free();
		}

		/// Basic copy operator
		/// @param[in] const Queue<T>& rhs Reference to Queue instance which state should be copied.
		Queue<T>& operator=(const Queue<T>& rhs)
		{
			Clear();
			Copy(rhs);
			return *this;
		}

		/// Basic move operator
		/// @param[in] Queue<T>&& rhs R-value reference to Queue instance which state should be moved.
		Queue<T>& operator=(Queue<T>&& rhs)
		{
			Clear();
			Free();
			Move(std::forward<Queue<T>>(rhs));
			return *this;
		}

		/// Clears current queue content and populates it with content from initializer list.
		/// @param[in] const std::initializer_list<T>& list
		Queue<T>& operator=(const std::initializer_list<T>& list)
		{
			Clear();
			PopulateFromInitializerList(list);
			return *this;
		}

		/// Equal comparison operator with other queue
		/// @return bool True if size of the queues match and objects represented by both of them
		/// are identical and in the same order, false otherwise.
		bool operator==(const Queue<T>& rhs) const
		{
			if (GetSize() != rhs.GetSize())
				return false;

			for (size_t n = 0; n < GetSize(); ++n)
				if (Data[GetNthIdx(n)] != rhs.Data[rhs.GetNthIdx(n)])
					return false;

			return true;
		}

		/// Not-equal comparison operator with other queue
		/// @return bool True when equal operator returns false, false otherwise.
		bool operator!=(const Queue<T>& rhs) const { return !(*this == rhs); };

		/// Checks whether queue is empty
		/// @return bool True if is empty, false otherwise.
		bool IsEmpty() const { return GetSize() == 0; }
		
		/// Returns current size of the queue
		/// @return size_t Size of the queue in objects count.
		size_t GetSize() const { return Size; }
		
		/// Returns current maximum capacity of the queue.
		/// If the capacity is exceeded the queue will have to expand.
		/// @return size_t Capacity of the queue in objects count.
		size_t GetCapacity() const { return Capacity; }

		/// Clears contents of the queue. This does not release aquired memory.
		void Clear()
		{
			for (size_t i = 0; i < Size; ++i)
				ObjectLifetimeHelper::Destroy(Data + GetNthIdx(i));
			Size = 0;
			Head = 0;
			Tail = 0;
		}

		/// Performs insertion to the back of the queue.
		/// @param[in] const T& obj Const reference to object that should be copied to the queue.
		void PushBack(const T& obj)
		{
			if (GetSize() >= GetCapacity())
				Enlarge();
			ObjectLifetimeHelper::CopyCreate(Data + Tail, obj);
			AdvanceIdx(Tail);
			Size++;
		}

		/// Performs insertion to the front of the queue.
		/// @param[in] const T& obj Const reference to object that should be copied to the queue.
		void PushFront(const T& obj)
		{
			if (GetSize() >= GetCapacity())
				Enlarge();
			DecreaseIdx(Head);
			ObjectLifetimeHelper::CopyCreate(Data + Head, obj);
			Size++;
		}

		/// Performs removal from the back of the queue.
		void PopBack()
		{
			HEAVY_ASSERTE(!IsEmpty(), "Trying to access empty queue!");
			DecreaseIdx(Tail);
			ObjectLifetimeHelper::Destroy(Data + Tail);
			--Size;
		}

		/// Performs removal from the front of the queue.
		void PopFront()
		{
			HEAVY_ASSERTE(!IsEmpty(), "Trying to access empty queue!");
			ObjectLifetimeHelper::Destroy(Data + Head);
			AdvanceIdx(Head);
			--Size;
		}


		/// Returns reference to the first element in queue.
		/// @return T& Reference to the first element in queue.
		T& Front() { HEAVY_ASSERTE(!IsEmpty(), "Trying to access empty queue!"); return Data[Head]; };

		/// Returns reference to the last element in queue.
		/// @return T& Reference to the last element in queue.
		T& Back() { HEAVY_ASSERTE(!IsEmpty(), "Trying to access empty queue!"); return Data[GetPrevIdx(Tail)]; };
		
		/// Returns const reference to the first element in queue.
		/// @return const T& Const reference to the first element in queue.
		const T& Front() const { HEAVY_ASSERTE(!IsEmpty(), "Trying to access empty queue!"); return Data[Head]; };

		/// Returns const reference to the last element in queue.
		/// @return const T& Const reference to the last element in queue.
		const T& Back() const { HEAVY_ASSERTE(!IsEmpty(), "Trying to access empty queue!"); return Data[GetPrevIdx(Tail)]; };

		/// Ensures that enough space is available in the queue.
		/// In case there is not enought the queue gets reallocated to new, bigger memory block.
		/// @param[in] size_t Requested capacity of the queue.
		void Reserve(size_t capacity)
		{
			HEAVY_ASSERTE(capacity >= Capacity, "Capacity too small!");
			Realloc(capacity);
		}

		/// Checks whether provided object is present in the queue at least once.
		/// @param[in] const T& rhs Searched object.
		/// @return bool True if present, false otherwise.
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
			for (size_t i = 0; i < Size; ++i)
				if (Data[GetNthIdx(i)] == rhs)
					return GetNthIdx(i);
			
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
			Reserve(rhs.GetSize());
			for (size_t i = 0; i < rhs.Size; ++i)
			{
				PushBack(rhs.Data[rhs.GetNthIdx(i)]);
			}
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