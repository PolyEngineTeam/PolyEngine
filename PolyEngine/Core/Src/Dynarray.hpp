#pragma once

#include "ObjectLifetimeHelpers.hpp"
#include "Defines.hpp"
#include "Allocator.hpp"

namespace Poly 
{
	/// Dynarray is a vector based container thet allocates its memory in one, continous block.
	/// This should be the goto container for all general purpose usage.
	template<typename T>
	class Dynarray : public BaseObject<>
	{
	public:

		/// Dynarray's Iterator class provides basic random access mutable iterator API for traversing dynarray memory
		class Iterator : public BaseObject<>, public std::iterator<std::random_access_iterator_tag, T>
		{
		public:
			bool operator==(const Iterator& rhs) const { return Data == rhs.Data && Idx == rhs.Idx; }
			bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }
			bool operator<(const Iterator& rhs) const { HEAVY_ASSERTE(Data == rhs.Data, "Not valid iterator pair");  return Idx < rhs.Idx; }
			bool operator<=(const Iterator& rhs) const { HEAVY_ASSERTE(Data == rhs.Data, "Not valid iterator pair");  return Idx <= rhs.Idx; }
			bool operator>(const Iterator& rhs) const { HEAVY_ASSERTE(Data == rhs.Data, "Not valid iterator pair");  return Idx > rhs.Idx; }
			bool operator>=(const Iterator& rhs) const { HEAVY_ASSERTE(Data == rhs.Data, "Not valid iterator pair");  return Idx >= rhs.Idx; }

			T& operator*() const { return Data[Idx]; }
			T& operator->() const { return Data[Idx]; }

			Iterator& operator++() { ++Idx; return *this; }
			Iterator operator++(int) { Iterator ret(Data, Idx); ++Idx; return ret; }
			Iterator& operator--() { --Idx; return *this; }
			Iterator operator--(int) { Iterator ret(Data, Idx); --Idx; return ret; }

			Iterator operator+(size_t val) const { return Iterator(Data, Idx + val); }
			Iterator operator-(size_t val) const { return Iterator(Data, Idx - val); }
			Iterator& operator+=(size_t val) { Idx += val; return *this; }
			Iterator& operator-=(size_t val) { Idx -= val; return *this; }

			size_t operator-(const Iterator& rhs) const { HEAVY_ASSERTE(Data == rhs.Data, "Not valid iterator pair"); return Idx - rhs.Idx; }
		private:
			Iterator(T* data, size_t idx) : Idx(idx), Data(data) {}

			size_t Idx = 0;
			T* Data = nullptr;
			friend class Dynarray<T>;
		};

		/// Dynarray's ConstIterator class provides basic random access const iterator API for traversing dynarray memory
		class ConstIterator : public BaseObject<>, public std::iterator<std::random_access_iterator_tag, T>
		{
		public:
			bool operator==(const ConstIterator& rhs) const { return Data == rhs.Data && Idx == rhs.Idx; }
			bool operator!=(const ConstIterator& rhs) const { return !(*this == rhs); }
			bool operator<(const ConstIterator& rhs) const { HEAVY_ASSERTE(Data == rhs.Data, "Not valid iterator pair");  return Idx < rhs.Idx; }
			bool operator<=(const ConstIterator& rhs) const { HEAVY_ASSERTE(Data == rhs.Data, "Not valid iterator pair");  return Idx <= rhs.Idx; }
			bool operator>(const ConstIterator& rhs) const { HEAVY_ASSERTE(Data == rhs.Data, "Not valid iterator pair");  return Idx > rhs.Idx; }
			bool operator>=(const ConstIterator& rhs) const { HEAVY_ASSERTE(Data == rhs.Data, "Not valid iterator pair");  return Idx >= rhs.Idx; }

			const T& operator*() const { return Data[Idx]; }
			const T& operator->() const { return Data[Idx]; }

			ConstIterator& operator++() { ++Idx; return *this; }
			ConstIterator operator++(int) { ConstIterator ret(Data, Idx); ++Idx; return ret; }
			ConstIterator& operator--() { --Idx; return *this; }
			ConstIterator operator--(int) { ConstIterator ret(Data, Idx); --Idx; return ret; }

			ConstIterator operator+(size_t val) const { return ConstIterator(Data, Idx + val); }
			ConstIterator operator-(size_t val) const { return ConstIterator(Data, Idx - val); }
			ConstIterator& operator+=(size_t val) { Idx += val; return *this; }
			ConstIterator& operator-=(size_t val) { Idx -= val; return *this; }

			size_t operator-(const ConstIterator& rhs) const { HEAVY_ASSERTE(Data == rhs.Data, "Not valid iterator pair"); return Idx - rhs.Idx; }
		private:
			ConstIterator(const T* data, size_t idx) : Idx(idx), Data(data) {}

			size_t Idx = 0;
			const T* Data = nullptr;
			friend class Dynarray<T>;
		};

		/// Base dynarray constructor that creates empty object with capacity == 0.
		Dynarray() = default;

		/// Creates dynarray instance with provided capacity.
		/// @param[in] size_t capcity
		Dynarray(size_t capacity) { Reserve(capacity); }

		/// Creates dynarray instance from initializer list.
		/// @param[in] const std::initializer_list<T>& list
		Dynarray(const std::initializer_list<T>& list) { PopulateFromInitializerList(list); }

		/// Basic copy constructor
		/// @param[in] const Dynarray<T>& rhs Reference to Dynarray instance which state should be copied.
		Dynarray(const Dynarray<T>& rhs) { Copy(rhs); }

		/// Basic move constructor
		/// @param[in] Dynarray<T>&& rhs R-value reference to Dynarray instance which state should be moved.
		Dynarray(Dynarray<T>&& rhs) { Move(std::forward<Dynarray<T>>(rhs)); }

		/// Basic destructor.
		~Dynarray()
		{
			Clear();
			Free();
		}

		/// Basic copy operator
		/// @param[in] const Dynarray<T>& rhs Reference to Dynarray instance which state should be copied.
		Dynarray<T>& operator=(const Dynarray<T>& rhs)
		{
			Clear();
			Copy(rhs);
			return *this;
		}

		/// Basic move operator
		/// @param[in] Dynarray<T>&& rhs R-value reference to Dynarray instance which state should be moved.
		Dynarray<T>& operator=(Dynarray<T>&& rhs)
		{
			Clear();
			Free();
			Move(std::forward<Dynarray<T>>(rhs));
			return *this;
		}

		/// Clears current dynarray content and populates it with content from initializer list.
		/// @param[in] const std::initializer_list<T>& list
		Dynarray<T>& operator=(const std::initializer_list<T>& list)
		{
			Clear();
			PopulateFromInitializerList(list);
			return *this;
		}

		/// Equal comparison operator with other dynarray
		/// @return bool True if size of the containers match and objects represented by both containers
		/// are identical and in the same order, false otherwise.
		bool operator==(const Dynarray<T>& rhs) const
		{
			if (GetSize() != rhs.GetSize())
				return false;

			for (size_t idx = 0; idx < GetSize(); ++idx)
				if (Data[idx] != rhs.Data[idx])
					return false;

			return true;
		}

		/// Not-equal comparison operator with other dynarray
		/// @return bool True when equal operator returns false, false otherwise.
		bool operator!=(const Dynarray<T>& rhs) const { return !(*this == rhs); };
		

		/// Checks whether dynarray is empty
		/// @return bool True if is empty, false otherwise.
		bool IsEmpty() const { return GetSize() == 0; }

		/// Returns current size of the container
		/// @return size_t Size of the container in objects count.
		size_t GetSize() const { return Size; }

		/// Returns current maximum capacity of the container.
		/// If the capacity is exceeded the container will expand
		/// @return size_t Capacity of the container in objects count.
		size_t GetCapacity() const { return Capacity; }

		/// Returns pointer to raw data which is a continous memory block of size == capacity.
		/// @return T* Pointer to raw container data.
		T* GetData() { return Data; }

		/// Returns const pointer to raw data which is a continous memory block of size == capacity.
		/// @return const T* Const pointer to raw container data.
		const T* GetData() const { return Data; }

		/// Element access operator for dynarray content.
		/// @param[in] size_t idx Index of the element to access. Out of bounds values will cause UB.
		/// @return T& Reference to object under provided index.
		T& operator[](size_t idx) { HEAVY_ASSERTE(idx < GetSize(), "Index out of bounds!"); return Data[idx]; }

		/// Element access operator for const dynarray content.
		/// @param[in] size_t idx Index of the element to access. Out of bounds values will cause UB.
		/// @return const T& Const reference to object under provided index.
		const T& operator[](size_t idx) const { HEAVY_ASSERTE(idx < GetSize(), "Index out of bounds!"); return Data[idx]; }

		//------------------------------------------------------------------------------
		void Clear() {
			for (size_t idx = 0; idx < GetSize(); ++idx)
				ObjectLifetimeHelper::Destroy(Data + idx);
			Size = 0;
		}

		//------------------------------------------------------------------------------
		void Insert(size_t idx, const T& obj)
		{
			HEAVY_ASSERTE(idx <= GetSize(), "Index out of bounds!");
			if (Size == GetCapacity())
				Enlarge();
			std::move_backward(Begin() + idx, End(), End() + 1);
			ObjectLifetimeHelper::CopyCreate(Data + idx, obj);
			++Size;
		}

		//------------------------------------------------------------------------------
		void RemoveByIdx(size_t idx)
		{
			HEAVY_ASSERTE(idx < GetSize(), "Index out of bounds!");
			ObjectLifetimeHelper::Destroy(Data + idx);
			std::move(Begin() + idx + 1, End(), Begin() + idx);
			--Size;
		}

		//------------------------------------------------------------------------------
		size_t FindIdx(const T& rhs) const
		{
			for (size_t idx = 0; idx < GetSize(); ++idx)
			{
				if (Data[idx] == rhs)
					return idx;
			}
			return GetSize();
		}

		//------------------------------------------------------------------------------
		void PushBack(const T& obj) { Insert(GetSize(), obj); }
		void PopBack() { RemoveByIdx(GetSize() - 1); }
		void PushFront(const T& obj) { Insert(0, obj); }
		void PopFront() { RemoveByIdx(0); }

		//------------------------------------------------------------------------------
		void Resize(size_t size)
		{
			if (size == GetSize())
				return;

			if (size < GetSize())
			{
				// remove excessive elements
				for (size_t idx = GetSize(); idx < size; ++idx)
					ObjectLifetimeHelper::Destroy(Data + idx);
			}
			else
			{
				Reserve(size);
				for (size_t idx = GetSize(); idx < size; ++idx)
					ObjectLifetimeHelper::DefaultCreate(Data + idx);
			}
			Size = size;
		}

		//------------------------------------------------------------------------------
		void Reserve(size_t capacity)
		{
			if (capacity <= Capacity)
				return;
			Realloc(capacity);
		}

		//------------------------------------------------------------------------------
		Iterator Begin() { return Iterator(Data, 0); }
		Iterator End() { return Iterator(Data, GetSize()); }
		ConstIterator Begin() const { return ConstIterator(Data, 0); }
		ConstIterator End() const { return ConstIterator(Data, GetSize()); }

		//------------------------------------------------------------------------------
		Iterator Find(const T& rhs) { return Iterator(Data, FindIdx(rhs)); }
		ConstIterator Find(const T& rhs) const { return ConstIterator(Data, FindIdx(rhs)); }

		//------------------------------------------------------------------------------
		bool Contains(const T& rhs) const { return FindIdx(rhs) < GetSize(); }
		void Remove(const T& rhs) { RemoveByIdx(FindIdx(rhs)); }
		bool TryRemove(const T& rhs)
		{
			size_t idx = FindIdx(rhs);
			if (idx < GetSize())
			{
				RemoveByIdx(idx);
				return true;
			}
			return false;
		}

	private:
		//------------------------------------------------------------------------------
		void Realloc(size_t capacity)
		{
			HEAVY_ASSERTE(Size <= capacity, "Invalid resize capacity!");
			T* newData = static_cast<T*>(DefaultAlloc(capacity * sizeof(T)));

			// move all elements
			for (size_t i = 0; i < Size; ++i)
			{
				ObjectLifetimeHelper::MoveCreate(newData + i, std::move(Data[i]));
				ObjectLifetimeHelper::Destroy(Data + i);
			}

			DefaultFree(Data);
			Data = newData;
			Capacity = capacity;
		}

		void Free() { if (Data) DefaultFree(Data); }

		//------------------------------------------------------------------------------
		void Copy(const Dynarray<T>& rhs)
		{
			Reserve(rhs.GetSize());
			for (size_t idx = 0; idx < rhs.GetSize(); ++idx)
				PushBack(rhs.Data[idx]);
		}

		//------------------------------------------------------------------------------
		void Move(Dynarray<T>&& rhs)
		{
			Size = rhs.Size;
			Capacity = rhs.Capacity;
			Data = rhs.Data;
			rhs.Data = nullptr;
			rhs.Size = 0;
			rhs.Capacity = 0;
		}

		//------------------------------------------------------------------------------
		void PopulateFromInitializerList(const std::initializer_list<T>& list)
		{
			Reserve(list.size());
			for (const T& obj : list)
				PushBack(obj);
		}

		void Enlarge() { Reserve((Capacity + 1) * 2); }

		size_t Size = 0;
		size_t Capacity = 0;
		T* Data = nullptr;
	};

	// std library for each enablers
	template <typename T> typename Poly::Dynarray<T>::Iterator begin(Poly::Dynarray<T>& rhs) { return rhs.Begin(); }
	template <typename T> typename Poly::Dynarray<T>::Iterator end(Poly::Dynarray<T>& rhs) { return rhs.End(); }
	template <typename T> typename Poly::Dynarray<T>::ConstIterator begin(const Poly::Dynarray<T>& rhs) { return rhs.Begin(); }
	template <typename T> typename Poly::Dynarray<T>::ConstIterator end(const Poly::Dynarray<T>& rhs) { return rhs.End(); }
}
