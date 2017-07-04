#pragma once

#include "ObjectLifetimeHelpers.hpp"
#include "Defines.hpp"
#include "Allocator.hpp"

namespace Poly 
{
	/// <summary>
	/// Dynarray is a vector based container thet allocates its memory in one, continous block.
	/// This should be the goto container for all general purpose usage.
	/// </summary>
	template<typename T>
	class Dynarray : public BaseObject<>
	{
	public:

		/// <summary>Dynarray's Iterator class provides basic random access mutable iterator API for traversing dynarray memory</summary>
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
			T* operator->() const { return Data + Idx; }

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

		/// <summary>Dynarray's ConstIterator class provides basic random access const iterator API for traversing dynarray memory</summary>
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
			const T* operator->() const { return Data + Idx; }

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

		/// <summary>Base dynarray constructor that creates empty object with capacity == 0.</summary>
		Dynarray() = default;

		/// <summary>Creates dynarray instance with provided capacity.</summary>
		/// <param name="capacity"></param>
		explicit Dynarray(size_t capacity) { Reserve(capacity); }

		/// <summary>Creates dynarray instance from initializer list.</summary>
		/// <param name="list"></param>
		Dynarray(const std::initializer_list<T>& list) { PopulateFromInitializerList(list); }

		/// <summary>Basic copy constructor</summary>
		/// <param name="rhs">Reference to Dynarray instance which state should be copied.</param>
		Dynarray(const Dynarray<T>& rhs) { Copy(rhs); }

		/// <summary>Basic move constructor</summary>
		/// <param name="rhs">R-value reference to Dynarray instance which state should be moved.</param>
		Dynarray(Dynarray<T>&& rhs) { Move(std::forward<Dynarray<T>>(rhs)); }

		/// <summary>Basic destructor.</summary>
		~Dynarray()
		{
			Clear();
			Free();
		}

		/// <summary>Basic copy operator</summary>
		/// <param name="rhs">Reference to Dynarray instance which state should be copied.</param>
		Dynarray<T>& operator=(const Dynarray<T>& rhs)
		{
			Clear();
			Copy(rhs);
			return *this;
		}

		/// <summary>Basic move operator</summary>
		/// <param name="rhs">R-value reference to Dynarray instance which state should be moved.</param>
		Dynarray<T>& operator=(Dynarray<T>&& rhs)
		{
			Clear();
			Free();
			Move(std::forward<Dynarray<T>>(rhs));
			return *this;
		}

		/// <summary>Clears current dynarray content and populates it with content from initializer list.</summary>
		/// <param name="list"></param>
		Dynarray<T>& operator=(const std::initializer_list<T>& list)
		{
			Clear();
			PopulateFromInitializerList(list);
			return *this;
		}

		/// <summary>Equal comparison operator with other dynarray.</summary>
		/// <returns>True if size of the containers match and objects represented by both containers
		/// are identical and in the same order, false otherwise.</returns>
		bool operator==(const Dynarray<T>& rhs) const
		{
			if (GetSize() != rhs.GetSize())
				return false;

			for (size_t idx = 0; idx < GetSize(); ++idx)
				if (Data[idx] != rhs.Data[idx])
					return false;

			return true;
		}

		/// <summary>Not-equal comparison operator with other dynarray.</summary>
		/// <returns>bool True when equal operator returns false, false otherwise.</returns>
		bool operator!=(const Dynarray<T>& rhs) const { return !(*this == rhs); };
		
		/// <summary>Checks whether dynarray is empty.</summary>
		/// <returns>True if is empty, false otherwise.</returns>
		bool IsEmpty() const { return GetSize() == 0; }

		/// <summary>Returns current size of the container</summary>
		/// <returns>Size of the container in objects count.</returns>
		size_t GetSize() const { return Size; }

		/// <summary>
		/// Returns current maximum capacity of the container.
		/// If the capacity is exceeded the container will have to expand.
		/// </summary>
		/// <returns>Capacity of the container in objects count.</returns>
		size_t GetCapacity() const { return Capacity; }

		/// <summary>Returns pointer to raw data which is a continous memory block of size == capacity.</summary>
		/// <returns>Pointer to raw container data.</returns>
		T* GetData() { return Data; }

		/// <summary>Returns const pointer to raw data which is a continous memory block of size == capacity.</summary>
		/// <returns>Const pointer to raw container data.</returns>
		const T* GetData() const { return Data; }

		/// <summary>Element access operator for dynarray content.</summary>
		/// <param name="idx">Index of the element to access. Out of bounds values will cause UB.</param>
		/// <returns>Reference to object under provided index.</returns>
		T& operator[](size_t idx) { HEAVY_ASSERTE(idx < GetSize(), "Index out of bounds!"); return Data[idx]; }

		/// <summary>Element access operator for const dynarray content.</summary>
		/// <param name="idx">Index of the element to access. Out of bounds values will cause UB.</param>
		/// <returns>Const reference to object under provided index.</returns>
		const T& operator[](size_t idx) const { HEAVY_ASSERTE(idx < GetSize(), "Index out of bounds!"); return Data[idx]; }

		/// <summary>Clears contents of the container. This does not release aquired memory.</summary>
		void Clear() {
			for (size_t idx = 0; idx < GetSize(); ++idx)
				ObjectLifetimeHelper::Destroy(Data + idx);
			Size = 0;
		}

		/// <summary>
		/// Copies provided object to the specified location in the dynarray.
		/// Objects already present that are in position >= idx will be moved one position to the right.
		/// Insertion with idx > size results in undefined behaviour.
		/// </summary>
		/// <param name="idx">Index in which object should be created.</param>
		/// <param name="obj">Const reference to object that should be copied to the container.</param>
		void Insert(size_t idx, const T& obj)
		{
			HEAVY_ASSERTE(idx <= GetSize(), "Index out of bounds!");
			if (Size == GetCapacity())
				Enlarge();
			std::move_backward(Begin() + idx, End(), End() + 1);
			ObjectLifetimeHelper::CopyCreate(Data + idx, obj);
			++Size;
		}

		/// <summary>
		/// Removes element from the collection with specified index.
		/// Objects in position > idx will be moved one position to the left.
		/// Removal of object in idx >= size results in undefined behavour.
		/// </summary>
		/// <param name="idx">Index from which object should be removed.</param>
		void RemoveByIdx(size_t idx)
		{
			HEAVY_ASSERTE(idx < GetSize(), "Index out of bounds!");
			ObjectLifetimeHelper::Destroy(Data + idx);
			std::move(Begin() + idx + 1, End(), Begin() + idx);
			--Size;
		}

		/// <summary>Finds index of the first encountered object from the container that is equal to provided object.</summary>
		/// <param name="rhs">Searched object.</param>
		/// <returns>Index of searched object or container size if object was not found.</returns>
		size_t FindIdx(const T& rhs) const
		{
			for (size_t idx = 0; idx < GetSize(); ++idx)
			{
				if (Data[idx] == rhs)
					return idx;
			}
			return GetSize();
		}

		/// <summary>Performs insertion to the back of the container.</summary>
		/// <param name="obj">Const reference to object that should be copied to the container.</param>
		void PushBack(const T& obj) { Insert(GetSize(), obj); }

		/// <summary>Performs removal from the back of the container.</summary>
		void PopBack() { RemoveByIdx(GetSize() - 1); }

		/// <summary>Performs insertion to the front of the container.</summary>
		/// <param name="obj">Const reference to object that should be copied to the container.</param>
		void PushFront(const T& obj) { Insert(0, obj); }

		/// <summary>Performs removal from the front of the container.</summary>
		void PopFront() { RemoveByIdx(0); }

		/// <summary>
		/// Forces resizing of the collection to the specified value.
		/// If necessary this will create new object via default empty construction.
		/// This can also shrink the collection in which case, excessive objects will be destroyed.
		/// </summary>
		/// <param name="size">Requested size of the collection.</param>
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

		/// <summary>
		/// Ensures that enough space is available in the collection.
		/// In case there is not enought the container gets reallocated to new, bigger memory block.
		/// </summary>
		/// <param name="capacity">Requested capacity of the collection.</param>
		void Reserve(size_t capacity)
		{
			if (capacity <= Capacity)
				return;
			Realloc(capacity);
		}

		/// <summary>Getter for the iterator to the element in front of the collection.</summary>
		Iterator Begin() { return Iterator(Data, 0); }

		/// <summary>Getter for the iterator to the location after last element in the collection.</summary>
		Iterator End() { return Iterator(Data, GetSize()); }

		/// <summary>Getter for the const iterator to the element in front of the collection.</summary>
		ConstIterator Begin() const { return ConstIterator(Data, 0); }

		/// <summary>Getter for the const iterator to the location after last element in the collection.</summary>
		ConstIterator End() const { return ConstIterator(Data, GetSize()); }

		/// <summary>Getter for the iterator that points to the first encountered searched element in collection.</summary>
		/// <param name="rhs">Searched object.</param>
		/// <returns>Iterator Iterator to the searched object or End().</returns>
		Iterator Find(const T& rhs) { return Iterator(Data, FindIdx(rhs)); }

		/// <summary>Getter for the const iterator that points to the first encountered searched element in collection.</summary>
		/// <param name="rhs">Searched object.</param>
		/// <returns>ConstIterator Const iterator to the searched object or End().</returns>
		ConstIterator Find(const T& rhs) const { return ConstIterator(Data, FindIdx(rhs)); }

		/// <summary>Checks whether provided object is present in the collection at least once.</summary>
		/// <param name="rhs">Searched object.</param>
		/// <returns>True if present, false otherwise.</returns>
		bool Contains(const T& rhs) const { return FindIdx(rhs) < GetSize(); }

		/// <summary>
		/// Remove the first encountered object of provided value from the collection.
		/// This will cause undefined behaviour when the value is not present in the collection.
		/// Use TryRemove if not sure about object existence.
		/// </summary>
		/// <param name="rhs">Object to be removed.</param>
		void Remove(const T& rhs) { RemoveByIdx(FindIdx(rhs)); }

		/// <summary>
		/// Try to remove the first encountered object of provided value from the collection.
		/// When value is not found in collection this method does nothing.
		/// </summary>
		/// <param name="rhs">Object to be removed.</param>
		/// <returns>True if removal succeded, false if value was not found.</returns>
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
