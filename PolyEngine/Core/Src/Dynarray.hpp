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
		/// If the capacity is exceeded the container will have to expand.
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

		/// Clears contents of the container. This does not release aquired memory.
		void Clear() {
			for (size_t idx = 0; idx < GetSize(); ++idx)
				ObjectLifetimeHelper::Destroy(Data + idx);
			Size = 0;
		}

		/// Copies provided object to the specified location in the dynarray.
		/// Objects already present that are in position >= idx will be moved one position to the right.
		/// Insertion with idx > size results in undefined behaviour.
		/// @param[in] size_t idx Index in which object should be created.
		/// @param[in] const T& obj Const reference to object that should be copied to the container.
		void Insert(size_t idx, const T& obj)
		{
			HEAVY_ASSERTE(idx <= GetSize(), "Index out of bounds!");
			if (Size == GetCapacity())
				Enlarge();
			std::move_backward(Begin() + idx, End(), End() + 1);
			ObjectLifetimeHelper::CopyCreate(Data + idx, obj);
			++Size;
		}

		/// Removes element from the collection with specified index.
		/// Objects in position > idx will be moved one position to the left.
		/// Removal of object in idx >= size results in undefined behavour.
		/// @param[in] size_t idx Index from which object should be removed.
		void RemoveByIdx(size_t idx)
		{
			HEAVY_ASSERTE(idx < GetSize(), "Index out of bounds!");
			ObjectLifetimeHelper::Destroy(Data + idx);
			std::move(Begin() + idx + 1, End(), Begin() + idx);
			--Size;
		}

		/// Finds index of the first encountered object from the container that is equal to provided object.
		/// @param[in] const T& rhs Searched object.
		/// @return size_t Index of searched object or container size if object was not found.
		size_t FindIdx(const T& rhs) const
		{
			for (size_t idx = 0; idx < GetSize(); ++idx)
			{
				if (Data[idx] == rhs)
					return idx;
			}
			return GetSize();
		}

		/// Performs insertion to the back of the container.
		/// @param[in] const T& obj Const reference to object that should be copied to the container.
		void PushBack(const T& obj) { Insert(GetSize(), obj); }

		/// Performs removal from the back of the container.
		void PopBack() { RemoveByIdx(GetSize() - 1); }

		/// Performs insertion to the front of the container.
		/// @param[in] const T& obj Const reference to object that should be copied to the container.
		void PushFront(const T& obj) { Insert(0, obj); }

		/// Performs removal from the front of the container.
		void PopFront() { RemoveByIdx(0); }

		/// Forces resizing of the collection to the specified value.
		/// If necessary this will create new object via default empty construction.
		/// This can also shrink the collection in which case, excessive objects will be destroyed.
		/// @param[in] size_t Requested size of the collection.
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

		/// Ensures that enough space is available in the collection.
		/// In case there is not enought the container gets reallocated to new, bigger memory block.
		/// @param[in] size_t Requested capacity of the collection.
		void Reserve(size_t capacity)
		{
			if (capacity <= Capacity)
				return;
			Realloc(capacity);
		}

		/// Getter for the iterator to the element in front of the collection.
		Iterator Begin() { return Iterator(Data, 0); }

		/// Getter for the iterator to the location after last element in the collection.
		Iterator End() { return Iterator(Data, GetSize()); }

		/// Getter for the const iterator to the element in front of the collection.
		ConstIterator Begin() const { return ConstIterator(Data, 0); }

		/// Getter for the const iterator to the location after last element in the collection.
		ConstIterator End() const { return ConstIterator(Data, GetSize()); }

		/// Getter for the iterator that points to the first encountered searched element in collection.
		/// @param[in] const T& rhs Searched object.
		/// @return Iterator Iterator to the searched object or End().
		Iterator Find(const T& rhs) { return Iterator(Data, FindIdx(rhs)); }

		/// Getter for the const iterator that points to the first encountered searched element in collection.
		/// @param[in] const T& rhs Searched object.
		/// @return ConstIterator Const iterator to the searched object or End().
		ConstIterator Find(const T& rhs) const { return ConstIterator(Data, FindIdx(rhs)); }

		/// Checks whether provided object is present in the collection at least once.
		/// @param[in] const T& rhs Searched object.
		/// @return bool True if present, false otherwise.
		bool Contains(const T& rhs) const { return FindIdx(rhs) < GetSize(); }

		/// Remove the first encountered object of provided value from the collection.
		/// This will cause undefined behaviour when the value is not present in the collection.
		/// Use TryRemove if not sure about object existence.
		/// @param[in] const T& rhs Object to be removed.
		void Remove(const T& rhs) { RemoveByIdx(FindIdx(rhs)); }

		/// Try to remove the first encountered object of provided value from the collection.
		/// When value is not found in collection this method does nothing.
		/// @param[in] const T& rhs Object to be removed.
		/// @return bool True if removal succeded, false if value was not found.
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
