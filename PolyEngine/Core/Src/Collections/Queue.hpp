#pragma once

#include <Defines.hpp>
#include <Memory/ObjectLifetimeHelpers.hpp>

#include <Memory/Allocator.hpp>

namespace Poly
{
	/// <summary>
	/// Queue is a vector based container thet allocates its memory in one, continous block and provides
	/// API that is very fast when instering/popping from back/front.
	/// <para>This should be the goto container for any queueing purposes, like FIFO or LIFO.
	/// It does not provide API for insertion in any position.</para>
	/// </summary>
	template<typename T>
	class Queue final : public BaseObjectLiteralType<>
	{
	public:
		/// <summary>Base queue constructor that creates empty object with capacity == 0.</summary>
		Queue() = default;

		/// <summary>Creates queue instance with provided capacity.</summary>
		/// <param name="capacity"></param>
		explicit Queue(size_t capacity) { Reserve(capacity); }

		/// <summary>Basic copy constructor</summary>
		/// <param name="rhs">Reference to Queue instance which state should be copied.</param>
		Queue(const Queue<T>& rhs) { Copy(rhs); }

		/// <summary>Basic move constructor</summary>
		/// <param name="rhs">R-value reference to Queue instance which state should be moved.</param>
		Queue(Queue<T>&& rhs) { Move(std::forward<Queue<T>>(rhs)); }

		/// <summary>Creates queue instance from initializer list.</summary>
		/// <param name="list"></param>
		Queue(const std::initializer_list<T>& list) { PopulateFromInitializerList(list); }

		/// <summary>Basic destructor.</summary>
		~Queue()
		{
			Clear();
			Free();
		}

		/// <summary>Basic copy operator</summary>
		/// <param name="rhs">Reference to Queue instance which state should be copied.</param>
		Queue<T>& operator=(const Queue<T>& rhs)
		{
			Clear();
			Copy(rhs);
			return *this;
		}

		/// <summary>Basic move operator</summary>
		/// <param name="rhs">R-value reference to Queue instance which state should be moved.</param>
		Queue<T>& operator=(Queue<T>&& rhs)
		{
			Clear();
			Free();
			Move(std::forward<Queue<T>>(rhs));
			return *this;
		}

		/// <summary>Clears current queue content and populates it with content from initializer list.</summary>
		/// <param name="list"></param>
		Queue<T>& operator=(const std::initializer_list<T>& list)
		{
			Clear();
			PopulateFromInitializerList(list);
			return *this;
		}

		/// <summary>Equal comparison operator with other queue</summary>
		/// <returns>
		/// True if size of the queues match and objects represented by both of them
		/// are identical and in the same order, false otherwise.
		/// </returns>
		bool operator==(const Queue<T>& rhs) const
		{
			if (GetSize() != rhs.GetSize())
				return false;

			for (size_t n = 0; n < GetSize(); ++n)
				if (Data[GetNthIdx(n)] != rhs.Data[rhs.GetNthIdx(n)])
					return false;

			return true;
		}

		/// <summary>Not-equal comparison operator with other queue</summary>
		/// @<returns>True when equal operator returns false, false otherwise.</returns>
		bool operator!=(const Queue<T>& rhs) const { return !(*this == rhs); };

		/// <summary>Checks whether queue is empty</summary>
		/// <returns>True if is empty, false otherwise.</returns>
		bool IsEmpty() const { return GetSize() == 0; }

		/// <summary>Returns current size of the queue</summary>
		/// <returns>Size of the queue in objects count.</returns>
		size_t GetSize() const { return Size; }

		/// <summary>
		/// Returns current maximum capacity of the queue.
		/// If the capacity is exceeded the queue will have to expand.
		/// </summary>
		/// <returns>Capacity of the queue in objects count.</returns>
		size_t GetCapacity() const { return Capacity; }

		/// <summary>Clears contents of the queue. This does not release aquired memory.</summary>
		void Clear()
		{
			for (size_t i = 0; i < Size; ++i)
				ObjectLifetimeHelper::Destroy(Data + GetNthIdx(i));
			Size = 0;
			Head = 0;
			Tail = 0;
		}

		/// <summary>Performs insertion to the back of the queue.</summary>
		/// <param name="obj">Const reference to object that should be copied to the queue.</param>
		void PushBack(const T& obj)
		{
			if (GetSize() >= GetCapacity())
				Enlarge();
			ObjectLifetimeHelper::CopyCreate(Data + Tail, obj);
			AdvanceIdx(Tail);
			Size++;
		}

		/// <summary>Performs insertion to the front of the queue.</summary>
		/// <param name="obj">Const reference to object that should be copied to the queue.</param>
		void PushFront(const T& obj)
		{
			if (GetSize() >= GetCapacity())
				Enlarge();
			DecreaseIdx(Head);
			ObjectLifetimeHelper::CopyCreate(Data + Head, obj);
			Size++;
		}

		/// <summary>Performs removal from the back of the queue.</summary>
		void PopBack()
		{
			HEAVY_ASSERTE(!IsEmpty(), "Trying to access empty queue!");
			DecreaseIdx(Tail);
			ObjectLifetimeHelper::Destroy(Data + Tail);
			--Size;
		}

		/// <summary>Performs removal from the front of the queue.</summary>
		void PopFront()
		{
			HEAVY_ASSERTE(!IsEmpty(), "Trying to access empty queue!");
			ObjectLifetimeHelper::Destroy(Data + Head);
			AdvanceIdx(Head);
			--Size;
		}


		/// <summary>Returns reference to the first element in queue.</summary>
		/// <returns>Reference to the first element in queue.</returns>
		T& Front() { HEAVY_ASSERTE(!IsEmpty(), "Trying to access empty queue!"); return Data[Head]; };

		/// <summary>Returns reference to the last element in queue.</summary>
		/// <returns>Reference to the last element in queue.</returns>
		T& Back() { HEAVY_ASSERTE(!IsEmpty(), "Trying to access empty queue!"); return Data[GetPrevIdx(Tail)]; };

		/// <summary>Returns const reference to the first element in queue.</summary>
		/// <returns>Const reference to the first element in queue.</returns>
		const T& Front() const { HEAVY_ASSERTE(!IsEmpty(), "Trying to access empty queue!"); return Data[Head]; };

		/// <summary>Returns const reference to the last element in queue.</summary>
		/// <returns>Const reference to the last element in queue.</returns>
		const T& Back() const { HEAVY_ASSERTE(!IsEmpty(), "Trying to access empty queue!"); return Data[GetPrevIdx(Tail)]; };

		/// <summary>
		/// Ensures that enough space is available in the queue.
		/// In case there is not enought the queue gets reallocated to new, bigger memory block.
		/// </summary>
		/// <param name="capacity">Requested capacity of the queue.</param>
		void Reserve(size_t capacity)
		{
			HEAVY_ASSERTE(capacity >= Capacity, "Capacity too small!");
			Realloc(capacity);
		}

		/// <summary>Checks whether provided object is present in the queue at least once.</summary>
		/// <param name="rhs">Searched object.</param>
		/// <returns>True if present, false otherwise.</returns>
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
			T* newData = Allocate<T>(capacity);

			// move all elements
			for (size_t i = 0; i < Size; ++i)
			{
				size_t oldIdx = GetNthIdx(i);
				ObjectLifetimeHelper::MoveCreate(newData + i, std::move(Data[oldIdx]));
				ObjectLifetimeHelper::Destroy(Data + oldIdx);
			}

			Deallocate(Data);
			Data = newData;
			Capacity = capacity;
			Head = 0;
			Tail = Size;
		}

		void Free() { if (Data) Deallocate(Data); }

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
