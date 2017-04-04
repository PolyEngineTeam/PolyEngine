#pragma once

#include <Core.hpp>

namespace Poly {

	namespace ObjectLifetimeHelper
	{
		template<class T>
		void Create(void* ptr, typename std::enable_if<std::is_trivially_default_constructible<T>::value>::type* = 0)
		{
		}

		template<class T>
		void Create(void* ptr, typename std::enable_if<!std::is_trivially_default_constructible<T>::value>::type* = 0)
		{
			new (ptr)T;
		}

		template<class T>
		void Destroy(T* t, typename std::enable_if<std::is_trivially_destructible<T>::value>::type* = 0)
		{
		}

		template<class T>
		void Destroy(T* t, typename std::enable_if<!std::is_trivially_destructible<T>::value>::type* = 0)
		{
			t->~T();
		}
	}


	template<typename T>
	class Dynarray : public BaseObject<>
	{
	public:
		class Iterator : public BaseObject<>
		{
		public:
			bool operator==(const Iterator& rhs) const { return Data == rhs.Data && Idx == rhs.Idx; }
			bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }

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
		private:
			Iterator(T* data, size_t idx) : Data(data), Idx(idx) {}

			size_t Idx = 0;
			T* Data = nullptr;
			friend class Dynarray<T>;
		};

		class ConstIterator : public BaseObject<>
		{
		public:
			bool operator==(const ConstIterator& rhs) const { return Data == rhs.Data && Idx == rhs.Idx; }
			bool operator!=(const ConstIterator& rhs) const { return !(*this == rhs); }

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
		private:
			ConstIterator(const T* data, size_t idx) : Data(data), Idx(idx) {}

			size_t Idx = 0;
			const T* Data = nullptr;
			friend class Dynarray<T>;
		};

		//------------------------------------------------------------------------------
		Dynarray() = default;
		Dynarray(size_t capacity) { Reserve(capacity); }

		//------------------------------------------------------------------------------
		Dynarray(const Dynarray<T>& rhs)
		{
			Copy(rhs);
		}

		//------------------------------------------------------------------------------
		Dynarray(Dynarray<T>&& rhs)
		{
			Move(std::forward<Dynarray<T>>(rhs));
		}

		//------------------------------------------------------------------------------
		Dynarray(const std::initializer_list<T>& list)
		{
			PopulateFromInitializerList(list);
		}

		//------------------------------------------------------------------------------
		~Dynarray()
		{
			Clear();
			Free();
		}

		//------------------------------------------------------------------------------
		Dynarray<T>& operator=(const Dynarray<T>& rhs)
		{
			Clear();
			Copy(rhs);
			return *this;
		}

		//------------------------------------------------------------------------------
		Dynarray<T>& operator=(Dynarray<T>&& rhs)
		{
			Clear();
			Free();
			Move(std::forward<Dynarray<T>>(rhs));
			return *this;
		}

		//------------------------------------------------------------------------------
		Dynarray<T>& operator=(const std::initializer_list<T>& list)
		{
			Clear();
			PopulateFromInitializerList(list);
			return *this;
		}

		//------------------------------------------------------------------------------
		bool operator==(const Dynarray<T>& rhs) const
		{
			if (GetSize() != rhs.GetSize())
				return false;

			for (size_t idx = 0; idx < GetSize(); ++idx)
				if (Data[idx] != rhs.Data[idx])
					return false;

			return true;
		}

		bool operator!=(const Dynarray<T>& rhs) const { return !(*this == rhs); };

		//------------------------------------------------------------------------------
		bool IsEmpty() const { return GetSize() == 0; }
		size_t GetSize() const { return Size; }
		size_t GetCapacity() const { return Capacity; }
		T* GetData() { return Data; }
		const T* GetData() const { return Data; }

		//------------------------------------------------------------------------------
		const T& operator[](size_t idx) { return Data[idx]; }
		const T& operator[](size_t idx) const { return Data[idx]; }

		//------------------------------------------------------------------------------
		void Clear() {
			for (size_t idx = 0; idx < GetSize(); ++idx)
				ObjectLifetimeHelper::Destroy(Data + idx);
			Size = 0;
		}

		//------------------------------------------------------------------------------
		void Insert(size_t idx, const T& obj)
		{
			if (Size == GetCapacity())
				Enlarge();
			memmove(Data + idx + 1, Data + idx, (GetSize() - idx) * sizeof(T));
			Data[idx] = obj;
			++Size;
		}

		//------------------------------------------------------------------------------
		void Remove(size_t idx)
		{
			ObjectLifetimeHelper::Destroy(Data + idx);
			memmove(Data + idx, Data + idx + 1, (GetSize() - idx - 1) * sizeof(T));
			--Size;
		}

		//------------------------------------------------------------------------------
		void PushBack(const T& obj) { Insert(GetSize(), obj); }
		void PopBack() { Remove(GetSize() - 1); }
		void PushFront(const T& obj) { Insert(0, obj); }
		void PopFront() { Remove(0); }

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
					ObjectLifetimeHelper::Construct(Data + idx);
			}
			Size = size;
		}

		//------------------------------------------------------------------------------
		void Reserve(size_t capacity)
		{
			if (capacity <= Capacity)
				return;
			Realloc(capacity * sizeof(T));
		}

		//------------------------------------------------------------------------------
		Iterator Begin() { return Iterator(Data, 0); }
		Iterator End() { return Iterator(Data, GetSize()); }
		ConstIterator Begin() const { return ConstIterator(Data, 0); }
		ConstIterator End() const { return ConstIterator(Data, GetSize()); }

	private:
		//------------------------------------------------------------------------------
		void Realloc(size_t capacity)
		{
			Data = static_cast<T*>(default_realloc(Data, capacity * sizeof(T)));
			Capacity = capacity;
		}

		void Free() { if (Data) default_free(Data); }

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
