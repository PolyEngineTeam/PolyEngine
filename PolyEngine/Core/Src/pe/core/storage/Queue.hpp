#pragma once

#include <pe/Defines.hpp>
#include <pe/core/memory/Allocator.hpp>
#include <pe/core/memory/ObjectLifetimeHelpers.hpp>

namespace pe::core::storage
{
	/// <summary>
	/// Queue is a vector based container thet allocates its memory in one, continous block and provides
	/// API that is very fast when instering/popping from back/front.
	/// <para>This should be the goto container for any queueing purposes, like FIFO or LIFO.
	/// It does not provide API for insertion in any position.</para>
	/// </summary>
    template<typename T>
    class Queue : public BaseObjectLiteralType<>
    {
    public:
        /// <summary>Base queue constructor that creates empty object with capacity == 0.</summary>
        Queue() = default;

        /// <summary>Creates queue instance with provided capacity.</summary>
		/// <param name="capacity"></param>
		explicit Queue(size_t capacity) { reserve(capacity); }

		/// <summary>Basic copy constructor</summary>
		/// <param name="rhs">Reference to Queue instance which state should be copied.</param>
		Queue(const Queue<T>& rhs) { copy(rhs); }

		/// <summary>Basic move constructor</summary>
		/// <param name="rhs">R-value reference to Queue instance which state should be moved.</param>
		Queue(Queue<T>&& rhs) { move(std::move(rhs)); }
		
		/// <summary>Creates queue instance from initializer list.</summary>
		/// <param name="list"></param>
		Queue(const std::initializer_list<T>& list) { populateFromInitializerList(list); }

		/// <summary>Basic destructor.</summary>
		~Queue()
		{
			clear();
			free();
		}

		/// <summary>Basic copy operator</summary>
		/// <param name="rhs">Reference to Queue instance which state should be copied.</param>
		Queue<T>& operator=(const Queue<T>& rhs)
		{
			clear();
			copy(rhs);
			return *this;
		}

		/// <summary>Basic move operator</summary>
		/// <param name="rhs">R-value reference to Queue instance which state should be moved.</param>
		Queue<T>& operator=(Queue<T>&& rhs)
		{
			clear();
			free();
			move(std::forward<Queue<T>>(rhs));
			return *this;
		}

		/// <summary>Clears current queue content and populates it with content from initializer list.</summary>
		/// <param name="list"></param>
		Queue<T>& operator=(const std::initializer_list<T>& list)
		{
			clear();
			populateFromInitializerList(list);
			return *this;
		}

		/// <summary>Equal comparison operator with other queue</summary>
		/// <returns>
		/// True if size of the queues match and objects represented by both of them
		/// are identical and in the same order, false otherwise.
		/// </returns>
		bool operator==(const Queue<T>& rhs) const
		{
			if (getSize() != rhs.getSize())
				return false;

			for (size_t n = 0; n < getSize(); ++n) 
				if (m_data[getNthIdx(n)] != rhs.m_data[rhs.getNthIdx(n)])
					return false;

			return true;
		}

		/// <summary>Not-equal comparison operator with other queue</summary>
		/// <returns>True when equal operator returns false, false otherwise.</returns>
		bool operator!=(const Queue<T>& rhs) const { return !(*this == rhs); };

		/// <summary>Checks whether queue is empty</summa()y>
		/// <returns>True if is empty, false otherwise.</()eturns>
		bool isEmpty() const { return getSize() == 0; }

		/// <summary>Returns current size of the queue</summary>
		/// <returns>Size of the queue in objects count.</returns>
		size_t getSize() const { return m_size; }

		/// <summary>
		/// Returns current maximum capacity of the queue.
		/// If the capacity is exceeded the queue will have to expand.
		/// </summary>
		/// <returns>Capacity of the queue in objects count.</returns>
		size_t getCapacity() const { return m_capacity; }

		/// <summary>Clears contents of the queue. </summary>
		void clear()
		{
			for (size_t i = 0; i < m_size; ++i)
				::pe::core::memory::ObjectLifetimeHelper::Destroy(m_data + getNthIdx(i));
			m_size = 0;
			m_head = 0;
			m_tail = 0;
		}

		/// <summary>Performs insertion to the back of the queue.</summary>
		/// <param name="obj">Const reference to object that should be copied to the queue.</param>
		void pushBack(T obj)
		{
			if (getSize() >= getCapacity())
				enlarge();
			::pe::core::memory::ObjectLifetimeHelper::MoveCreate((m_data + m_tail), std::move(obj));
			advanceIdx(m_tail);
			m_size++;
		}

		/// <summary>Performs insertion to the front of the queue.</summary>
		/// <param name="obj">Const reference to object that should be copied to the queue.</param>
		void pushFront(T obj)
		{
			if (getSize() >= getCapacity())
				enlarge();
			decreaseIdx(m_head);
			::pe::core::memory::ObjectLifetimeHelper::MoveCreate((m_data + m_head), std::move(obj));
			m_size++;
		}

	/*
		//TODO(celeb): implement MoveEmplace in ObjectLifetimeHelper
		template<typename... Args>
		void emplaceBack(Args&&... args)
		{
		}

		template<typename... Args>
		void emplaceFront(Args&&... args)
		{
		}
	*/
		/// <summary>Performs removal from the back of the queue.</summary>
		/// <returns>The last element in queue.</returns>
		T popBack()
		{
			HEAVY_ASSERTE(!isEmpty(), "Trying to access empty queue!");
			decreaseIdx(m_tail);
			T val = m_data[m_tail];
			::pe::core::memory::ObjectLifetimeHelper::Destroy(m_data + m_tail);
			--m_size;
			return val;	
		}

		/// <summary>Performs removal from the front of the queue.</summary>
		/// <returns>The first element in queue.</returns>
		T popFront()
		{
			HEAVY_ASSERTE(!isEmpty(), "Trying to access empty queue!");
			T val = m_data[m_head];
			::pe::core::memory::ObjectLifetimeHelper::Destroy(m_data + m_head);
			advanceIdx(m_head);
			--m_size;
			return val;
		}

		/// <summary>Returns reference to the first element in queue.</summary>
		/// <returns>Reference to the first element in queue.</returns>
		T& front() { HEAVY_ASSERTE(!isEmpty(), "Trying to access empty queue!"); return m_data[m_head]; };

		/// <summary>Returns reference to the last element in queue.</summary>
		/// <returns>Reference to the last element in queue.</returns>
		T& back() { HEAVY_ASSERTE(!isEmpty(), "Trying to access empty queue!"); return m_data[getPrevCellIdx(m_tail)]; };

		/// <summary>Returns const reference to the first element in queue.</summary>
		/// <returns>Const reference to the first element in queue.</returns>
		const T& front() const { HEAVY_ASSERTE(!isEmpty(), "Trying to access empty queue!"); return m_data[m_head]; };

		/// <summary>Returns const reference to the last element in queue.</summary>
		/// <returns>Const reference to the last element in queue.</returns>
		const T& back() const { HEAVY_ASSERTE(!isEmpty(), "Trying to access empty queue!"); return m_data[getPrevCellIdx(m_tail)]; };

		/// <summary>
		/// Ensures that enough space is available in the queue.
		/// In case there is not enought the queue gets reallocated to new, bigger memory block.
		/// </summary>
		/// <param name="capacity">Requested capacity of the queue.</param>
		void reserve(size_t capacity)
		{
			HEAVY_ASSERTE(capacity >= m_capacity, "Capacity too small!");
			realloc(capacity);
		}

		class Iterator final : public BaseObjectLiteralType<>
		{
			friend class Queue<T>;
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			bool operator==(const Iterator& rhs) const { return idx == rhs.idx; }
			bool operator!=(const Iterator& rhs) const { return idx != rhs.idx; }

			T& operator*() const { return q->m_data[idx]; }
			T* operator->() const { return q->m_data[idx]; }

			Iterator& operator++() { idx = q->getNextIdx(idx); return *this; }
			Iterator operator++(int) { Iterator ret(q, idx); idx = q->getNextIdx(idx); return ret; }
			Iterator& operator--() { idx = q->getPrevIdx(idx); return *this; }
			Iterator operator--(int) { Iterator ret(q, idx); idx = q->getPrevIdx(idx); return ret; }
		private:
			Iterator(Queue<T>* queue, size_t index) : q(queue), idx(index) {};
			Queue<T>* q;
			size_t idx;
		};

		class ConstIterator final : public BaseObjectLiteralType<>
		{
			friend class Queue<T>;
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			bool operator==(const ConstIterator& rhs) const { return idx == rhs.idx; }
			bool operator!=(const ConstIterator& rhs) const { return idx != rhs.idx; }
			
			const T& operator*() const { return q->m_data[idx]; }
			const T* operator->() const { return q->m_data[idx]; }

			ConstIterator& operator++() { idx = q->getNextIdx(idx); return *this; }
			ConstIterator operator++(int) { ConstIterator ret(q, idx); idx = q->getNextIdx(idx); return ret; }
			ConstIterator& operator--() { idx = q->getPrevIdx(idx); return *this; }
			ConstIterator operator--(int) { ConstIterator ret(q, idx); idx = q->getPrevIdx(idx); return ret; }
		private:
			ConstIterator(const Queue<T>* queue, size_t index) : q(queue), idx(index) {};
			const Queue<T>* q;
			size_t idx;
		};

		class ReverseIterator final : public BaseObjectLiteralType<>
		{
			friend class Queue<T>;
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			bool operator==(const ReverseIterator& rhs) const { return idx == rhs.idx; }
			bool operator!=(const ReverseIterator& rhs) const { return idx != rhs.idx; }

			T& operator*() const { return q->m_data[idx]; }
			T* operator->() const { return q->m_data[idx]; }

			ReverseIterator& operator++() { idx = q->getPrevIdx(idx); return *this; }
			ReverseIterator operator++(int) { ReverseIterator ret(q, idx); idx = q->getPrevIdx(idx); return ret; }
			ReverseIterator& operator--() { idx = q->getNextIdx(idx); return *this; }
			ReverseIterator operator--(int) { ReverseIterator ret(q, idx); idx = q->getNextIdx(idx); return ret; }
		private:
			ReverseIterator(Queue<T>* queue, size_t index) : q(queue), idx(index) {};
			Queue<T>* q;
			size_t idx;
		};

		class ConstReverseIterator final : public BaseObjectLiteralType<>
		{
			friend class Queue<T>;
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			bool operator==(const ConstReverseIterator& rhs) const { return idx == rhs.idx; }
			bool operator!=(const ConstReverseIterator& rhs) const { return idx != rhs.idx; }
			
			const T& operator*() const { return q->m_data[idx]; }
			const T* operator->() const { return q->m_data[idx]; }

			ConstReverseIterator& operator++() { idx = q->getPrevIdx(idx); return *this; }
			ConstReverseIterator operator++(int) { ConstReverseIterator ret(q, idx); idx = q->getPrevIdx(idx); return ret; }
			ConstReverseIterator& operator--() { idx = q->getNextIdx(idx); return *this; }
			ConstReverseIterator operator--(int) { ConstReverseIterator ret(q, idx); idx = q->getNextIdx(idx); return ret; }
		private:
			ConstReverseIterator(const Queue<T>* queue, size_t index) : q(queue), idx(index) {};
			const Queue<T>* q;
			size_t idx;
		};

		Iterator begin() { return Iterator(this, m_head); }
		Iterator end() { return Iterator(this, m_tail); }
		ConstIterator cbegin() const { return ConstIterator(this, m_head); }
		ConstIterator cend() const { return ConstIterator(this, m_tail); }
		ReverseIterator rbegin() { return ReverseIterator(this, m_tail ? m_tail - 1 : m_tail); }
		ReverseIterator rend() { return ReverseIterator(this, m_tail); }
		ConstReverseIterator crbegin() const { return ConstReverseIterator(this, m_tail ? m_tail - 1 : m_tail); }
		ConstReverseIterator crend() const { return ConstReverseIterator(this, m_tail); }

	private:
		//------------------------------------------------------------------------------
		size_t getPrevIdx(size_t idx) const
		{
			const size_t sizeWithGuard = m_size + 1;
			const size_t prevOffset = (idx + m_capacity - 1 - m_head) % m_capacity;
			const size_t sizeCorrectedPrevOffset = prevOffset % sizeWithGuard;
			return (m_head + sizeCorrectedPrevOffset) % m_capacity;
		}
		
		size_t getNextIdx(size_t idx) const
		{
			const size_t sizeWithGuard = m_size + 1;
			const size_t nextOffset = (idx + m_capacity + 1 - m_head) % m_capacity;
			const size_t sizeCorrectedNextOffset = nextOffset % sizeWithGuard;
			return (m_head + sizeCorrectedNextOffset) % m_capacity;
		}

		size_t getPrevCellIdx(size_t idx) const
		{
			return (idx + m_capacity - 1) % m_capacity;
		}	
		
		size_t getNextCellIdx(size_t idx) const
		{
			return (idx + 1) % m_capacity;
		}	

		void decreaseIdx(size_t& idx) const { idx = getPrevCellIdx(idx); }
		void advanceIdx(size_t& idx) const { idx = getNextCellIdx(idx); }
		size_t getNthIdx(size_t n)	const { return (m_head + n) % m_size; }
		
		//------------------------------------------------------------------------------
		void realloc(size_t capacity)
		{
			HEAVY_ASSERTE(m_size <= capacity, "Invalid resize capacity!");
			T* newData = ::pe::core::memory::Allocate<T>(capacity);

			// move all elements
			for (size_t i = 0; i < m_size; ++i)
			{
				size_t oldIdx = getNthIdx(i);
				::pe::core::memory::ObjectLifetimeHelper::MoveCreate(newData + i, std::move(m_data[oldIdx]));
				::pe::core::memory::ObjectLifetimeHelper::Destroy(m_data + oldIdx);
			}

			::pe::core::memory::Deallocate(m_data);
			m_data = newData;
			m_capacity = capacity;
			m_head = 0;
			m_tail = m_size;
		}

		//------------------------------------------------------------------------------
		void free() { if (m_data) ::pe::core::memory::Deallocate(m_data); }

		//------------------------------------------------------------------------------
		void copy(const Queue<T>& rhs)
		{
			reserve(rhs.getSize());
			for (size_t i = 0; i < rhs.m_size; ++i)
				pushBack(rhs.m_data[rhs.getNthIdx(i)]);
		}

		//------------------------------------------------------------------------------
		void move(Queue<T>&& rhs)
		{
			m_size = rhs.m_size;
			m_capacity = rhs.m_capacity;
			m_data = rhs.m_data;
			m_head = rhs.m_head;
			m_tail = rhs.m_tail;
			rhs.m_data = nullptr;
			rhs.m_size = 0;
			rhs.m_capacity = 0;
			rhs.m_head = 0;
			rhs.m_tail = 0;
		}

		//------------------------------------------------------------------------------
		void populateFromInitializerList(const std::initializer_list<T>& list)
		{
			reserve(list.size());
			for (const T& obj : list)
				pushBack(obj);
		}

		void enlarge() { reserve((m_capacity + 1) * 2); }

	private:
		size_t m_head = 0;
		size_t m_tail = 0;
		size_t m_size = 0;
		size_t m_capacity = 0;
		T* m_data = nullptr;
    };
}