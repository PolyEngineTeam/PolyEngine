#pragma once

#include <pe/Defines.hpp>
#include <pe/core/memory/ObjectLifetimeHelpers.hpp>

namespace pe::core::storage
{
	/// <summary>
	/// Queue is a container basing on type passed as argument
	/// provides API that is very fast when instering/popping from back/front.
	/// <para>This should be the goto container for any queueing purposes, like FIFO or LIFO.
	/// It does not provide API for insertion in any position.</para>
	/// </summary>
    template<class T>
    class Queue : public BaseObject<>
    {
    public:
        /// <summary>Base queue constructor that creates empty object with capacity == 0.</summary>
        Queue() = default;

        /// <summary>Creates queue instance with provided capacity.</summary>
		/// <param name="capacity"></param>
		explicit Queue(size_t capacity) { m_data.reserve(capacity); }

		/// <summary>Basic copy constructor</summary>
		/// <param name="rhs">Reference to Queue instance which state should be copied.</param>
		Queue(const Queue<T>& rhs) { copy(rhs); }

		/// <summary>Basic move constructor</summary>
		/// <param name="rhs">R-value reference to Queue instance which state should be moved.</param>
		Queue(Queue<T>&& rhs) { move(std::forward<Queue<T>>(rhs)); }
		
		/// <summary>Creates queue instance from initializer list.</summary>
		/// <param name="list"></param>
		Queue(const std::initializer_list<T>& list) { populateFromInitializerList(list); }

		/// <summary>Basic destructor.</summary>
		~Queue()
		{
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

			for (size_t n = 0; n < getSize(); ++n) // maybe just normal vector comparison for equality suffices?
				if (m_data.at(getNthIdx(n)) != rhs.m_data.at(rhs.getNthIdx(n)))
					return false;

			return true;
		}

		/// <summary>Not-equal comparison operator with other queue</summary>
		/// @<returns>True when equal operator returns false, false otherwise.</returns>
		bool operator!=(const Queue<T>& rhs) const { return !(*this == rhs); };

		/// <summary>Checks whether queue is empty</summary>
		/// <returns>True if is empty, false otherwise.</returns>
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
			m_data.clear();
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
			::pe::core::memory::ObjectLifetimeHelper::MoveCreate(m_data.data() + m_tail, std::move(obj));
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
			::pe::core::memory::ObjectLifetimeHelper::MoveCreate(m_data.data() + m_head, std::move(obj));
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
		void popBack()
		{
			HEAVY_ASSERTE(!isEmpty(), "Trying to access empty queue!");
			decreaseIdx(m_tail);
			::pe::core::memory::ObjectLifetimeHelper::Destroy(m_data.data() + m_tail);
			--m_size;
		}

		/// <summary>Performs removal from the front of the queue.</summary>
		void popFront()
		{
			HEAVY_ASSERTE(!isEmpty(), "Trying to access empty queue!");
			::pe::core::memory::ObjectLifetimeHelper::Destroy(m_data.data() + m_head);
			advanceIdx(m_head);
			--m_size;
		}


		/// <summary>Returns reference to the first element in queue.</summary>
		/// <returns>Reference to the first element in queue.</returns>
		T& front() { HEAVY_ASSERTE(!isEmpty(), "Trying to access empty queue!"); return m_data[m_head]; };

		/// <summary>Returns reference to the last element in queue.</summary>
		/// <returns>Reference to the last element in queue.</returns>
		T& back() { HEAVY_ASSERTE(!isEmpty(), "Trying to access empty queue!"); return m_data[getPrevIdx(m_tail)]; };

		/// <summary>Returns const reference to the first element in queue.</summary>
		/// <returns>Const reference to the first element in queue.</returns>
		const T& front() const { HEAVY_ASSERTE(!isEmpty(), "Trying to access empty queue!"); return m_data[m_head]; };

		/// <summary>Returns const reference to the last element in queue.</summary>
		/// <returns>Const reference to the last element in queue.</returns>
		const T& back() const { HEAVY_ASSERTE(!isEmpty(), "Trying to access empty queue!"); return m_data[getPrevIdx(m_tail)]; };

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

		/// <summary>Checks whether provided object is present in the queue at least once.</summary>
		/// <param name="rhs">Searched object.</param>
		/// <returns>True if present, false otherwise.</returns>
		bool contains(const T& rhs) const { return findIdx(rhs) < getSize(); }
		
		class Iterator : public BaseObject<>
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
			Iterator& operator=(const Iterator& rhs) = default;

			T& operator*() const { return ref.m_data.at(idx); }

			Iterator& operator++() { idx = ref.getNextIdx(idx); return *this; }
			Iterator operator++(int) { Iterator ret(ref, idx); idx = ref.getNextIdx(idx); return ret; }
			Iterator& operator--() { idx = ref.getNextIdx(idx); return *this; }
			Iterator operator--(int) { Iterator ret(ref, idx); idx = ref.getNextIdx(idx); return ret; }
		private:
			Iterator(Queue<T>& queue, size_t index) : ref(queue), idx(index) {};
			Queue<T>& ref;
			size_t idx;
		};

		class ConstIterator : public BaseObject<>
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

			const T& operator*() const { return ref.m_data.at(idx); }

			ConstIterator& operator++() { idx = ref.getNextIdx(idx); return *this; }
			ConstIterator operator++(int) { ConstIterator ret(ref, idx); idx = ref.getNextIdx(idx); return ret; }
			ConstIterator& operator--() { idx = ref.getNextIdx(idx); return *this; }
			ConstIterator operator--(int) { ConstIterator ret(ref, idx); idx = ref.getNextIdx(idx); return ret; }
		private:
			ConstIterator(const Queue<T>& queue, size_t index) : ref(queue), idx(index) {};
			const Queue<T>& ref;
			size_t idx;

		};

		class ReverseIterator : public BaseObject<>
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

			T& operator*() const { return ref.m_data.at(idx); }

			ReverseIterator& operator++() { increment(); return *this; }
			ReverseIterator operator++(int) { ReverseIterator ret(ref, idx); increment(); return ret; }
			ReverseIterator& operator--() { idx = ref.getPrevIdx(idx); return *this; }
			ReverseIterator operator--(int) { ReverseIterator ret(ref, idx); idx = ref.getPrevIdx(idx); return ret; }
		private:
			ReverseIterator(Queue<T>& queue, size_t index) : ref(queue), idx(index) {};
			void increment()
			{
				if(idx == ref.m_head)
					idx = ref.m_tail;
				else 
					idx = ref.getPrevIdx(idx);
			}
			Queue<T>& ref;
			size_t idx;
		};

		class ConstReverseIterator : public BaseObject<>
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

			const T& operator*() const { return ref.m_data.at(idx); }

			ConstReverseIterator& operator++() { increment(); return *this; }
			ConstReverseIterator operator++(int) { ConstReverseIterator ret(ref, idx); increment(); return ret; }
			ConstReverseIterator& operator--() { idx = ref.getPrevIdx(idx); return *this; }
			ConstReverseIterator operator--(int) { ConstReverseIterator ret(ref, idx); idx = ref.getPrevIdx(idx); return ret; }
		private:
			ConstReverseIterator(const Queue<T>& queue, size_t index) : ref(queue), idx(index) {};
			void increment()
			{
				if(idx == ref.m_head)
					idx = ref.m_tail;
				else 
					idx = ref.getPrevIdx(idx);
			}
			const Queue<T>& ref;
			size_t idx;
		};

		Iterator begin() { return Iterator(*this, m_head); }
		Iterator end() { return Iterator(*this, m_tail); }
		ConstIterator cbegin() const { return ConstIterator(*this, m_head); }
		ConstIterator cend() const { return ConstIterator(*this, m_tail); }
		ReverseIterator rbegin() { return ReverseIterator(*this, m_tail - 1); }
		ReverseIterator rend() { return ReverseIterator(*this, m_tail); }
		ConstReverseIterator crbegin() const { return ConstReverseIterator(*this, m_tail - 1); }
		ConstReverseIterator crend() const { return ConstReverseIterator(*this, m_tail); }

	private:
		//------------------------------------------------------------------------------
		size_t getPrevIdx(size_t idx) const { return (idx == 0) ? m_capacity - 1 : idx - 1; }
		size_t getNextIdx(size_t idx) const { return (idx + 1) % m_capacity; }
		void decreaseIdx(size_t& idx) const { idx = getPrevIdx(idx); }
		void advanceIdx(size_t& idx) const { idx = getNextIdx(idx); }
		size_t getNthIdx(size_t& n)	const { return (m_head + n) % m_capacity; }

		//------------------------------------------------------------------------------
		size_t findIdx(const T& rhs) const
		{
			for (size_t i = 0; i < m_size; ++i)
				if (m_data.at(getNthIdx(i)) == rhs)
					return getNthIdx(i);

			return getSize();
		}

		//------------------------------------------------------------------------------
		void realloc(size_t capacity)
		{
			HEAVY_ASSERTE(m_size <= capacity, "Invalid resize capacity!");
			m_data.resize(capacity);

			m_capacity = capacity;
			m_head = 0;
			m_tail = m_size;
		}

		//------------------------------------------------------------------------------
		void copy(const Queue<T>& rhs)
		{
			reserve(rhs.getSize());
			m_data = rhs.m_data;
			m_size = rhs.m_size;
			m_head = rhs.m_head;
			m_tail = rhs.m_tail;
			m_capacity = rhs.m_capacity;
		}

		//------------------------------------------------------------------------------
		void move(Queue<T>&& rhs)
		{
			m_size = rhs.m_size;
			m_capacity = rhs.m_capacity;
			m_data = std::move(rhs.m_data);
			m_head = rhs.m_head;
			m_tail = rhs.m_tail;
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
		std::vector<T> m_data;
    };
	
	template<typename T> typename ::pe::core::storage::Queue<T>::Iterator begin(::pe::core::storage::Queue<T>& rhs) { return rhs.begin(); }
	template<typename T> typename ::pe::core::storage::Queue<T>::Iterator end(::pe::core::storage::Queue<T>& rhs) { return rhs.end(); }
}